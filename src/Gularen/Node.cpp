#include "Node.h"

namespace Gularen 
{
    Node::Node(NodeType type): type(type) {}

    Node::Node(const NodeChildren& children): children(children) {}

    String Node::ToString() 
    { 
        return ""; 
    }

    bool operator==(const RC<Node>& a, const RC<Node>& b)
    {
        if (a->type != b->type) 
            return false;

        // TODO: Implement GetHash instead
        if (a->ToString() != b->ToString())
            return false;

        if (a->children.size() != b->children.size())
            return false;

        for (std::size_t i = 0; i < a->children.size(); ++i)
            if (!(a->children[i] ==  b->children[i]))
                return false;

        return true;
    }

    DocumentNode::DocumentNode(): Node(NodeType::Document) {}

    String DocumentNode::ToString()
    {
        return "Document";
    }

    ParagraphNode::ParagraphNode(): Node(NodeType::Paragraph) {}

    String ParagraphNode::ToString()
    {
        return "Paragraph";
    }

    TextNode::TextNode(const String& content): Node(NodeType::Text), content(content) {}
        
    String TextNode::ToString()
    {
        return "Text (" + std::to_string(content.size()) + ") \"" + content + "\"";
    }

    FSNode::FSNode(NodeType type): Node(type) {}

    String FSNode::ToString()
    {
        switch (type)
        {
        case NodeType::BoldFS:
            return "BoldFS";

        case NodeType::ItalicFS:
            return "ItalicFS";

        case NodeType::MonospacedFS:
            return "MonospacedFS";
            
        default:
            return "UnknownFS";
        }
    }

    QuoteNode::QuoteNode(NodeType type): Node(type) {}

    String QuoteNode::ToString()
    {
        switch (type)
        {
        case NodeType::LSQuote:
            return "LSQuote";

        case NodeType::RSQuote:
            return "RSQuote";

        case NodeType::LDQuote:
            return "LDQuote";

        case NodeType::RDQuote:
            return "RDQuote";
            
        default:
            return "UnknownQuote";
        }
    }    
}
