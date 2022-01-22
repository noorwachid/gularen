#include "Node.hpp"

namespace Gularen
{
	Node::Node()
		:
		type(NodeType::unknown),
		group(NodeGroup::unknown),
		shape(NodeShape::unknown)
	{
	}

	Node::Node(NodeType type, NodeGroup group, NodeShape shape)
		:
		type(type),
		group(group),
		shape(shape)
	{
	}

	Node::~Node()
	{
	}

	void Node::add(Node* node)
	{
		children.push_back(node);
	}

	std::string Node::toString()
	{
		return Gularen::toString(type) + ":";
	}

	ValueNode::ValueNode()
		:
		Node()
	{
	}

	ValueNode::ValueNode(NodeType type, NodeGroup group, NodeShape shape, const std::string& value)
		:
		Node(type, group, shape),
		value(value)
	{
	}

	std::string ValueNode::toString()
	{
		return Gularen::toString(type) + ": \"" + value + "\"";;
	}

	SizeNode::SizeNode()
		:
		Node()
	{
	}

	SizeNode::SizeNode(NodeType type, NodeGroup group, NodeShape shape, const size_t size)
		:
		Node(type, group, shape),
		size(size)
	{
	}

	std::string SizeNode::toString()
	{
		return Gularen::toString(type) + ": " + std::to_string(size);
	}

	BooleanNode::BooleanNode()
		:
		Node(NodeType::unknown),
		state(false)
	{
	}

	BooleanNode::BooleanNode(NodeType type, NodeGroup group, NodeShape shape, bool state)
		:
		Node(type, group, shape),
		state(state)
	{
	}

	std::string BooleanNode::toString()
	{
		return Gularen::toString(type) + ": " + (state ? "on" : "off");
	}

	TernaryNode::TernaryNode()
		:
		Node(),
		state(TernaryState::off)
	{
	}

	TernaryNode::TernaryNode(NodeType type, NodeGroup group, NodeShape shape, TernaryState state)
		:
		Node(type, group, shape),
		state(state)
	{
	}

	std::string TernaryNode::toString()
	{
		std::string buffer = Gularen::toString(type) + ": ";

		switch (state) {
			case TernaryState::off:
				buffer += "off";
				break;
			case TernaryState::on:
				buffer += "on";
				break;
			default:
				buffer += "inBetween";
				break;
		}

		return buffer;
	}

	ContainerNode::ContainerNode()
		:
		Node(),
		value(nullptr)
	{
	}

	ContainerNode::ContainerNode(NodeType type, NodeGroup group, NodeShape shape, Node* node)
		:
		Node(type, group, shape),
		value(node)
	{
	}

	std::string ContainerNode::toString()
	{
		std::string buffer = Gularen::toString(type) + ": ";
		if (value) {
			if (value->type == NodeType::symbol)
				buffer += "%" + static_cast<ValueNode*>(value)->value;

			if (value->type == NodeType::quotedText)
				buffer += "\"" + static_cast<ValueNode*>(value)->value + "\"";
		}
		return buffer;
	}

	TableNode::TableNode()
		:
		Node(NodeType::table, NodeGroup::table, NodeShape::block)
	{
	}

	CodeNode::CodeNode()
		:
		Node(NodeType::code, NodeGroup::code, NodeShape::block)
	{
	}

	CodeNode::CodeNode(const std::string& value, Node* lang)
		:
		Node(NodeType::code, NodeGroup::code, NodeShape::block),
		value(value),
		langCode(lang)
	{
	}

	std::string CodeNode::toString()
	{
		std::string buffer = Gularen::toString(type) + ": ";
		if (langCode) {
			if (langCode->type == NodeType::symbol)
				buffer += "%" + static_cast<ValueNode*>(langCode)->value + " ";

			if (langCode->type == NodeType::quotedText)
				buffer += "\"" + static_cast<ValueNode*>(langCode)->value + "\" ";
		}
		return buffer + "\"" + value + "\"";
	}
}
