#include "CoreNode.h"

namespace Gularen
{
    RootNode::RootNode() : Node(NodeType::Root, NodeGroup::Block) {}

    RootNode::RootNode(const NodeChildren& children) : Node(NodeType::Root, NodeGroup::Block, children) {}

    String RootNode::ToString() const { return "root"; }

    IndentationNode::IndentationNode() : Node(NodeType::Indentation, NodeGroup::Block) {}

    IndentationNode::IndentationNode(const NodeChildren& children)
        : Node(NodeType::Indentation, NodeGroup::Block, children)
    {
    }

    String IndentationNode::ToString() const { return "indentation"; }

    ParagraphNode::ParagraphNode() : Node(NodeType::Paragraph, NodeGroup::Block) {}

    ParagraphNode::ParagraphNode(const NodeChildren& children) : Node(NodeType::Paragraph, NodeGroup::Block, children)
    {
    }

    String ParagraphNode::ToString() const { return "paragraph"; }

    TextNode::TextNode(const String& content) : Node(NodeType::Text, NodeGroup::Inline), content(content) {}

    String TextNode::ToString() const { return "text content = \"" + content + "\""; }

    NewlineNode::NewlineNode(UintSize repeatition)
        : Node(NodeType::Newline, NodeGroup::Inline), repetition(repeatition)
    {
    }

    String NewlineNode::ToString() const { return "newline repetition = " + std::to_string(repetition); }
}
