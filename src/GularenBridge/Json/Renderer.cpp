#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge {
namespace Json {

using Gularen::IO;

Renderer::Renderer()
{
}

void Renderer::SetTree(Node* tree)
{
    this->tree = tree;
    buffer.clear();
}

void Renderer::Parse()
{
    Traverse(tree);
}

std::string Renderer::GetBuffer()
{
    return buffer;
}

void Renderer::Traverse(Node* node)
{
    TraverseBeforeChildren(node);

    for (size_t i = 0; i < node->children.size(); ++i)
    {
        if (i > 0)
            buffer += ",";
        Traverse(node->children[i]);
    }

    TraverseAfterChildren(node);
}

void Renderer::TraverseBeforeChildren(Node* node)
{
    buffer += "{\"type\":\"" + Gularen::ToString(node->type) + "\"";

    switch (node->type)
    {
        case NodeType::Text:
        case NodeType::InlineCode:
            buffer += ",\"value\":\"" + EscapeText(static_cast<ValueNode*>(node)->value) + "\"}";
            break;

        case NodeType::Paragraph:
        case NodeType::Indent:
        case NodeType::Wrapper:
        case NodeType::FBold:
        case NodeType::FItalic:
        case NodeType::FMonospace:
        case NodeType::Root:
            buffer += ",\"children\":[";
            break;

        case NodeType::LineBreak:
        case NodeType::ThematicBreak:
        case NodeType::PageBreak:
            buffer += "}";
            break;

        case NodeType::Newline:
            buffer += ",\"size\":" + std::to_string(static_cast<SizeNode*>(node)->size) + "}";
            break;

        case NodeType::Title:
        case NodeType::Part:
        case NodeType::Chapter:
        case NodeType::Section:
        case NodeType::Subsection:
        case NodeType::Subsubsection:
        case NodeType::Minisection:
        case NodeType::List:
        case NodeType::NList:
        case NodeType::CheckList:
        case NodeType::Item:
        case NodeType::CheckItem:
        case NodeType::Table:
        case NodeType::TableRow:
        case NodeType::TableColumn:
            buffer += ",\"children\":[";
            break;

        case NodeType::Code:
        {
            CodeNode* code = static_cast<CodeNode*>(node);
            if (code->lang)
                buffer += ",\"lang\":\"" + static_cast<ValueNode*>(code->lang)->value + "\"";
            buffer += ",\"buffer\":\"" + EscapeText(code->value) + "\"";
            buffer += "}";
            break;
        }

        case NodeType::Link:
            break;

        default:
            break;
    }
}

void Renderer::TraverseAfterChildren(Gularen::Node *node)
{
    switch (node->type)
    {
        case NodeType::Paragraph:
        case NodeType::Indent:
        case NodeType::Wrapper:
        case NodeType::FBold:
        case NodeType::FItalic:
        case NodeType::FMonospace:
        case NodeType::Root:
        case NodeType::Title:
        case NodeType::Part:
        case NodeType::Chapter:
        case NodeType::Section:
        case NodeType::Subsection:
        case NodeType::Subsubsection:
        case NodeType::Minisection:
        case NodeType::List:
        case NodeType::NList:
        case NodeType::CheckList:
        case NodeType::Item:
        case NodeType::CheckItem:
        case NodeType::Table:
        case NodeType::TableRow:
        case NodeType::TableColumn:
            buffer += "]}";
            break;

        default:
            break;
    }
}

std::string Renderer::EscapeText(const std::string& text)
{
    std::string out;

    for (size_t i = 0; i < text.size(); ++i)
    {
        switch (text[i])
        {
            case '"':
                out += "\\\"";
                break;

            case '\n':
                out += "\\n";
                break;

            case '\t':
                out += "\\t";
                break;

            default:
                out += text[i];
                break;
        }
    }

    return out;
}

}
}
