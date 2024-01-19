#pragma once

#include <wmtk/Primitive.hpp>
#include <wmtk/utils/Rational.hpp>

#include <vector>

namespace wmtk {

class MeshWriter
{
public:
    virtual ~MeshWriter() {}

    virtual bool write(const int dim) = 0;


    virtual void write_capacities(const std::vector<int64_t>& capacities) = 0;

    virtual void write(
        const std::string& name,
        const int64_t type,
        const int64_t stride,
        const std::vector<char>& val,
        const char default_val) = 0;

    virtual void write(
        const std::string& name,
        const int64_t type,
        const int64_t stride,
        const std::vector<int64_t>& val,
        const int64_t default_val) = 0;

    virtual void write(
        const std::string& name,
        const int64_t type,
        const int64_t stride,
        const std::vector<double>& val,
        const double default_val) = 0;

    virtual void write(
        const std::string& name,
        const int64_t type,
        const int64_t stride,
        const std::vector<Rational>& val,
        const Rational& default_val) = 0;


    virtual void set_current_mesh(const Mesh& m) const;

protected:
    std::vector<int64_t> m_current_id;
    bool writing_root_mesh() const;
};

} // namespace wmtk
