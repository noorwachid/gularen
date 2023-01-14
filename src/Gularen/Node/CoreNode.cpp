#include "CoreNode.h"

namespace Gularen
{
    RootNode::RootNode() : Node(NodeType::Root, NodeGroup::Block) {}

    RootNode::RootNode(const NodeChildren& children) : Node(NodeType::Root, NodeGroup::Block, children) {}

    String RootNode::ToString() const { return "Root"; }

    IndentationNode::IndentationNode() : Node(NodeType::Indentation, NodeGroup::Block) {}

    IndentationNode::IndentationNode(const NodeChildren& children)
        : Node(NodeType::Indentation, NodeGroup::Block, children)
    {
    }

    String IndentationNode::ToString() const { return "Indentation"; }

    ParagraphNode::ParagraphNode() : Node(NodeType::Paragraph, NodeGroup::Block) {}

    ParagraphNode::ParagraphNode(const NodeChildren& children) : Node(NodeType::Paragraph, NodeGroup::Block, children)
    {
    }

    String ParagraphNode::ToString() const { return "Paragraph"; }

    TextNode::TextNode(const String& content) : Node(NodeType::Text, NodeGroup::Inline), content(content) {}

    String TextNode::ToString() const { return "Text content = \"" + content + "\""; }

    NewlineNode::NewlineNode(UintSize repeatition)
        : Node(NodeType::Newline, NodeGroup::Inline), repetition(repeatition)
    {
    }

    String NewlineNode::ToString() const { return "Newline repetition = " + std::to_string(repetition); }
}
