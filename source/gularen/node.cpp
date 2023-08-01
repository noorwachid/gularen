#include <gularen/node.h>
#include <gularen/helper/escape.h>

namespace Gularen {
	// constructors

	DocumentNode::DocumentNode(const std::string& path) : path{path} {
		group = NodeGroup::document;
	}

	TextNode::TextNode(const std::string& value) : value{value} {
		group = NodeGroup::text;
	}

	PunctNode::PunctNode(PunctType type, const std::string& value) : type{type}, value{value} {
		group = NodeGroup::punct;
	}

	EmojiNode::EmojiNode(const std::string& value) : value{value} {
		group = NodeGroup::emoji;
	}

	FSNode::FSNode(FSType type) : type{type} {
		group = NodeGroup::fs;
	}

	HeadingNode::HeadingNode(HeadingType type) : type{type} {
		group = NodeGroup::heading;
	}

	ParagraphNode::ParagraphNode() {
		group = NodeGroup::paragraph;
	}

	AdmonNode::AdmonNode(AdmonType type) : type{type} {
		group = NodeGroup::admon;
	}

	FootnoteJumpNode::FootnoteJumpNode(const std::string& value) : value{value} {
		group = NodeGroup::footnoteJump;
	}

	FootnoteDescribeNode::FootnoteDescribeNode(const std::string& value) : value{value} {
		group = NodeGroup::footnoteDescribe;
	}

	BreakNode::BreakNode(BreakType type) : type{type} {
		group = NodeGroup::break_;
	}

	ListNode::ListNode(ListType type) : type{type} {
		group = NodeGroup::list;
	}

	ListItemNode::ListItemNode(size_t index) : index{index}, state{ListItemState::none} {
		group = NodeGroup::listItem;
	}

	ListItemNode::ListItemNode(size_t index, ListItemState state) : index{index}, state{state} {
		group = NodeGroup::listItem;
	}

	TableNode::TableNode() {
		group = NodeGroup::table;
	}

	TableRowNode::TableRowNode() {
		group = NodeGroup::tableRow;
	}

	ResourceNode::ResourceNode(ResourceType type, const std::string& value) : type{type}, value{value} {
		group = NodeGroup::resource;
	}

	// toStrings

	std::string Node::toString() const {
		return "base";
	}

	std::string DocumentNode::toString() const {
		if (path.empty())
			return "document";

		return "document " + Helper::escape(path);
	}

	std::string TextNode::toString() const {
		return "text " + Helper::escape(value);
	}

	std::string PunctNode::toString() const {
		std::string typeString;

		return "punct " + value;
	}

	std::string EmojiNode::toString() const {
		return "emoji " + value;
	}

	std::string FSNode::toString() const {
		std::string string = "fs ";

		switch (type) {
			case FSType::bold:
				string += "bold";
				break;
			case FSType::italic:
				string += "italic";
				break;
			case FSType::monospace:
				string += "monospace";
				break;

				return string;
		}

		return string;
	}

	std::string HeadingNode::toString() const {
		std::string string;

		switch (type) {
			case HeadingType::chapter:
				string = "chapter";
				break;
			case HeadingType::section:
				string = "section";
				break;
			case HeadingType::subsection:
				string = "subsection";
				break;
			case HeadingType::subtitle:
				string = "subtitle";
				break;
		}

		if (!id.empty()) {
			string += " id:" + id;
		}

		return string;
	}

	std::string ParagraphNode::toString() const {
		return "paragraph";
	}

	std::string AdmonNode::toString() const {
		std::string string = "admon ";

		switch (type) {
			case AdmonType::note:
				string += "note";
				break;
			case AdmonType::hint:
				string += "hint";
				break;
			case AdmonType::important:
				string += "important";
				break;
			case AdmonType::warning:
				string += "warning";
				break;
			case AdmonType::seeAlso:
				string += "seeAlso";
				break;
			case AdmonType::tip:
				string += "tip";
				break;
		}
		return string;
	}

	std::string FootnoteJumpNode::toString() const {
		return "footnoteJump " + value;
	}

	std::string FootnoteDescribeNode::toString() const {
		return "footnoteDescribe " + value;
	}

	std::string IndentNode::toString() const {
		return "indent";
	}

	std::string BQNode::toString() const {
		return "bq";
	}

	std::string BreakNode::toString() const {
		switch (type) {
			case BreakType::line:
				return "lineBreak";
			case BreakType::page:
				return "pageBreak";
		}
	}

	std::string ListNode::toString() const {
		switch (type) {
			case ListType::bullet:
				return "bulletList";
			case ListType::index:
				return "indexList";
			case ListType::check:
				return "checkList";
		}
	}

	std::string ListItemNode::toString() const {
		std::string string = "listItem ";

		switch (state) {
			case ListItemState::none:
				break;
			case ListItemState::todo:
				string += "todo";
				break;
			case ListItemState::done:
				string += "done";
				break;
			case ListItemState::canceled:
				string += "canceled";
				break;
		}

		return string;
	}

	std::string TableNode::toString() const {
		std::string string = "table";
		if (header > 0) {
			string += " header:" + std::to_string(header);
		}
		if (footer > 0) {
			string += " footer:" + std::to_string(footer);
		}

		if (!alignments.empty()) {
			string += " alignments:";
			for (Alignment alignment : alignments) {
				switch (alignment) {
					case Alignment::left:
						string += "left,";
						break;
					case Alignment::center:
						string += "center,";
						break;
					case Alignment::right:
						string += "right,";
						break;
				}
			}
			string.pop_back();
		}

		return string;
	}

	std::string TableRowNode::toString() const {
		return "tableRow";
	}

	std::string TableCellNode::toString() const {
		return "tableCell";
	}

	std::string ResourceNode::toString() const {
		std::string string;

		switch (type) {
			case ResourceType::linker:
				string = "linker";
				break;
			case ResourceType::linkerLocal:
				string = "linkerLocal";
				break;
			case ResourceType::presenter:
				string = "presenter";
				break;
			case ResourceType::presenterLocal:
				string = "presenterLocal";
				break;
		}

		if (!value.empty()) {
			string += " value:" + Helper::escape(value);
		}

		if (!id.empty()) {
			string += " id:" + id;
		}

		if (!label.empty()) {
			string += " label:" + Helper::escape(label);
		}

		return string;
	}

	std::string CodeNode::toString() const {
		std::string string = type == CodeType::block ? "codeblock" : "code";

		if (!lang.empty()) {
			string += " lang:" + lang;
		}

		if (!source.empty()) {
			string += " source:" + Helper::escape(source);
		}

		return string;
	}
}
