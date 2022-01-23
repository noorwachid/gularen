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
			case NodeType::container:
				return "container";

			case NodeType::string:
				return "string";
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
			case NodeType::subSection:
				return "subsection";
			case NodeType::subSubSection:
				return "subSubSection";
			case NodeType::miniSection:
				return "miniSection";
			case NodeType::paragraph:
				return "paragraph";

			case NodeType::table:
				return "table";
			case NodeType::row:
				return "row";
			case NodeType::cell:
				return "cell";

			case NodeType::code:
				return "code";
			case NodeType::file:
				return "file";
			case NodeType::image:
				return "image";
			case NodeType::toc:
				return "toc";
			case NodeType::index:
				return "index";
			case NodeType::reference:
				return "reference";
			case NodeType::admonition:
				return "admonition";
			case NodeType::assignment:
				return "assignment";

			case NodeType::urlLink:
				return "urlLink";
			case NodeType::markerLink:
				return "markerLink";
			case NodeType::referenceLink:
				return "referenceLink";

			case NodeType::inlineImage:
				return "inlineImage";
			case NodeType::indexDefinition:
				return "indexDefinition";

			case NodeType::boldFormat:
				return "boldFormat";
			case NodeType::italicFormat:
				return "italicFormat";
			case NodeType::monospaceFormat:
				return "monospaceFormat";

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
