#pragma once

#include <string>
#include <memory>
#include <stack>

namespace Gularen {
	enum class NodeType {
		text,

		fsBold,
		fsItalic,
		fsMonospace
	};

	struct Node;

	using NodePtr = std::shared_ptr<Node>;
	using NodeChildren = std::vector<NodePtr>;

	struct Node {
		NodeType type;
		NodeChildren children;

		Node() = default;
		
		Node(NodeType type) : type{type} {}

		virtual std::string toString() {
			return "base: " + std::to_string(static_cast<int>(type));
		}
	};

	struct TextNode : Node {
		std::string value;

		TextNode(const std::string& value) : value{value} {
			type = NodeType::text;
		}
		
		virtual std::string toString() override {
			return "text: " + value;
		}
	};
}
