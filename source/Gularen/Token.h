#pragma once

#include <string>

namespace Gularen
{
	enum class TokenType
	{
		Comment,

		IndentIncr,
		IndentDecr,

		BQIncr,
		BQDecr,

		Newline,
		NewlinePlus,

		Text,

		CodeMark,
		CodeLang,
		CodeSource,

		ChapterMark,
		SectionMark,
		SubsectionMark,

		HeadingID,
		HeadingIDMark,

		FSBold,
		FSItalic,
		FSMonospace,

		Bullet,
		Checkbox,
		Index,

		Break,
		Dinkus,

		Pipe,
		PipeConnector,

		Date,
		Time,
		DateTime,

		SquareOpen,
		SquareClose,

		CurlyOpen,
		CurlyClose,

		Resource,
		ResourceIDMark,
		ResourceID,
		ResourceLabel,

		ParenOpen,
		ParenClose,

		PresentMark,
		IncludeMark,
		JumpMark,
		JumpID,
		DescribeMark,

		AdmonNote,
		AdmonHint,
		AdmonImportant,
		AdmonWarning,
		AdmonSeeAlso,
		AdmonTip,

		EmojiMark,
		EmojiCode,

		Hyphen,
		EnDash,
		EmDash,

		SingleQuoteOpen,
		SingleQuoteClose,
		QuoteOpen,
		QuoteClose,

		End,
	};

	struct Position
	{
		size_t line;
		size_t column;

		Position() : line(1), column(1)
		{
		}

		Position(size_t line, size_t column) : line(line), column(column)
		{
		}

		bool operator==(const Position& other) const
		{
			return line == other.line && column == other.column;
		}

		std::string ToString() const;
	};

	struct Range
	{
		Position begin;
		Position end;

		std::string ToString() const;
	};

	struct Token
	{
		TokenType type;
		Range range;
		std::string value;

		std::string ToString() const;
	};

	using Tokens = std::vector<Token>;
}
