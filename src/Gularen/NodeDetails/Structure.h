#pragma once

#include "../DS/String.h"
#include "../DS/Array.h"
#include "../DS/RC.h"

namespace Gularen {
    enum struct NodeType {
        root,

        title,
        subtitle,

        document,
        part,
        chapter,
        section,
        subsection,
        subsubsection,
        segment,

        paragraph,

        text,

        boldFS,
        italicFS,
        monospaceFS,
        
        lsQuote,
        rsQuote,
        ldQuote,
        rdQuote,
    };

	struct Node;

	using NodeChildren = Array<RC<Node>>;

    struct Node {
        NodeType type;
        
        NodeChildren children;

        Node(NodeType type);

        Node(NodeType type, const NodeChildren& children);

        virtual String toString() const;
    };

    bool operator==(const Node& a, const Node& b);
}