#pragma once

#include "../Node.h";

namespace Gularen {
    class Transpiler {
    public:
        virtual String transpile(const RC<Node>& node);
    };
}