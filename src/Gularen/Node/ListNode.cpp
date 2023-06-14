#include "ListNode.h"

namespace Gularen
{
    ListNode::ListNode(NodeType type) : Node(type, NodeGroup::Block) {}

    ListNode::ListNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {}

    String ListNode::ToString() const { return "ListNode"; }

    ItemNode::ItemNode(NodeType type) : Node(type, NodeGroup::Block) {}

    ItemNode::ItemNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {}

    String ItemNode::ToString() const { return "ItemNode"; }

    NumericListNode::NumericListNode() : ListNode(NodeType::NumericList) {}

    NumericListNode::NumericListNode(const NodeChildren& children) : ListNode(NodeType::NumericItem, children) {}

    String NumericListNode::ToString() const { return "NumericListNode"; }

    NumericItemNode::NumericItemNode() : ItemNode(NodeType::NumericItem) {}

    NumericItemNode::NumericItemNode(const NodeChildren& children) : ItemNode(NodeType::NumericItem, children) {}

    String NumericItemNode::ToString() const { return "NumericItemNode"; }
}
