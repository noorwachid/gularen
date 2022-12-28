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

        Node(const NodeChildren& children);

        virtual String ToString();
    };

    bool operator==(const RC<Node>& a, const RC<Node>& b);

    struct DocumentNode: Node
    {
        DocumentNode();

		DocumentNode(const NodeChildren& children): Node(children) {}

        virtual String ToString() override;
    };

    struct ParagraphNode: Node
    {
        ParagraphNode();

        virtual String ToString() override;
    };

    struct TextNode: Node
    {
        String content;

        TextNode(const String& content);
        
        virtual String ToString() override;
    };

    struct FSNode: Node
    {
        FSNode(NodeType type);

        virtual String ToString() override;
    };

    struct QuoteNode: Node
    {
        QuoteNode(NodeType type);

        virtual String ToString() override;
    };
}
