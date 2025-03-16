#pragma once
#include "Dart.hpp"
#include "../PrimitiveType.hpp"

namespace wmtk::dart {
    class Involution {
        public:
            Involution(PrimitiveType spt,
            const Dart& s,
            PrimitiveType tpt, 
            const Dart& t);

            Dart apply(const Dart& d) const;
            Dart invert(const Dart& s) const;
        private:
            PrimitiveType source;
            PrimitiveType target;
            Dart involution;

    };
}
