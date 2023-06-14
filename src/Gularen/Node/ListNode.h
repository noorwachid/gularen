#pragma once

#include "CoreNode.h"

namespace Gularen
{
    struct ListNode : Node
    {
        ListNode(NodeType type);

        ListNode(NodeType type, const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct ItemNode : Node
    {
        ItemNode(NodeType type);

        ItemNode(NodeType type, const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct NumericListNode : ListNode
    {
        NumericListNode();

        NumericListNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct NumericItemNode : ItemNode
    {
        NumericItemNode();

        NumericItemNode(const NodeChildren& children);

        virtual String ToString() const override;
    };
}
