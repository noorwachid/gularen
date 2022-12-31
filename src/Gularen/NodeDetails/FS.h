#pragma once

#include "Core.h"

namespace Gularen 
{
    struct FSNode: Node
    {
        FSNode(NodeType type);

        FSNode(NodeType type, const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct BoldFSNode: FSNode
    {
        BoldFSNode();

        BoldFSNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct ItalicFSNode: FSNode
    {
        ItalicFSNode();

        ItalicFSNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct MonospaceFSNode: FSNode
    {
        MonospaceFSNode();

        MonospaceFSNode(const NodeChildren& children);

        virtual String ToString() const override;
    };
}