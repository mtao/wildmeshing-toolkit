#include "Models.h"
#include <igl/readOBJ.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ExtremeOpt.h"

namespace extremeopt::tests {
// singleton pattern that computes
namespace {

bool find_edge_in_F(

    const std::map<std::array<int, 2>, std::set<int>>& EFidx,
    const Eigen::MatrixXi& F,
    int v0,
    int v1,
    int& fid,
    int& eid)
{
    fid = -1;
    eid = -1;
    std::array<int, 2> arr{{v0, v1}};
    if (EFidx.find(arr) == EFidx.end()) {
        return false;
    }
    for (int i : EFidx.at(arr)) {
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
    std::map<std::array<int, 2>, std::set<int>> EFidx;
    for (int i = 0; i < F.rows(); i++) {
        auto f = F.row(i);
        for (int j = 0; j < 3; j++) {
            int a = f(j);
            int b = f((j + 1) % 3);
            EFidx[std::array<int, 2>{{a, b}}].insert(i);
            EFidx[std::array<int, 2>{{b, a}}].insert(i);
        }
    }

    EE_e.resize(EE_v.rows());
    for (int i = 0; i < EE_v.rows(); i++) {
        std::vector<int> one_row;
        int v0 = EE_v(i, 0), v1 = EE_v(i, 1);
        int fid, eid;
        if (find_edge_in_F(EFidx, F, v0, v1, fid, eid)) {
            one_row.push_back(fid);
            one_row.push_back(eid);
            // one_row.push_back(3 * fid + eid);
        } else {
            std::cout << "Something Wrong in transform_EE: edge not found in F" << std::endl;
        }

        v0 = EE_v(i, 2);
        v1 = EE_v(i, 3);
        if (find_edge_in_F(EFidx, F, v0, v1, fid, eid)) {
            one_row.push_back(fid);
            one_row.push_back(eid);
            // one_row.push_back(3 * fid + eid);
        } else {
            std::cout << "Something Wrong in transform_EE: edge not found in F" << std::endl;
        }
        EE_e[i] = one_row;
    }
}
std::map<std::string, ModelFiles> generate_model_file_data()
{
    std::filesystem::path data_base_path(WMT_DATA_DIR);
    assert(std::filesystem::exists(data_base_path));
    std::filesystem::path seamed_path = data_base_path / "seamed";
    std::filesystem::path file_list_file = seamed_path / "files.js";
    std::ifstream ifs(file_list_file);
    nlohmann::json files;
    ifs >> files;

    std::map<std::string, ModelFiles> ret;
    for (const auto& [modelname, files] : files.items()) {
        ModelFiles r;
        r.obj_filepath = seamed_path / files["filename"];
        if (files.contains("EEfilename")) {
            r.constraint_filepath = seamed_path / files["EEfilename"];
        }
        ret[modelname] = std::move(r);
    }

    return ret;
}
const static std::map<std::string, ModelFiles> all_model_files = generate_model_file_data();
} // namespace

// returns a sequence of model names
std::vector<std::string> get_model_names()
{
    std::vector<std::string> ret;
    ret.reserve(all_model_files.size());
    for (const auto& [filename, data] : all_model_files) {
        ret.emplace_back(filename);
    }
    return ret;
}


// returns the obj file and the EE file (if it exists)
ModelFiles get_model_files(const std::string& name)
{
    return all_model_files.at(name);
}

// returns all of the model files we have as pairs of example names and
std::map<std::string, ModelFiles> get_all_model_files()
{
    return all_model_files;
}

void create(const std::string& model_name, ExtremeOpt& mesh)
{
    create(get_model_files(model_name), mesh);
}
void create(const ModelFiles& model_files, ExtremeOpt& mesh)
{
    Eigen::MatrixXd V, UV, N;
    Eigen::MatrixXi F, FUV, FN;
    spdlog::info("Read mesh");
    igl::readOBJ(model_files.obj_filepath, V, UV, N, F, FUV, FN);

    spdlog::info("create mesh");
    mesh.create_mesh(V, F, UV);

    if (model_files.constraint_filepath) {
        // Loading the seamless boundary constraints
        spdlog::info("read constraints");
        Eigen::MatrixXi EE;
        int EE_rows;
        std::ifstream EE_in(*model_files.constraint_filepath);
        EE_in >> EE_rows;
        EE.resize(EE_rows, 4);
        for (int i = 0; i < EE.rows(); i++) {
            EE_in >> EE(i, 0) >> EE(i, 1) >> EE(i, 2) >> EE(i, 3);
        }

        std::vector<std::vector<int>> EE_e;
        spdlog::info("transform constraints");
        transform_EE(F, EE, EE_e);
        spdlog::info("init constraints");
        mesh.init_constraints(EE_e);
    }
}

} // namespace extremeopt::tests
