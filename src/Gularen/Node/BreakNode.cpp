#include "BreakNode.h"

namespace Gularen {
	BreakNode::BreakNode(NodeType type, NodeGroup group) : Node(type, group) {
	}

	String BreakNode::toString() const {
		return "break";
	}

	LineBreakNode::LineBreakNode() : BreakNode(NodeType::lineBreak, NodeGroup::Inline) {
	}

	String LineBreakNode::toString() const {
		return "lineBreak";
	}

	PageBreakNode::PageBreakNode() : BreakNode(NodeType::pageBreak, NodeGroup::Block) {
	}

	String PageBreakNode::toString() const {
		return "pageBreak";
	}
}
