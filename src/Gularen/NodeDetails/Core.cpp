#include "Core.h"

namespace Gularen {
    RootNode::RootNode(): Node(NodeType::root) {}

    RootNode::RootNode(const NodeChildren& children): Node(NodeType::root, children) {}

    String RootNode::toString() const {
        return "root";
    }

    ParagraphNode::ParagraphNode(): Node(NodeType::paragraph) {}

    ParagraphNode::ParagraphNode(const NodeChildren& children): Node(NodeType::paragraph, children) {}

    String ParagraphNode::toString() const {
        return "paragraph";
    }

    TextNode::TextNode(const String& content): Node(NodeType::text), content(content) {}
        
    String TextNode::toString() const {
        return "text content:\"" + content + "\"";
    }
}