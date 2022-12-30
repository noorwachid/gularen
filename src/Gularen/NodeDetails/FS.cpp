#include "FS.h"

namespace Gularen 
{
    FSNode::FSNode(NodeType type): Node(type) {}

    FSNode::FSNode(NodeType type, const NodeChildren& children): Node(type, children) {}

    String FSNode::ToString() const
    {
        return "UnknownFS";
    }

    BoldFSNode::BoldFSNode(): FSNode(NodeType::BoldFS) {}

    BoldFSNode::BoldFSNode(const NodeChildren& children): FSNode(NodeType::BoldFS, children) {}

    String BoldFSNode::ToString() const
    {
        return "BoldFS";
    }

    ItalicFSNode::ItalicFSNode(): FSNode(NodeType::ItalicFS) {}

    ItalicFSNode::ItalicFSNode(const NodeChildren& children): FSNode(NodeType::ItalicFS, children) {}

    String ItalicFSNode::ToString() const
    {
        return "ItalicFS";
    }

    MonospaceFSNode::MonospaceFSNode(): FSNode(NodeType::MonospaceFS) {}

    MonospaceFSNode::MonospaceFSNode(const NodeChildren& children): FSNode(NodeType::MonospaceFS, children) {}

    String MonospaceFSNode::ToString() const
    {
        return "MonospaceFS";
    }
}
