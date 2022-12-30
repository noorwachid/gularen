#pragma once

#include "../DS/String.h"
#include "../DS/Array.h"
#include "../DS/RC.h"

namespace Gularen
{
    enum struct NodeType
    {
        Document,

        Paragraph,

        Text,

        BoldFS,
        ItalicFS,
        MonospaceFS,
        
        LSQuote,
        RSQuote,
        LDQuote,
        RDQuote,
    };

	struct Node;

	using NodeChildren = Array<RC<Node>>;

    struct Node
    {
        NodeType type;
        
        NodeChildren children;

        Node(NodeType type);

        Node(NodeType type, const NodeChildren& children);

        virtual String ToString() const;
    };

    bool operator==(const Node& a, const Node& b);
}