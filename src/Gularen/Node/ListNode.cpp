#include "ListNode.h"

namespace Gularen {
	ListNode::ListNode(NodeType type) : Node(type, NodeGroup::Block) {
	}

	ListNode::ListNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {
	}

	String ListNode::toString() const {
		return "list";
	}

	ItemNode::ItemNode(NodeType type) : Node(type, NodeGroup::Block) {
	}

	ItemNode::ItemNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {
	}

	String ItemNode::toString() const {
		return "item";
	}

	NumericListNode::NumericListNode() : ListNode(NodeType::numericList) {
	}

	NumericListNode::NumericListNode(const NodeChildren& children) : ListNode(NodeType::numericItem, children) {
	}

	String NumericListNode::toString() const {
		return "numericList";
	}

	NumericItemNode::NumericItemNode() : ItemNode(NodeType::numericItem) {
	}

	NumericItemNode::NumericItemNode(const NodeChildren& children) : ItemNode(NodeType::numericItem, children) {
	}

	String NumericItemNode::toString() const {
		return "numericItem";
	}
}
