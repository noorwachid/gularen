#include "FSNode.h"

namespace Gularen
{
    FSNode::FSNode(NodeType type) : Node(type, NodeGroup::Inline) {}

    FSNode::FSNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Inline, children) {}

    String FSNode::ToString() const { return "[UnimplementedFS]"; }

    BoldFSNode::BoldFSNode() : FSNode(NodeType::BoldFS) {}

    BoldFSNode::BoldFSNode(const NodeChildren& children) : FSNode(NodeType::BoldFS, children) {}

    String BoldFSNode::ToString() const { return "BoldFS"; }

    ItalicFSNode::ItalicFSNode() : FSNode(NodeType::ItalicFS) {}

    ItalicFSNode::ItalicFSNode(const NodeChildren& children) : FSNode(NodeType::ItalicFS, children) {}

    String ItalicFSNode::ToString() const { return "ItalicFS"; }

    MonospaceFSNode::MonospaceFSNode() : FSNode(NodeType::MonospaceFS) {}

    MonospaceFSNode::MonospaceFSNode(const NodeChildren& children) : FSNode(NodeType::MonospaceFS, children) {}

    String MonospaceFSNode::ToString() const { return "MonospaceFS"; }
}
