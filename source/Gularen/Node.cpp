#include "Gularen/Node.h"
#include "Gularen/Helper/Escape.h"

namespace Gularen
{
	// constructors

	DocumentNode::DocumentNode(const std::string& path) : path{path}
	{
		group = NodeGroup::Document;
	}

	CommentNode::CommentNode(const std::string& value) : value{value}
	{
		group = NodeGroup::Comment;
	}

	TextNode::TextNode(const std::string& value) : value{value}
	{
		group = NodeGroup::Text;
	}

	PunctNode::PunctNode(PunctType type, const std::string& value) : type{type}, value{value}
	{
		group = NodeGroup::Punct;
	}

	EmojiNode::EmojiNode(const std::string& value) : value{value}
	{
		group = NodeGroup::Emoji;
	}

	FSNode::FSNode(FSType type) : type{type}
	{
		group = NodeGroup::FS;
	}

	HeadingNode::HeadingNode(HeadingType type) : type{type}
	{
		group = NodeGroup::Heading;
	}

	ParagraphNode::ParagraphNode()
	{
		group = NodeGroup::Paragraph;
	}

	AdmonNode::AdmonNode(AdmonType type) : type{type}
	{
		group = NodeGroup::Admon;
	}

	FootnoteJumpNode::FootnoteJumpNode(const std::string& value) : value{value}
	{
		group = NodeGroup::FootnoteJump;
	}

	FootnoteDescribeNode::FootnoteDescribeNode(const std::string& value) : value{value}
	{
		group = NodeGroup::FootnoteDescribe;
	}

	BreakNode::BreakNode(BreakType type) : type{type}
	{
		group = NodeGroup::Break;
	}

	ListNode::ListNode(ListType type) : type{type}
	{
		group = NodeGroup::List;
	}

	ListItemNode::ListItemNode(size_t index) : index{index}, state{ListItemState::None}
	{
		group = NodeGroup::ListItem;
	}

	ListItemNode::ListItemNode(size_t index, ListItemState state) : index{index}, state{state}
	{
		group = NodeGroup::ListItem;
	}

	TableNode::TableNode()
	{
		group = NodeGroup::Table;
	}

	TableRowNode::TableRowNode()
	{
		group = NodeGroup::TableRow;
	}

	ResourceNode::ResourceNode(ResourceType type, const std::string& value) : type{type}, value{value}
	{
		group = NodeGroup::Resource;
	}

	// toStrings

	std::string Node::ToString() const
	{
		return range.ToString() + " base";
	}

	std::string DocumentNode::ToString() const
	{
		if (path.empty())
			return range.ToString() + " document";

		return range.ToString() + " document " + Helper::Escape(path);
	}

	std::string CommentNode::ToString() const
	{
		return range.ToString() + " comment " + Helper::Escape(value);
	}

	std::string TextNode::ToString() const
	{
		return range.ToString() + " text " + Helper::Escape(value);
	}

	std::string PunctNode::ToString() const
	{
		std::string typeString;

		return range.ToString() + " punct " + value;
	}

	std::string EmojiNode::ToString() const
	{
		return range.ToString() + " emoji " + value;
	}

	std::string FSNode::ToString() const
	{
		std::string string = range.ToString() + " fs ";

		switch (type)
		{
			case FSType::Bold:
				string += "bold";
				break;
			case FSType::Italic:
				string += "italic";
				break;
			case FSType::Monospace:
				string += "monospace";
				break;

			default:
				break;
		}

		return string;
	}

	std::string HeadingNode::ToString() const
	{
		std::string string = range.ToString() + " ";

		switch (type)
		{
			case HeadingType::Chapter:
				string += "chapter";
				break;
			case HeadingType::Section:
				string += "section";
				break;
			case HeadingType::Subsection:
				string += "subsection";
				break;
			case HeadingType::Subtitle:
				string += "subtitle";
				break;
		}

		if (!id.empty())
		{
			string += " id:" + id;
		}

		return string;
	}

	std::string ParagraphNode::ToString() const
	{
		return range.ToString() + " paragraph";
	}

	std::string AdmonNode::ToString() const
	{
		std::string string = range.ToString() + " admon ";

		switch (type)
		{
			case AdmonType::Note:
				string += "note";
				break;
			case AdmonType::Hint:
				string += "hint";
				break;
			case AdmonType::Important:
				string += "important";
				break;
			case AdmonType::Warning:
				string += "warning";
				break;
			case AdmonType::SeeAlso:
				string += "seeAlso";
				break;
			case AdmonType::Tip:
				string += "tip";
				break;
		}
		return string;
	}

	std::string FootnoteJumpNode::ToString() const
	{
		return range.ToString() + " footnoteJump " + value;
	}

	std::string FootnoteDescribeNode::ToString() const
	{
		return range.ToString() + " footnoteDescribe " + value;
	}

	std::string IndentNode::ToString() const
	{
		return range.ToString() + " indent";
	}

	std::string BQNode::ToString() const
	{
		return range.ToString() + " bq";
	}

	std::string BreakNode::ToString() const
	{
		switch (type)
		{
			case BreakType::Line:
				return range.ToString() + " lineBreak";
			case BreakType::Page:
				return range.ToString() + " pageBreak";
			case BreakType::Thematic:
				return range.ToString() + " thematicBreak";
		}
	}

	std::string ListNode::ToString() const
	{
		switch (type)
		{
			case ListType::Bullet:
				return range.ToString() + " bulletList";
			case ListType::Index:
				return range.ToString() + " indexList";
			case ListType::Check:
				return range.ToString() + " checkList";
		}
	}

	std::string ListItemNode::ToString() const
	{
		std::string string = range.ToString() + " listItem ";

		switch (state)
		{
			case ListItemState::None:
				break;
			case ListItemState::Todo:
				string += "todo";
				break;
			case ListItemState::Done:
				string += "done";
				break;
			case ListItemState::Cancelled:
				string += "canceled";
				break;
		}

		return string;
	}

	std::string TableNode::ToString() const
	{
		std::string string = range.ToString() + " table";
		if (header > 0)
		{
			string += " header:" + std::to_string(header);
		}
		if (footer > 0)
		{
			string += " footer:" + std::to_string(footer);
		}

		if (!alignments.empty())
		{
			string += " alignments:";
			for (Alignment alignment : alignments)
			{
				switch (alignment)
				{
					case Alignment::Left:
						string += "left,";
						break;
					case Alignment::Center:
						string += "center,";
						break;
					case Alignment::Right:
						string += "right,";
						break;
				}
			}
			string.pop_back();
		}

		return string;
	}

	std::string TableRowNode::ToString() const
	{
		return range.ToString() + " tableRow";
	}

	std::string TableCellNode::ToString() const
	{
		return range.ToString() + " tableCell";
	}

	std::string ResourceNode::ToString() const
	{
		std::string string = range.ToString() + " ";

		switch (type)
		{
			case ResourceType::Link:
				string = "linker";
				break;
			case ResourceType::LinkLocal:
				string = "linkerLocal";
				break;
			case ResourceType::Present:
				string = "presenter";
				break;
			case ResourceType::PresentLocal:
				string = "presenterLocal";
				break;
		}

		if (!value.empty())
		{
			string += " value:" + Helper::Escape(value);
		}

		if (!id.empty())
		{
			string += " id:" + id;
		}

		if (!label.empty())
		{
			string += " label:" + Helper::Escape(label);
		}

		return string;
	}

	std::string CodeNode::ToString() const
	{
		std::string string = range.ToString() + " ";
		string += type == CodeType::Block ? "codeblock" : "code";

		if (!lang.empty())
		{
			string += " lang:" + lang;
		}

		if (!source.empty())
		{
			string += " source:" + Helper::Escape(source);
		}

		return string;
	}

	std::string DateTimeNode::ToString() const
	{
		std::string string = range.ToString() + " datetime";

		if (!date.empty())
		{
			string += " date:" + date;
		}

		if (!time.empty())
		{
			string += " time:" + time;
		}

		return string;
	}
}
