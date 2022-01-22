#include "NodeType.hpp"

namespace Gularen
{
	std::string toString(NodeType type)
	{
		switch (type) {
			case NodeType::unknown:
				return "unknown";
			case NodeType::root:
				return "rootNode";

			case NodeType::newline:
				return "newline";

			case NodeType::symbol:
				return "symbol";
			case NodeType::curtain:
				return "curtain";
			case NodeType::wrapper:
				return "container";

			case NodeType::text:
				return "text";
			case NodeType::quotedText:
				return "quotedText";
			case NodeType::rawText:
				return "rawText";
			case NodeType::indent:
				return "indent";

			case NodeType::lineBreak:
				return "lineBreak";
			case NodeType::thematicBreak:
				return "thematicBreak";
			case NodeType::pageBreak:
				return "pageBreak";

			case NodeType::title:
				return "title";
			case NodeType::part:
				return "part";
			case NodeType::chapter:
				return "chapter";
			case NodeType::section:
				return "section";
			case NodeType::subsection:
				return "subsection";
			case NodeType::subsubsection:
				return "subsubsection";
			case NodeType::minisection:
				return "minisection";
			case NodeType::paragraph:
				return "Paragraph";

			case NodeType::inlineCode:
				return "inlineCode";
			case NodeType::code:
				return "code";

			case NodeType::table:
				return "table";
			case NodeType::tableRow:
				return "tableRow";
			case NodeType::tableColumn:
				return "tableColumn";

			case NodeType::file:
				return "file";
			case NodeType::toc:
				return "toc";
			case NodeType::block:
				return "block";
			case NodeType::assignment:
				return "assignment";

			case NodeType::link:
				return "link";
			case NodeType::localLink:
				return "localLink";

			case NodeType::inlineImage:
				return "inlineImage";
			case NodeType::image:
				return "image";

			case NodeType::formatBold:
				return "formatBold";
			case NodeType::formatItalic:
				return "formatItalic";
			case NodeType::formatMonospace:
				return "formatMonospace";

			case NodeType::list:
				return "list";
			case NodeType::numericList:
				return "numericList";
			case NodeType::item:
				return "item";

			case NodeType::checkList:
				return "checkList";
			case NodeType::checkItem:
				return "checkItem";

			default:
				return "[unhandled]";
		}
	}
}
