#include "NodeType.hpp"

namespace Gularen {

std::string ToString(NodeType type)
{
    switch (type)
    {
        case NodeType::Unknown: return "Unknown";
        case NodeType::Root: return "Root";

        case NodeType::Newline: return "Newline";

        case NodeType::Symbol: return "Symbol";
        case NodeType::Curtain: return "Curtain";
        case NodeType::Wrapper: return "Wrapper";

        case NodeType::Text: return "Text";
        case NodeType::QuotedText: return "QuoteText";
        case NodeType::RawText: return "RawText";
        case NodeType::Indent: return "Indent";

        case NodeType::LineBreak: return "LineBreak";
        case NodeType::ThematicBreak: return "ThematicBreak";
        case NodeType::PageBreak: return "PageBreak";

        case NodeType::Title: return "Title";
        case NodeType::Part: return "Part";
        case NodeType::Chapter: return "Chapter";
        case NodeType::Section: return "Section";
        case NodeType::Subsection: return "Subsection";
        case NodeType::Subsubsection: return "Subsubsection";
        case NodeType::Minisection: return "Minisection";
        case NodeType::Paragraph: return "Paragraph";

        case NodeType::InlineCode: return "InlineCode";
        case NodeType::Code: return "Code";
        case NodeType::Table: return "Table";
        case NodeType::TableRow: return "TableRow";
        case NodeType::TableColumn: return "TableColumn";

        case NodeType::Link: return "Link";
        case NodeType::LocalLink: return "LocalLink";

        case NodeType::InlineImage: return "InlineImage";
        case NodeType::Image: return "Image";

        case NodeType::FBold: return "FBold";
        case NodeType::FItalic: return "FItalic";
        case NodeType::FMonospace: return "FMonospace";

        case NodeType::List: return "List";
        case NodeType::NList: return "NList";
        case NodeType::Item: return "Item";

        case NodeType::CheckList: return "CheckList";
        case NodeType::CheckItem: return "CheckItem";

        default: return "[Unhandled]";
    }
}

}
