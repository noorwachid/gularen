#pragma once

#include "Node.h"

namespace Gularen
{
    struct BreakNode : Node
    {
        BreakNode(NodeType type, NodeGroup group);

        virtual String ToString() const override;
    };

    struct LineBreakNode : BreakNode
    {
        LineBreakNode();

        virtual String ToString() const override;
    };

    struct PageBreakNode : BreakNode
    {
        PageBreakNode();

        virtual String ToString() const override;
    };
}
