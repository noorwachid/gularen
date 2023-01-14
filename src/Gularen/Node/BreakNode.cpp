#include "BreakNode.h"

namespace Gularen
{
    BreakNode::BreakNode(NodeType type, NodeGroup group) : Node(type, group) {}

    String BreakNode::ToString() const { return "break"; }

    LineBreakNode::LineBreakNode() : BreakNode(NodeType::LineBreak, NodeGroup::Inline) {}

    String LineBreakNode::ToString() const { return "lineBreak"; }

    PageBreakNode::PageBreakNode() : BreakNode(NodeType::PageBreak, NodeGroup::Block) {}

    String PageBreakNode::ToString() const { return "pageBreak"; }
}
