#pragma once

#include <string>

namespace Gularen
{
	enum struct NodeType
	{
		unknown = -1,
		root,

		// Abstract types
		newline,
		indent,

		lineBreak,
		thematicBreak,
		pageBreak,     // PDF or paper like only documents

		// Semi-abstract types
		title,
		part,
		chapter,
		section,
		subsection,
		subsubsection,
		minisection,
		paragraph,

		inlineCode,
		code,

		table,
		tableRow,
		tableColumn,

		file,
		toc,
		block,
		assignment,

		hashSymbol,
		atSymbol,
		symbol,
		curtain,
		wrapper,

		// value types
		text,
		quotedText,
		rawText,
		link,
		localLink,

		inlineImage,
		image,

		// Font style types
		formatBold,
		formatItalic,
		formatMonospace,

		// List types
		list,
		numericList,
		item,
		checkList,
		checkItem,

	};

	enum struct NodeShape
	{
		unknown = 0,
		line,
		inBetween,
		block,
		superBlock,
	};

	std::string toString(NodeType type);
}