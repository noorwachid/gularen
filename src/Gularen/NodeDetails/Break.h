#pragma once

#include "Structure.h"

namespace Gularen {
    struct BreakNode: Node {
        BreakNode(NodeType type, NodeGroup group);

        virtual String toString() const override;
    };

    struct LineBreakNode: BreakNode {
        LineBreakNode();

        virtual String toString() const override;
    };

    struct PageBreakNode: BreakNode {
        PageBreakNode();

        virtual String toString() const override;
    };
}