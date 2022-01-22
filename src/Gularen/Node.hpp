#pragma once

#include "NodeType.hpp"
#include "NodeGroup.hpp"
#include <vector>

namespace Gularen
{
	struct Node
	{
		Node();

		Node(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown);

		virtual ~Node();

		void add(Node* node);

		virtual std::string toString();

		NodeType type;
		NodeGroup group;
		NodeShape shape;
		std::vector<Node*> children;
	};

	struct ValueNode : public Node
	{
		ValueNode();

		ValueNode(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown,
				const std::string& value = std::string());

		std::string toString() override;

		std::string value;
	};

	struct SizeNode : public Node
	{
		SizeNode();

		SizeNode(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown,
				const size_t size = 0);

		std::string toString() override;

		size_t size;
	};

	struct BooleanNode : public Node
	{
		BooleanNode();

		BooleanNode(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown,
				bool state = false);

		std::string toString() override;

		bool state;
	};

	enum struct TernaryState
	{
		off = 0,
		inBetween = 1,
		on = 2,
	};

	struct TernaryNode : public Node
	{
		TernaryNode();

		TernaryNode(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown,
				TernaryState state = TernaryState::off);

		std::string toString() override;

		TernaryState state;
	};

	struct ContainerNode : public Node
	{
		ContainerNode();

		ContainerNode(NodeType type, NodeGroup group = NodeGroup::unknown, NodeShape shape = NodeShape::unknown,
				Node* package = nullptr);

		std::string toString() override;

		Node* value;
	};

	struct CodeNode : public Node
	{
		CodeNode();

		CodeNode(const std::string& value, Node* lang = nullptr);

		std::string toString() override;

		std::string value;
		Node* langCode;
	};

	struct TableNode : public Node
	{
		TableNode();

		std::vector<ssize_t> horizontalHeaders;
		std::vector<ssize_t> verticalHeaders;
	};
}
