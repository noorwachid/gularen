#pragma once

#include "Node.hpp"

namespace Gularen {

    class IRenderer {
    public:
        virtual void setTree(Node *tree) = 0;

        virtual void parse() = 0;

        virtual std::string getBuffer() = 0;
    };
}
