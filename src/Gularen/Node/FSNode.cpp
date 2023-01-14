#include "FSNode.h"

namespace Gularen
{
    FSNode::FSNode(NodeType type) : Node(type, NodeGroup::Inline) {}

    FSNode::FSNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Inline, children) {}

    String FSNode::ToString() const { return "[unimplementedFS]"; }

    BoldFSNode::BoldFSNode() : FSNode(NodeType::BoldFS) {}

    BoldFSNode::BoldFSNode(const NodeChildren& children) : FSNode(NodeType::BoldFS, children) {}

    String BoldFSNode::ToString() const { return "boldFS"; }

    ItalicFSNode::ItalicFSNode() : FSNode(NodeType::ItalicFS) {}

    ItalicFSNode::ItalicFSNode(const NodeChildren& children) : FSNode(NodeType::ItalicFS, children) {}

    String ItalicFSNode::ToString() const { return "italicFS"; }

    MonospaceFSNode::MonospaceFSNode() : FSNode(NodeType::MonospaceFS) {}

    MonospaceFSNode::MonospaceFSNode(const NodeChildren& children) : FSNode(NodeType::MonospaceFS, children) {}

    String MonospaceFSNode::ToString() const { return "monospaceFS"; }
}
