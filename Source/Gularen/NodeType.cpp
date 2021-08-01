#include "NodeType.hpp"

namespace Gularen {

std::string ToString(NodeType type)
{
    switch (type) {
        case NodeType::Unknown: return "Unknown";
        case NodeType::Root: return "Root";

        case NodeType::Newline: return "Newline";
        case NodeType::Text: return "Text";
        case NodeType::Indent: return "Indent";

        case NodeType::Title: return "Title";
        case NodeType::Part: return "Part";
        case NodeType::Section: return "Section";
        case NodeType::Subsection: return "Subsection";
        case NodeType::Subsubsection: return "Subsubsection";
        case NodeType::Minisection: return "Minisection";
        case NodeType::Paragraph: return "Paragraph";

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
