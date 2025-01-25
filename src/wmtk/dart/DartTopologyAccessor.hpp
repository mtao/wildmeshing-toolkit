#pragma once
#include <wmtk/dart/find_local_dart_action.hpp>
#include "DartAccessor.hpp"
#include "utils/get_simplex_involution.hpp"
namespace wmtk::dart {

template <int Dim, typename MeshType>
class DartTopologyAccessor : public DartAccessor<Dim, MeshType>
{
public:
    using BaseType = DartAccessor<Dim, MeshType>;
    using IndexBaseType = typename BaseType::IndexBaseType;

public:
    using BaseType::BaseType;

    using IndexBaseType::mesh;
    static wmtk::attribute::TypedAttributeHandle<int64_t> register_boundary_topology_attribute(
        MeshType& m,
        const std::string_view& name,
        bool do_populate = false)
    {
        auto handle = BaseType::register_attribute(m, name, m.top_simplex_type() - 1);
        if (do_populate) {
            DartTopologyAccessor acc(m, handle);
            acc.populate();
        }
        return handle;
    }
    template <typename IT, typename OT>
    dart::Dart switch_facet(const dart::_Dart<IT, OT>& d) const
    {
        return switch_facet(d.global_id(), d.permutation());
    }

    template <typename IT, typename OT>
    dart::Dart switch_dart(const dart::_Dart<IT, OT>& d, PrimitiveType pt) const
    {
        const PrimitiveType FT = mesh().top_simplex_type();
        if (pt == FT) {
            return switch_facet(d);
        } else {
            const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(FT);
            dart::Dart ret = sd.act(d, sd.primitive_as_index(pt));
            return ret;
        }
    }
    template <typename IT, typename OT>
    bool is_boundary(const dart::_Dart<IT, OT>& d) const
    {
        return get_neighbor(d).is_null();
    }

    template <typename IT, typename OT>
    int8_t get_boundary_local_index(const dart::_Dart<IT, OT>& d) const
    {
        const PrimitiveType FT = mesh().top_simplex_type();
        const PrimitiveType BT = FT - 1;
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(FT);
        return sd.simplex_index(d, BT);
    }
    template <typename IT, typename OT>
    dart::Dart get_neighbor(const dart::_Dart<IT, OT>& d) const
    {
        const auto& sa = IndexBaseType::operator[](d.global_id());
        return sa[get_boundary_local_index(d)];
    }
    template <typename IT, typename OT>
    dart::DartWrap get_neighbor(const dart::_Dart<IT, OT>& d)
    {
        auto& sa = IndexBaseType::operator[](d.global_id());
        return sa[get_boundary_local_index(d)];
    }
    void fuse(const dart::Dart& d, const dart::Dart& od)
    {
        const PrimitiveType FT = mesh().top_simplex_type();
        auto a = get_neighbor(d);
        auto b = get_neighbor(od);
        std::tie(a, b) = utils::get_simplex_involution_pair(FT, d, FT, od);
#if !defined(NDEBUG)
        auto sa = utils::get_simplex_involution(FT, d, FT, od);
        auto sb = utils::get_simplex_involution(FT, od, FT, d);
        assert(get_neighbor(d) == sa);
        assert(get_neighbor(od) == sb);
#endif
    }
    void populate()
    {
        const PrimitiveType FT = mesh().top_simplex_type();
        const PrimitiveType BT = FT - 1;
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(FT);
        for (Tuple t : mesh().get_all(BT)) {
            dart::Dart d = sd.dart_from_tuple(t);
            if (mesh().is_boundary(BT, t)) {
                get_neighbor(d) = dart::Dart();
            } else {
                const Tuple ot = mesh().switch_tuple(t, FT);
                assert(ot != t);
                const dart::Dart od = sd.dart_from_tuple(ot);
                fuse(d, od);
            }
        }
    }

private:
    dart::Dart switch_facet(int64_t global_id, int8_t permutation) const
    {
        PrimitiveType FT = mesh().top_simplex_type();
        PrimitiveType BT = FT - 1;
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(FT);


        const auto anchor =
            IndexBaseType::operator[](global_id)[sd.simplex_index(permutation, BT)];


        return dart::Dart(
            anchor.global_id(),
            sd.product(anchor.permutation(), permutation));
    }
};

template <typename Handle>
DartTopologyAccessor(const PointMesh& p, const Handle&) -> DartTopologyAccessor<2, PointMesh>;
template <typename Handle>
DartTopologyAccessor(const EdgeMesh&, const Handle&) -> DartTopologyAccessor<3, EdgeMesh>;
template <typename Handle>
DartTopologyAccessor(const TriMesh&, const Handle&) -> DartTopologyAccessor<4, TriMesh>;
template <typename Handle>
DartTopologyAccessor(const TetMesh&, const Handle&) -> DartTopologyAccessor<5, TetMesh>;


template <typename MeshType>
auto register_dart_boundary_topology_attribute(
    MeshType& mesh,
    const std::string_view& name,
    bool do_populate = false)
{
    return decltype(DartTopologyAccessor(
        mesh,
        std::declval<wmtk::attribute::MeshAttributeHandle>()))::
        register_boundary_topology_attribute(mesh, name, do_populate);
}

} // namespace wmtk::dart
