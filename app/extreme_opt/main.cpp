#include <spdlog/common.h>
#include "ExtremeOpt.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#elif (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <CLI/CLI.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#pragma GCC diagnostic pop
#endif
#include <fstream>

#include <igl/PI.h>
#include <igl/boundary_loop.h>
#include <igl/read_triangle_mesh.h>
#include <igl/upsample.h>
#include <igl/writeOBJ.h>
#include <nlohmann/json.hpp>

double check_constraints(
    const Eigen::MatrixXi& EE,
    const Eigen::MatrixXd& uv,
    const Eigen::MatrixXi& F,
    Eigen::SparseMatrix<double> Aeq)
{
    int N = uv.rows();
    int c = 0;
    int m = EE.rows() / 2;

    std::vector<std::vector<int>> bds;
    igl::boundary_loop(F, bds);

    // if there are no constraints then the constraint error should be 0?
    if (m == 0) {
        return 0.0;
    }
    double ret = 0;
    std::set<std::pair<int, int>> added_e;
    Aeq.resize(2 * m, uv.rows() * 2);
    for (int i = 0; i < EE.rows(); i++) {
        int A2 = EE(i, 0);
        int B2 = EE(i, 1);
        int C2 = EE(i, 2);
        int D2 = EE(i, 3);
        auto e0 = std::make_pair(A2, B2);
        auto e1 = std::make_pair(C2, D2);
        if (added_e.find(e0) != added_e.end() || added_e.find(e1) != added_e.end()) continue;
        added_e.insert(e0);
        added_e.insert(e1);

        Eigen::Vector2d e_ab = uv.row(B2) - uv.row(A2);
        Eigen::Vector2d e_dc = uv.row(C2) - uv.row(D2);

        Eigen::Vector2d e_ab_perp;
        e_ab_perp(0) = -e_ab(1);
        e_ab_perp(1) = e_ab(0);
        double angle = atan2(-e_ab_perp.dot(e_dc), e_ab.dot(e_dc));
        int r = (int)(round(2 * angle / igl::PI) + 2) % 4;

        std::vector<Eigen::Matrix<double, 2, 2>> r_mat(4);
        r_mat[0] << -1, 0, 0, -1;
        r_mat[1] << 0, 1, -1, 0;
        r_mat[2] << 1, 0, 0, 1;
        r_mat[3] << 0, -1, 1, 0;

        Aeq.coeffRef(c, A2) += 1;
        Aeq.coeffRef(c, B2) += -1;
        Aeq.coeffRef(c + 1, A2 + N) += 1;
        Aeq.coeffRef(c + 1, B2 + N) += -1;

        Aeq.coeffRef(c, C2) += r_mat[r](0, 0);
        Aeq.coeffRef(c, D2) += -r_mat[r](0, 0);
        Aeq.coeffRef(c, C2 + N) += r_mat[r](0, 1);
        Aeq.coeffRef(c, D2 + N) += -r_mat[r](0, 1);
        Aeq.coeffRef(c + 1, C2) += r_mat[r](1, 0);
        Aeq.coeffRef(c + 1, D2) += -r_mat[r](1, 0);
        Aeq.coeffRef(c + 1, C2 + N) += r_mat[r](1, 1);
        Aeq.coeffRef(c + 1, D2 + N) += -r_mat[r](1, 1);
        c = c + 2;
    }

    Eigen::VectorXd flat_uv = Eigen::Map<const Eigen::VectorXd>(uv.data(), uv.size());
    auto res = Aeq * flat_uv;
    ret = res.cwiseAbs().maxCoeff();

    return ret;
}

bool find_edge_in_F(const Eigen::MatrixXi& F, int v0, int v1, int& fid, int& eid)
{
    fid = -1;
    eid = -1;
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < 3; j++) {
            if (F(i, j) == v0 && F(i, (j + 1) % 3) == v1) {
                fid = i;
                eid = 3 - j - ((j + 1) % 3);
                return true;
            }
        }
    }
    return false;
}
void transform_EE(
    const Eigen::MatrixXi& F,
    const Eigen::MatrixXi& EE_v,
    std::vector<std::vector<int>>& EE_e)
{
    EE_e.resize(EE_v.rows());
    for (int i = 0; i < EE_v.rows(); i++) {
        std::vector<int> one_row;
        int v0 = EE_v(i, 0), v1 = EE_v(i, 1);
        int fid, eid;
        if (find_edge_in_F(F, v0, v1, fid, eid)) {
            one_row.push_back(fid);
            one_row.push_back(eid);
            // one_row.push_back(3 * fid + eid);
        } else {
            std::cout << "Something Wrong in transform_EE: edge not found in F" << std::endl;
        }

        v0 = EE_v(i, 2);
        v1 = EE_v(i, 3);
        if (find_edge_in_F(F, v0, v1, fid, eid)) {
            one_row.push_back(fid);
            one_row.push_back(eid);
            // one_row.push_back(3 * fid + eid);
        } else {
            std::cout << "Something Wrong in transform_EE: edge not found in F" << std::endl;
        }
        EE_e[i] = one_row;
    }
}

bool uniform_upsample_with_cons(
    const Eigen::MatrixXd& V,
    const Eigen::MatrixXd& uv,
    const Eigen::MatrixXi& F,
    Eigen::MatrixXi& EE,
    Eigen::MatrixXd& new_V,
    Eigen::MatrixXd& new_uv,
    Eigen::MatrixXi& new_F,
    double eps = 1e-6)
{
    igl::upsample(V, F, new_V, new_F);
    igl::upsample(uv, F, new_uv, new_F);

    std::cout << "F size " << F.rows() << " --> " << new_F.rows() << std::endl;
    std::cout << "V size " << V.rows() << " --> " << new_V.rows() << std::endl;
    std::vector<std::vector<int>> new_bds;
    igl::boundary_loop(new_F, new_bds);
    auto new_bd = new_bds[0];
    Eigen::MatrixXi new_EE(EE.rows() * 2, 4);
    for (int i = 0; i < EE.rows(); i++) {
        int A = EE(i, 0);
        int B = EE(i, 1);
        int C = EE(i, 2);
        int D = EE(i, 3);
        int posA = std::find(new_bd.begin(), new_bd.end(), A) - new_bd.begin();
        if (posA == new_bd.size()) {
            std::cout << "cant find A" << std::endl;
        }
        if (new_bd[(posA + 2) % new_bd.size()] != B) {
            std::cout << "cant find B" << std::endl;
        }
        int posC = std::find(new_bd.begin(), new_bd.end(), C) - new_bd.begin();
        if (posC == new_bd.size()) {
            std::cout << "cant find C" << std::endl;
        }
        if (new_bd[(posC + 2) % new_bd.size()] != D) {
            std::cout << "cant find D" << std::endl;
        }
        int new_v_AB = new_bd[(posA + 1) % new_bd.size()];
        int new_v_CD = new_bd[(posC + 1) % new_bd.size()];
        new_EE.row(2 * i) << A, new_v_AB, new_v_CD, D;
        new_EE.row(2 * i + 1) << new_v_AB, B, C, new_v_CD;
    }

    Eigen::SparseMatrix<double> new_Aeq;
    double new_cons_residual = check_constraints(new_EE, new_uv, new_F, new_Aeq);
    wmtk::logger().info("constraints error after upsample {}", new_cons_residual);

    EE = new_EE;
    if (new_cons_residual > eps) {
        return false;
    } else {
        return true;
    }
}


int main(int argc, char** argv)
{
    ZoneScopedN("extreme_opt_main");

    CLI::App app{argv[0]};
    std::string input_dir = "./objs";
    std::string output_dir = "./test_out";
    std::string input_json = "../config/config.json";
    std::string model = "eight";
    extremeopt::Parameters param;
    app.add_option("-i,--input", input_dir, "Input mesh dir.");
    app.add_option("-m,--model", model, "Input model name.");
    app.add_option("-j,--json", input_json, "Input arguments.");
    app.add_option("-o,--output", output_dir, "Output dir.");

    CLI11_PARSE(app, argc, argv);


    std::string input_file = input_dir + "/" + model + "_init.obj";
    // Loading the input mesh
    Eigen::MatrixXd V, uv;
    Eigen::MatrixXi F;
    igl::readOBJ(input_file, V, uv, uv, F, F, F);
    wmtk::logger()
        .info("Input mesh F size {}, V size {}, uv size {}", F.rows(), V.rows(), uv.rows());

    // Loading the seamless boundary constraints
    Eigen::MatrixXi EE;
    int EE_rows;
    std::ifstream EE_in(input_dir + "/EE/" + model + "_EE.txt");
    EE_in >> EE_rows;
    EE.resize(EE_rows, 4);
    for (int i = 0; i < EE.rows(); i++) {
        EE_in >> EE(i, 0) >> EE(i, 1) >> EE(i, 2) >> EE(i, 3);
    }
    wmtk::logger().info("Input EE size {}", EE.rows());
    Eigen::SparseMatrix<double> Aeq;
    double cons_residual = check_constraints(EE, uv, F, Aeq);
    wmtk::logger().info("Initial constraints error {}", cons_residual);

    std::ifstream js_in(input_json);
    nlohmann::json config = nlohmann::json::parse(js_in);
    
    param.load(config);
    param.model_name = model;
    nlohmann::json opt_log;
    opt_log["model_name"] = model;
    opt_log["args"] = config;
    std::ofstream js_out(output_dir + "/" + model + ".json");

    std::vector<std::vector<int>> bds;
    igl::boundary_loop(F, bds);
    std::cout << "#bd loops:" << bds.size() << std::endl;
    int Nv_bds = 0;
    for (auto bd : bds) Nv_bds += bd.size();
    wmtk::logger().info("Boundary size: {}", Nv_bds);


    // std::cout << "try upsample constraints" << std::endl;
    Eigen::MatrixXi new_F;
    Eigen::MatrixXd new_V, new_uv;
    // uniform_upsample_with_cons(V, uv, F, EE, new_V, new_uv, new_F);
    // Load the mesh in the trimesh class

    extremeopt::ExtremeOpt extremeopt;
    // uniform_upsample_with_cons(V, uv, F, EE, new_V, new_uv, new_F);
    extremeopt.m_params = param;
    
    extremeopt.create_mesh(V, F, uv);
    // return -1;



    if (extremeopt.m_params.with_cons)
    {
        std::vector<std::vector<int>> EE_e;
        transform_EE(F, EE, EE_e);
        extremeopt.init_constraints(EE_e);
        assert(extremeopt.check_mesh_connectivity_validity());
        std::cout << "check constraints inside wmtk" << std::endl;
        if (extremeopt.check_constraints()) {
            std::cout << "initial constraints satisfied" << std::endl;
        } else {
            std::cout << "fails" << std::endl;
        }
    }

    extremeopt.do_optimization(opt_log);
    if (extremeopt.m_params.with_cons)
    {
        std::cout << "check constraints inside wmtk" << std::endl;
        if (extremeopt.check_constraints()) {
            std::cout << "constraints satisfied" << std::endl;
        } else {
            std::cout << "fails" << std::endl;
        }
    }
    
    extremeopt.export_mesh(V, F, uv);

    if (extremeopt.m_params.with_cons) extremeopt.export_EE(EE);

    for (int i = 0; i < param.global_upsample; i++)
    {
        uniform_upsample_with_cons(V, uv, F, EE, new_V, new_uv, new_F);
        V = new_V;
        uv = new_uv;
        F = new_F;

        extremeopt::ExtremeOpt extremeopt1;
        extremeopt1.create_mesh(V, F, uv);
        extremeopt1.m_params = param;
        std::vector<std::vector<int>> EE_e;
        transform_EE(F, EE, EE_e);
        extremeopt1.init_constraints(EE_e);
        std::cout << "check constraints inside wmtk" << std::endl;
        if (extremeopt1.check_constraints()) {
            std::cout << "initial constraints satisfied" << std::endl;
        } else {
            std::cout << "fails" << std::endl;
        }

        extremeopt1.do_optimization(opt_log);

        std::cout << "check constraints inside wmtk" << std::endl;
        if (extremeopt1.check_constraints()) {
            std::cout << "constraints satisfied" << std::endl;
        } else {
            std::cout << "fails" << std::endl;
        }
        extremeopt1.export_mesh(V, F, uv);
        extremeopt1.export_EE(EE);
    }
    igl::writeOBJ(output_dir + "/" + model + "_out.obj", V, F, V, F, uv, F);
    
    if (extremeopt.m_params.with_cons)
    {
        std::ofstream EE_out(output_dir + "/EE/" + model + "_EE.txt");
        EE_out << EE << std::endl;
    }
    

    js_out << std::setw(4) << opt_log << std::endl;
    return 0;
}
