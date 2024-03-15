#include "TetEdgeSwap.hpp"
#include <wmtk/simplex/closed_star.hpp>
#include <wmtk/simplex/open_star.hpp>
#include <wmtk/simplex/utils/SimplexComparisons.hpp>


#include <wmtk/Mesh.hpp>

namespace wmtk::operations::composite {
TetEdgeSwap::TetEdgeSwap(Mesh& m, int64_t collapse_index)
    : Operation(m)
    , m_split(m)
    , m_collapse(m)
    , m_collapse_index(collapse_index)
{}

std::vector<simplex::Simplex> TetEdgeSwap::execute(const simplex::Simplex& simplex)
{
    const auto split_simplicies = m_split(simplex);
    if (split_simplicies.empty()) return {};
    assert(split_simplicies.size() == 1);

    // after split
    const Tuple& split_ret = split_simplicies.front().tuple();
    assert(!mesh().is_boundary(split_simplicies.front()));
    auto iter_tuple = split_ret;
    std::vector<Tuple> candidate_edge_tuples;
    while (true) {
        candidate_edge_tuples.push_back(mesh().switch_tuple(iter_tuple, PrimitiveType::Edge));
        iter_tuple =
            mesh().switch_tuples(iter_tuple, {PrimitiveType::Triangle, PrimitiveType::Tetrahedron});
        if (iter_tuple == split_ret) {
            break;
        }
    }

    // collapse index failed, return empty
    if (m_collapse_index >= candidate_edge_tuples.size()) return {};

    const Tuple& collapse_tuple = candidate_edge_tuples[m_collapse_index];


    // before collapse, compute the edges generated by split - edges deleted by collapse

    // auto edges_generated_by_split = m_split.new_edge_tuples();
    const auto split_tuple_open_star =
        simplex::open_star(mesh(), simplex::Simplex::vertex(split_ret));
    std::array<std::vector<simplex::Simplex>, 4> simplices_generated_by_split;
    for (const simplex::Simplex& s : split_tuple_open_star.simplex_vector()) {
        simplices_generated_by_split[get_primitive_type_id(s.primitive_type())].emplace_back(s);
    }

    const auto v_open_star = simplex::open_star(mesh(), simplex::Simplex::vertex(collapse_tuple));
    const auto e_closed_star = simplex::closed_star(mesh(), simplex::Simplex::edge(collapse_tuple));
    const auto sc = simplex::SimplexCollection::get_intersection(v_open_star, e_closed_star);

    std::array<std::vector<simplex::Simplex>, 4> simplices_deleted_by_collapse;

    for (const simplex::Simplex& s : sc.simplex_vector()) {
        simplices_deleted_by_collapse[get_primitive_type_id(s.primitive_type())].emplace_back(s);
    }

    // check if #edge_generated_by_split > #edges_deleted_by_collapse, if not, return face instead
    assert(simplices_generated_by_split[1].size() >= simplices_deleted_by_collapse[1].size());

    bool able_to_return_edges = true;
    if (simplices_generated_by_split[1].size() == simplices_deleted_by_collapse[1].size())
        able_to_return_edges = false;


    std::vector<simplex::Simplex> edges_generated_by_swap;
    std::vector<simplex::Simplex> faces_generated_by_swap;

    // add compare lambda here
    auto comp_simplex = [&](const simplex::Simplex& s1, const simplex::Simplex& s2) {
        return simplex::utils::SimplexComparisons::less(mesh(), s1, s2);
    };

    if (able_to_return_edges) {
        std::sort(
            simplices_generated_by_split[1].begin(),
            simplices_generated_by_split[1].end(),
            comp_simplex);
        std::sort(
            simplices_deleted_by_collapse[1].begin(),
            simplices_deleted_by_collapse[1].end(),
            comp_simplex);

        std::set_difference(
            simplices_generated_by_split[1].begin(),
            simplices_generated_by_split[1].end(),
            simplices_deleted_by_collapse[1].begin(),
            simplices_deleted_by_collapse[1].end(),
            std::back_inserter(edges_generated_by_swap),
            comp_simplex);

        assert(
            edges_generated_by_swap.size() ==
            (simplices_generated_by_split[1].size() - simplices_deleted_by_collapse[1].size()));
    } else {
        // return face
        // std::cout << "swap returns face" << std::endl;
        assert(simplices_generated_by_split[2].size() >= simplices_deleted_by_collapse[2].size());

        std::sort(
            simplices_generated_by_split[2].begin(),
            simplices_generated_by_split[2].end(),
            comp_simplex);
        std::sort(
            simplices_deleted_by_collapse[2].begin(),
            simplices_deleted_by_collapse[2].end(),
            comp_simplex);

        std::set_difference(
            simplices_generated_by_split[2].begin(),
            simplices_generated_by_split[2].end(),
            simplices_deleted_by_collapse[2].begin(),
            simplices_deleted_by_collapse[2].end(),
            std::back_inserter(faces_generated_by_swap),
            comp_simplex);

        assert(
            faces_generated_by_swap.size() ==
            (simplices_generated_by_split[2].size() - simplices_deleted_by_collapse[2].size()));
        assert(faces_generated_by_swap.size() == 1);

        if (faces_generated_by_swap.size() != 1) {
            std::cout << "swap return more than one face!!" << std::endl;
            throw std::runtime_error("faces_generated_by_swap.size() != 1");
        }
    }


    // do collapse
    const auto collapse_simplicies =
        m_collapse(simplex::Simplex(m_collapse.primitive_type(), collapse_tuple));
    if (collapse_simplicies.empty()) return {};
    assert(collapse_simplicies.size() == 1);

    if (able_to_return_edges) {
        for (int64_t i = 0; i < edges_generated_by_swap.size(); ++i) {
            edges_generated_by_swap[i] =
                simplex::Simplex::edge(resurrect_tuple(edges_generated_by_swap[i].tuple()));
        }

        return edges_generated_by_swap;
    } else {
        for (int64_t i = 0; i < faces_generated_by_swap.size(); ++i) {
            faces_generated_by_swap[i] =
                simplex::Simplex::face(resurrect_tuple(faces_generated_by_swap[i].tuple()));
        }

        return faces_generated_by_swap;
    }
}

std::vector<simplex::Simplex> TetEdgeSwap::unmodified_primitives(
    const simplex::Simplex& simplex) const
{
    return {simplex};
}

} // namespace wmtk::operations::composite
