#include "Document.h"

namespace Gularen 
{
    DocumentNode::DocumentNode(): Node(NodeType::Document) {}

    DocumentNode::DocumentNode(const NodeChildren& children): Node(NodeType::Document, children) {}

    String DocumentNode::ToString() const
    {
        return "Document";
    }

    ParagraphNode::ParagraphNode(): Node(NodeType::Paragraph) {}

    ParagraphNode::ParagraphNode(const NodeChildren& children): Node(NodeType::Paragraph, children) {}

    String ParagraphNode::ToString() const
    {
        return "Paragraph";
    }

    TextNode::TextNode(const String& content): Node(NodeType::Text), content(content) {}
        
    String TextNode::ToString() const
    {
        return "Text (" + std::to_string(content.size()) + ") \"" + content + "\"";
    }
}