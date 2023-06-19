#include "CoreNode.h"

namespace Gularen {
	RootNode::RootNode() : Node(NodeType::root, NodeGroup::Block) {
	}

	RootNode::RootNode(const NodeChildren& children) : Node(NodeType::root, NodeGroup::Block, children) {
	}

	String RootNode::toString() const {
		return "root";
	}

	IndentationNode::IndentationNode() : Node(NodeType::indentation, NodeGroup::Block) {
	}

	IndentationNode::IndentationNode(const NodeChildren& children)
		: Node(NodeType::indentation, NodeGroup::Block, children) {
	}

	String IndentationNode::toString() const {
		return "indentation";
	}

	ParagraphNode::ParagraphNode() : Node(NodeType::paragraph, NodeGroup::Block) {
	}

	ParagraphNode::ParagraphNode(const NodeChildren& children) : Node(NodeType::paragraph, NodeGroup::Block, children) {
	}

	String ParagraphNode::toString() const {
		return "paragraph";
	}

	TextNode::TextNode(const String& content) : Node(NodeType::text, NodeGroup::Inline), content(content) {
	}

	String TextNode::toString() const {
		return "text content = \"" + content + "\"";
	}

	NewlineNode::NewlineNode(UintSize repeatition)
		: Node(NodeType::newline, NodeGroup::Inline), repetition(repeatition) {
	}

	String NewlineNode::toString() const {
		return "newline repetition = " + std::to_string(repetition);
	}
}
