#pragma once

#include <string>
#include <memory>
#include <stack>

namespace Gularen {
	enum class NodeGroup {
		text,
		fs,
		heading,
		paragraph,
		indent,
		break_,
	};

	struct Node;

	using NodePtr = std::shared_ptr<Node>;
	using NodeChildren = std::vector<NodePtr>;

	struct Node {
		NodeGroup group;
		NodeChildren children;

		Node() = default;
		
		Node(NodeGroup group) : group{group} {}

		virtual std::string toString() {
			return "base: " + std::to_string(static_cast<int>(group));
		}
	};

	struct TextNode : Node {
		std::string value;

		TextNode(const std::string& value) : value{value} {
			group = NodeGroup::text;
		}
		
		virtual std::string toString() override {
			return "text: " + value;
		}
	};
	
	enum class FSType {
		bold,
		italic,
		monospace
	};

	struct FSNode : Node {
		FSType type;
		
		FSNode(FSType type) : type{type} {
			group = NodeGroup::fs;
		}

		virtual std::string toString() override {
			return "fs: " + std::to_string(static_cast<int>(type));
		}
	};

	enum class HeadingType {
		chapter,
		section,
		subsection,
		subtitle,
	};

	struct HeadingNode : Node {
		HeadingType type;
		std::string id;
		
		HeadingNode(HeadingType type) : type{type} {
			group = NodeGroup::heading;
		}

		virtual std::string toString() override {
			std::string string = "heading: " + std::to_string(static_cast<int>(type));

			if (!id.empty()) {
				string += " id: " + id;
			}
			
			return string;
		}
	};
	
	struct ParagraphNode : Node {
		ParagraphNode() {
			group = NodeGroup::paragraph;
		}
	
		virtual std::string toString() override {
			return "paragraph";
		}
	};

	struct IndentNode : Node {
		IndentNode() {
			group = NodeGroup::indent;
		}
	
		virtual std::string toString() override {
			return "indent";
		}
	};

	enum class BreakType {
		line,
		page,
	};

	struct BreakNode : Node {
		BreakType type;

		BreakNode(BreakType type) : type{type} {
			group = NodeGroup::break_;;
		}
	
		virtual std::string toString() override {
			return "break: " + std::to_string(static_cast<int>(type));
		}
	};
}
