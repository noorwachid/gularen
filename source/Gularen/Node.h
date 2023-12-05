#pragma once

#include "Gularen/Token.h"
#include <memory>
#include <stack>

namespace Gularen
{
	enum class NodeGroup
	{
		Document,

		Comment,
		Text,
		FS,
		Heading,
		Paragraph,
		Indent,
		BQ,
		Break,
		List,
		ListItem,

		Table,
		TableRow,
		TableCell,

		Resource,

		Code,

		Punct,
		Emoji,

		Admon,

		FootnoteJump,
		FootnoteDescribe,

		DateTime,
	};

	struct Node;

	using NodePtr = std::shared_ptr<Node>;
	using NodeChildren = std::vector<NodePtr>;

	struct Node
	{
		NodeGroup group;
		NodeChildren children;

		Range range;

		template <class T>
		const T& As()
		{
			return *static_cast<T*>(this);
		}

		virtual std::string ToString() const;

		static std::string Escape(const std::string& from);
	};

	struct DocumentNode : Node
	{
		std::string path;

		DocumentNode(const std::string& path);

		virtual std::string ToString() const override;
	};

	struct CommentNode : Node
	{
		std::string value;

		CommentNode(const std::string& value);

		virtual std::string ToString() const override;
	};

	struct TextNode : Node
	{
		std::string value;

		TextNode(const std::string& value);

		virtual std::string ToString() const override;
	};

	enum class PunctType
	{
		Hyphen,
		EnDash,
		EmDash,

		SingleQuoteOpen,
		SingleQuoteClose,
		QuoteOpen,
		QuoteClose,
	};

	struct PunctNode : Node
	{
		PunctType type;
		std::string value;

		PunctNode(PunctType type, const std::string& value);

		virtual std::string ToString() const override;
	};

	struct EmojiNode : Node
	{
		std::string value;

		EmojiNode(const std::string& value);

		virtual std::string ToString() const override;
	};

	enum class FSType
	{
		Bold,
		Italic,
		Monospace
	};

	struct FSNode : Node
	{
		FSType type;

		FSNode(FSType type);

		virtual std::string ToString() const override;
	};

	enum class HeadingType
	{
		Chapter,
		Section,
		Subsection,
		Subtitle,
	};

	struct HeadingNode : Node
	{
		HeadingType type;
		std::string id;

		HeadingNode(HeadingType type);

		virtual std::string ToString() const override;
	};

	struct ParagraphNode : Node
	{
		ParagraphNode();

		virtual std::string ToString() const override;
	};

	enum class AdmonType
	{
		Note,
		Hint,
		Important,
		Warning,
		SeeAlso,
		Tip,
	};

	struct AdmonNode : Node
	{
		AdmonType type;

		AdmonNode(AdmonType type);

		virtual std::string ToString() const override;
	};

	struct FootnoteJumpNode : Node
	{
		std::string value;

		FootnoteJumpNode(const std::string& value);

		virtual std::string ToString() const override;
	};

	struct FootnoteDescribeNode : Node
	{
		std::string value;

		FootnoteDescribeNode(const std::string& value);

		virtual std::string ToString() const override;
	};

	struct IndentNode : Node
	{
		bool skipable = false;

		IndentNode()
		{
			group = NodeGroup::Indent;
		}

		virtual std::string ToString() const override;
	};

	struct BQNode : Node
	{
		BQNode()
		{
			group = NodeGroup::BQ;
		}

		virtual std::string ToString() const override;
	};

	enum class BreakType
	{
		Line,
		Page,
		Thematic,
	};

	struct BreakNode : Node
	{
		BreakType type;

		BreakNode(BreakType type);

		virtual std::string ToString() const override;
	};

	enum class ListType
	{
		Bullet,
		Index,
		Check,
	};

	struct ListNode : Node
	{
		ListType type;

		ListNode(ListType type);

		virtual std::string ToString() const override;
	};

	enum class ListItemState
	{
		None,
		Todo,
		Done,
		Cancelled,
	};

	struct ListItemNode : Node
	{
		ListItemState state;
		size_t index;

		ListItemNode(size_t index);

		ListItemNode(size_t index, ListItemState state);

		virtual std::string ToString() const override;
	};

	enum class Alignment
	{
		Left = 0,
		Center = 1,
		Right = 2,
	};

	struct TableNode : Node
	{
		size_t header = 0;
		size_t footer = 0;
		std::vector<Alignment> alignments;

		TableNode();

		virtual std::string ToString() const override;
	};

	struct TableRowNode : Node
	{
		TableRowNode();

		virtual std::string ToString() const override;
	};

	struct TableCellNode : Node
	{
		TableCellNode()
		{
			group = NodeGroup::TableCell;
		}

		virtual std::string ToString() const override;
	};

	enum class ResourceType
	{
		Link,
		LinkLocal,
		Present,
		PresentLocal,
	};

	struct ResourceNode : Node
	{
		ResourceType type;
		std::string value;
		std::string id;
		std::string label;

		ResourceNode(ResourceType type, const std::string& value);

		virtual std::string ToString() const override;
	};

	enum class CodeType
	{
		Inline,
		Block,
	};

	struct CodeNode : Node
	{
		CodeType type;

		std::string lang;
		std::string source;

		CodeNode()
		{
			group = NodeGroup::Code;
		}

		virtual std::string ToString() const override;
	};

	struct DateTimeNode : Node
	{
		std::string date;
		std::string time;

		DateTimeNode()
		{
			group = NodeGroup::DateTime;
		}

		virtual std::string ToString() const override;
	};
}
