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
		list,
		listItem,

		table,
		tableRow,
		tableCell,

		resource,

		code,

		punct,
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
			return "base " + std::to_string(static_cast<int>(group));
		}
	};

	struct TextNode : Node {
		std::string value;

		TextNode(const std::string& value) : value{value} {
			group = NodeGroup::text;
		}
		
		virtual std::string toString() override {
			return "text " + value;
		}
	};
	
	enum class PunctType {
		minus,
		hyphen,
		enDash,
		emDash,
		
		lsQuo,
		rsQuo,
		ldQuo,
		rdQuo,
	};

	struct PunctNode : Node {
		PunctType type;
		std::string value;

		PunctNode(PunctType type, const std::string& value) : type{type}, value{value} {
			group = NodeGroup::punct;
		}

		virtual std::string toString() {
			return "punct " + std::to_string(static_cast<int>(type)) + " " + value;
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
			return "fs " + std::to_string(static_cast<int>(type));
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
			std::string string = "heading " + std::to_string(static_cast<int>(type));

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
			return "break " + std::to_string(static_cast<int>(type));
		}
	};

	enum class ListType {
		bullet,
		index,
		checkbox,
	};

	struct ListNode : Node {
		ListType type;

		ListNode(ListType type) : type{type} {
			group = NodeGroup::list;
		}
	
		virtual std::string toString() override {
			return "list " + std::to_string(static_cast<int>(type));
		}
	};

	struct ListItemNode : Node {
		size_t state = 0;

		ListItemNode() {
			group = NodeGroup::listItem;
		}

		ListItemNode(size_t state) : state{state} {
			group = NodeGroup::listItem;
		}
	
		virtual std::string toString() override {
			return "listItem state: " + std::to_string(static_cast<int>(state));
		}
	};

	enum class Alignment {
		left,
		center,
		right,
	};

	struct TableNode : Node {
		size_t header = 0;
		size_t footer = 0;
		std::vector<Alignment> alignments;
		
		TableNode() {
			group = NodeGroup::table;
		}

		virtual std::string toString() override {
			std::string string =  "table";
			if (header > 0) {
				string += " header: " + std::to_string(header);
			}
			if (footer > 0) {
				string += " footer: " + std::to_string(footer);
			}

			if (!alignments.empty()) {
				string += " alignments: ";
				for (Alignment alignment : alignments) {
					string += std::to_string(static_cast<int>(alignment)) + " ";
				}
			}

			return string;
		}
	};

	struct TableRowNode : Node {
		TableRowNode() {
			group = NodeGroup::tableRow;
		}

		virtual std::string toString() override {
			return "tableRow";
		}
	};

	struct TableCellNode : Node {
		TableCellNode() {
			group = NodeGroup::tableCell;
		}

		virtual std::string toString() override {
			return "tableCell";
		}
	};

	enum class ResourceType {
		linker,
		linkerLocal,
		presenter,
		presenterLocal,
	};

	struct ResourceNode : Node {
		ResourceType type;
		std::string value;
		std::string id;
		std::string label;
		
		ResourceNode(ResourceType type, const std::string& value) : type{type}, value{value} {
			group = NodeGroup::resource;
		}

		virtual std::string toString() override {
			std::string string = "resource " + std::to_string(static_cast<int>(type));

			if (!value.empty()) {
				string += " value: " + value;
			}

			if (!id.empty()) {
				string += " id: " + id;
			}

			if (!label.empty()) {
				string += " label: " + label;
			}
			
			return string;
		}
	};

	struct CodeNode : Node {
		std::string lang;
		std::string source;
		
		CodeNode() {
			group = NodeGroup::code;
		}

		virtual std::string toString() override {
			std::string string = "code";

			if (!lang.empty()) {
				string += " lang: " + lang;
			}

			if (!source.empty()) {
				string += " source: " + source;
			}

			return string;
		}
	};
}
