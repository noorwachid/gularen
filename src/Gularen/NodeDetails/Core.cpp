#include "Core.h"

namespace Gularen 
{
    RootNode::RootNode(): Node(NodeType::Root) {}

    RootNode::RootNode(const NodeChildren& children): Node(NodeType::Root, children) {}

    String RootNode::ToString() const
    {
        return "Root";
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
        return "Text content:\"" + content + "\"";
    }
}