#include "Core.h"

namespace Gularen {
    RootNode::RootNode(): Node(NodeType::root, NodeGroup::block) {}

    RootNode::RootNode(const NodeChildren& children): Node(NodeType::root, NodeGroup::block, children) {}

    String RootNode::toString() const {
        return "root";
    }

    IndentationNode::IndentationNode(): Node(NodeType::indentation, NodeGroup::block) {}

    IndentationNode::IndentationNode(const NodeChildren& children): Node(NodeType::indentation, NodeGroup::block, children) {}

    String IndentationNode::toString() const {
        return "indentation";
    }

    ParagraphNode::ParagraphNode(): Node(NodeType::paragraph, NodeGroup::block) {}

    ParagraphNode::ParagraphNode(const NodeChildren& children): Node(NodeType::paragraph, NodeGroup::block, children) {}

    String ParagraphNode::toString() const {
        return "paragraph";
    }

    TextNode::TextNode(const String& content): Node(NodeType::text, NodeGroup::inline_), content(content) {}
        
    String TextNode::toString() const {
        return "text content = \"" + content + "\"";
    }

    NewlineNode::NewlineNode(UintSize repeatition): Node(NodeType::newline, NodeGroup::inline_), repetition(repeatition) {}
        
    String NewlineNode::toString() const {
        return "newline repetition = " + std::to_string(repetition);
    }
}
