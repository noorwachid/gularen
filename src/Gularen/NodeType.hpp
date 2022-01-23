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
		subSection,
		subSubSection,
		miniSection,
		paragraph,

		table,
		row,
		cell,

		code,
		file,
		image,
		toc,
		index,
		reference,
		admonition,
		assignment,

		hashtag,
		username,
		symbol,
		container,

		string,
		array,

		// quote
		openSingleQuote,
		closeSingleQuote,
		openDoubleQuote,
		closeDoubleQuote,

		writer,

		urlLink,
		markerLink,
		referenceLink,

		inlineImage,
		indexDefinition,

		// Font style types
		boldFormat,
		italicFormat,
		monospaceFormat,

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