#include "FS.h"

namespace Gularen {
    FSNode::FSNode(NodeType type): Node(type, NodeGroup::inline_) {}

    FSNode::FSNode(NodeType type, const NodeChildren& children): Node(type, NodeGroup::inline_, children) {}

    String FSNode::toString() const {
        return "[unimplementedFS]";
    }

    BoldFSNode::BoldFSNode(): FSNode(NodeType::boldFS) {}

    BoldFSNode::BoldFSNode(const NodeChildren& children): FSNode(NodeType::boldFS, children) {}

    String BoldFSNode::toString() const {
        return "boldFS";
    }

    ItalicFSNode::ItalicFSNode(): FSNode(NodeType::italicFS) {}

    ItalicFSNode::ItalicFSNode(const NodeChildren& children): FSNode(NodeType::italicFS, children) {}

    String ItalicFSNode::toString() const {
        return "italicFS";
    }

    MonospaceFSNode::MonospaceFSNode(): FSNode(NodeType::monospaceFS) {}

    MonospaceFSNode::MonospaceFSNode(const NodeChildren& children): FSNode(NodeType::monospaceFS, children) {}

    String MonospaceFSNode::toString() const {
        return "monospaceFS";
    }
}
