#pragma once

#include "DS/String.h"
#include "DS/Array.h"
#include "DS/RC.h"

namespace Gularen
{
    enum struct NodeType
    {
        Document,

        Paragraph,

        Text,

        BoldFS,
        ItalicFS,
        MonospacedFS,
        
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

    struct DocumentNode: Node
    {
        DocumentNode();

		DocumentNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct ParagraphNode: Node
    {
        ParagraphNode();

        virtual String ToString() const override;
    };

    struct TextNode: Node
    {
        String content;

        TextNode(const String& content);
        
        virtual String ToString() const override;
    };

    struct FSNode: Node
    {
        FSNode(NodeType type);

        virtual String ToString() const override;
    };

    struct QuoteNode: Node
    {
        QuoteNode(NodeType type);

        virtual String ToString() const override;
    };
}
