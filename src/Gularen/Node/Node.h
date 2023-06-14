#pragma once

#include "../DS/Array.h"
#include "../DS/RC.h"
#include "../DS/String.h"

namespace Gularen
{
    enum struct NodeType
    {
        Root,

        Title,
        Subtitle,

        Document,
        Part,
        Chapter,
        Section,
        Subsection,
        Subsubsection,
        Segment,

        Indentation,

        Paragraph,

        Text,
        Newline,

        LineBreak,
        PageBreak,

        BoldFS,
        ItalicFS,
        MonospaceFS,

        LSQuote,
        RSQuote,
        LDQuote,
        RDQuote,

        List,
        Item,

        NumericList,
        NumericItem,
    };

    enum struct NodeGroup
    {
        Inline,
        Block,
        VirtualBlock,
    };

    struct Node;

    using NodeChildren = Array<RC<Node>>;

    struct Node
    {
        NodeType type;
        NodeGroup group;

        NodeChildren children;

        Node(NodeType type, NodeGroup group);

        Node(NodeType type, NodeGroup group, const NodeChildren& children);

        virtual String ToString() const;
    };

    bool operator==(const Node& a, const Node& b);
}
