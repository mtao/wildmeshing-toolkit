#pragma once

#include <optional>
#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include "AttributeInvariant.hpp"

namespace wmtk {
namespace invariants {
class TodoInvariant : public AttributeInvariant<int64_t>
{
    /**
     * Invariant for todo-list in scheduler. Recording which simplicity still need to be operated.
     * If the todo_tag tagged as 1 then return true, otherwise return false
     */
public:
    TodoInvariant(
        const Mesh& m,
        const TypedAttributeHandle<int64_t>& todo_handle,
        const int64_t val = 1);

    TodoInvariant(const attribute::MeshAttributeHandle& mah, const int64_t val = 1);
    bool before(const simplex::Simplex& t) const override;

    std::string name() const override;

private:
    const TypedAttributeHandle<int64_t> m_todo_handle;
    const int64_t m_val;
};

namespace detail {
/// A virtual invariant class that stores an internal invariant
template <typename Scalar>
class ComparisonInvariantBase : public Invariant
{
public:
    ComparisonInvariantBase(const attribute::MeshAttributeHandle& mah)
        : m_

        ComparisonInvariantBase(
            const Mesh& m,
            const TypedAttributeHandle<double>& todo_handle,
            const TypedAttributeHandle<double>& comparison_handle);

    ComparisonInvariantBase(
        const attribute::MeshAttributeHandle& todo_handle,
        const attribute::MeshAttributeHandle& comparison_handle);

private:
    const TypedAttributeHandle<double> m_todo_handle;
    const std::optional<TypedAttributeHandle<double>> m_comparison_handle;
};

template <typename Comparison>
    requires(std::is_same_v<
             typename Comparison::first_argument_type,
             typename Comparison::second_argument_type>)
class TodoInvariant : public Invariant
{
    /**
     * Invariant for todo-list in scheduler. Recording which simplices still need to be operated.
     * If the todo_tag tagged as 1 then return true, otherwise return false
     */
public:
    using comparison_type = Comparison;
    using value_type = typename comparison_type::first_argument_type;
    TodoInvariant(const Mesh& m, const TypedAttributeHandle<double>& todo_handle, const double val);

    TodoInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& todo_handle,
        const TypedAttributeHandle<double>& comparison_handle,
        const double pre_factor = 1);

    TodoInvariant(
        const attribute::MeshAttributeHandle& todo_handle,
        const attribute::MeshAttributeHandle& comparison_handle,
        const int64_t val = 1);

    bool before(const simplex::Simplex& t) const override;

    std::string name() const override;

private:
    const TypedAttributeHandle<double> m_todo_handle;
    const std::optional<TypedAttributeHandle<double>> m_comparison_handle;
    const double m_val;
};
} // namespace detail

class TodoLargerInvariant : public Invariant
{
    /**
     * Invariant for todo-list in scheduler. Recording which simplices still need to be operated.
     * If the todo_tag tagged as 1 then return true, otherwise return false
     */
public:
    TodoLargerInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& todo_handle,
        const double val);

    TodoLargerInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& todo_handle,
        const TypedAttributeHandle<double>& comparison_handle,
        const double pre_factor = 1);

    TodoLargerInvariant(
        const attribute::MeshAttributeHandle& todo_handle,
        const attribute::MeshAttributeHandle& comparison_handle,
        const int64_t val = 1);

    bool before(const simplex::Simplex& t) const override;

    std::string name() const override;

private:
    const TypedAttributeHandle<double> m_todo_handle;
    const std::optional<TypedAttributeHandle<double>> m_comparison_handle;
    const double m_val;
};

class TodoSmallerInvariant : public Invariant
{
    /**
     * Invariant for todo-list in scheduler. Recording which simplices still need to be operated.
     * If the todo_tag tagged as 1 then return true, otherwise return false
     */
public:
    TodoSmallerInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& todo_handle,
        const double val);

    TodoSmallerInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& todo_handle,
        const TypedAttributeHandle<double>& comparison_handle,
        const double pre_factor = 1);

    bool before(const simplex::Simplex& t) const override;

    std::string name() const override;

private:
    const TypedAttributeHandle<double> m_todo_handle;
    const std::optional<TypedAttributeHandle<double>> m_comparison_handle;
    const double m_val;
};
} // namespace invariants


using TodoInvariant = invariants::TodoInvariant;
using TodoLargerInvariant = invariants::TodoLargerInvariant;
using TodoSmallerInvariant = invariants::TodoSmallerInvariant;
} // namespace wmtk
