#pragma once

namespace wmtk {
    class Mesh;
}
namespace wmtk::utils {

    // do all valdiationts on this mesh (multimesh) as possible
    bool validate(const Mesh& m);
}
