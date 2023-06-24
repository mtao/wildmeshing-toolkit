#include <wmtk/utils/trimesh_topology_initialization.h>
#include <wmtk/utils/tetmesh_topology_initialization.h>

#include <wmtk/Mesh.hpp>

#include <catch2/catch.hpp>

#include <igl/read_triangle_mesh.h>

#include <stdlib.h>
#include <iostream>

#include <wmtk/utils/Logger.hpp>

using namespace wmtk;

TEST_CASE("load mesh from libigl and test mesh topology on a single triangle", "[test_topology_single_triangle]")
{
    Eigen::Matrix<long, 1, 3> F;
    F << 0, 1, 2;
    auto [FE, FF, VF, EF] = trimesh_topology_initialization(F);

    // std::cout << "F:\n" << F << std::endl;
    // std::cout << "FE:\n" << FE << std::endl;
    // std::cout << "FF:\n" << FF << std::endl;
    // std::cout << "VF:\n" << VF << std::endl;
    // std::cout << "EF:\n" << EF << std::endl;

    // 1. Test relationship between EF and FE
    for (int i = 0; i < EF.size(); ++i)
    {
        CHECK((FE.row(EF(i)).array() == i).any());
    }

    // 2. Test relationship between VF and F
    for (int i = 0; i < VF.size(); ++i)
    {
        CHECK((F.row(VF(i)).array() == i).any());
    }
    
    // 3. Test relationship between FF and FE
    for (int i = 0; i < FF.rows(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            long nb = FF(i, j);
            if (nb < 0) 
                continue;
            
            CHECK((FF.row(nb).array() == i).any());

            if ((FF.row(nb).array() == i).any())
            {
                int cnt = (FF.row(nb).array() == i).count();
                CHECK(cnt == 1);

                auto is_nb = (FF.row(nb).array() == i);
                for (int k = 0; k < 3; ++k)
                {
                    if (is_nb(k))
                    {
                        CHECK(FE(i, j) == FE(nb, k));
                    }
                }
            }    
        }
    }
}

TEST_CASE("load mesh from libigl and test mesh topology on two triangles", "[test_topology_two_triangles]")
{
    Eigen::Matrix<long, 2, 3> F;
    F << 0, 1, 2, 
        1, 3, 2;

    auto [FE, FF, VF, EF] = trimesh_topology_initialization(F);

    // std::cout << "F:\n" << F << std::endl;
    // std::cout << "FE:\n" << FE << std::endl;
    // std::cout << "FF:\n" << FF << std::endl;
    // std::cout << "VF:\n" << VF << std::endl;
    // std::cout << "EF:\n" << EF << std::endl;

    // 1. Test relationship between EF and FE
    for (int i = 0; i < EF.size(); ++i)
    {
        CHECK((FE.row(EF(i)).array() == i).any());
    }

    // 2. Test relationship between VF and F
    for (int i = 0; i < VF.size(); ++i)
    {
        CHECK((F.row(VF(i)).array() == i).any());
    }
    
    // 3. Test relationship between FF and FE
    for (int i = 0; i < FF.rows(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            long nb = FF(i, j);
            if (nb < 0) 
                continue;
            
            CHECK((FF.row(nb).array() == i).any());

            if ((FF.row(nb).array() == i).any())
            {
                int cnt = (FF.row(nb).array() == i).count();
                CHECK(cnt == 1);

                auto is_nb = (FF.row(nb).array() == i);
                for (int k = 0; k < 3; ++k)
                {
                    if (is_nb(k))
                    {
                        CHECK(FE(i, j) == FE(nb, k));
                    }
                }
            }    
        }
    }
}

TEST_CASE("load mesh from libigl and test mesh topology", "[test_topology_2D]")
{
    Eigen::MatrixXd V;
    Eigen::Matrix<long, -1, -1> F;

    std::vector<std::string> names = {
        "/Octocat.obj", 
        "/armadillo.obj",
        "/blub.obj",
        "/bunny.obj",
        "/circle.obj",
        "/fan.obj",
        "/sphere.obj",
        "/test_triwild.obj",
        "/hemisphere.obj"
        };

    for (auto name : names)
    {
        std::string path;
        path.append(WMTK_DATA_DIR);
        path.append(name);
        igl::read_triangle_mesh(path, V, F);

        auto [FE, FF, VF, EF] = trimesh_topology_initialization(F);

        // std::cout << "F:\n" << F << std::endl;
        // std::cout << "FE:\n" << FE << std::endl;
        // std::cout << "FF:\n" << FF << std::endl;
        // std::cout << "VF:\n" << VF << std::endl;
        // std::cout << "EF:\n" << EF << std::endl;

        // 1. Test relationship between EF and FE
        for (int i = 0; i < EF.size(); ++i)
        {
            CHECK((FE.row(EF(i)).array() == i).any());
        }

        // 2. Test relationship between VF and F
        for (int i = 0; i < VF.size(); ++i)
        {
            if (VF(i) < 0)
                continue;
            CHECK((F.row(VF(i)).array() == i).any());
        }
        
        // 3. Test relationship between FF and FE
        for (int i = 0; i < FF.rows(); ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                long nb = FF(i, j);
                if (nb < 0) 
                    continue;
                
                CHECK((FF.row(nb).array() == i).any());

                if ((FF.row(nb).array() == i).any())
                {
                    int cnt = (FF.row(nb).array() == i).count();
                    CHECK(cnt == 1);

                    auto is_nb = (FF.row(nb).array() == i);
                    for (int k = 0; k < 3; ++k)
                    {
                        if (is_nb(k))
                        {
                            // wmtk::logger().info("{} {} {} {}", i, j, nb, k);
                            CHECK(FE(i, j) == FE(nb, k));
                        }
                    }
                }    
            }
        }
    }
}

TEST_CASE("tetmesh_topology_initialization_1", "[test_topology]")
{
    Eigen::Matrix<long, 2, 4> T;
    T << 
    0, 1, 2, 3,
    1, 2, 3, 4;

    auto [TE,TF,TT,FT,ET,VT] = tetmesh_topology_initialization(T);

    std::cout << "T: \n" << T << std::endl;
    std::cout << "TE: \n" << TE << std::endl;
    std::cout << "TF: \n" << TF << std::endl;
    std::cout << "TT: \n" << TT << std::endl;
    std::cout << "FT: \n" << FT << std::endl;
    std::cout << "ET: \n" << ET << std::endl;
    std::cout << "VT: \n" << VT << std::endl;

    
    // // 1. Test relationship between EF and FE
    // for (size_t i = 0; i < EF.size(); ++i)
    // {
    //     CHECK((FE.row(EF(i)).array() == i).any());
    // }

    // // 2. Test relationship between VF and F
    // for (size_t i = 0; i < VF.size(); ++i)
    // {
    //     CHECK((F.row(VF(i)).array() == i).any());
    // }
    
    // // 3. Test relationship between FF and FE
    // for (size_t i = 0; i < FF.rows(); ++i)
    // {
    //     for (size_t j = 0; j < 3; ++j)
    //     {
    //         long nb = FF(i, j);
    //         if (nb < 0) 
    //             continue;
            
    //         CHECK((FF.row(nb).array() == i).any());

    //         if ((FF.row(nb).array() == i).any())
    //         {
    //             int cnt = (FF.row(nb).array() == i).count();
    //             CHECK(cnt == 1);

    //             auto is_nb = (FE.row(nb).array() == i);
    //             for (size_t k = 0; k < 3; ++k)
    //             {
    //                 if (is_nb(k))
    //                 {
    //                     CHECK(FE(i, j) == FE(nb, k));
    //                 }
    //             }
    //         }    
    //     }
    // }
}