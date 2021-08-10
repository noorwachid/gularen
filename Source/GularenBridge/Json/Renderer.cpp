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
    buffer += "{\"Type\":\"" + Gularen::ToString(node->type) + "\"";

    switch (node->type)
    {
        case NodeType::Text:
        case NodeType::InlineCode:
            buffer += ",\"Value\":\"" + EscapeText(static_cast<ValueNode*>(node)->value) + "\"}";
            break;

        case NodeType::Paragraph:
        case NodeType::FBold:
        case NodeType::FItalic:
        case NodeType::FMonospace:
        case NodeType::Root:
            buffer += ",\"Children\":[";
            break;

        case NodeType::LineBreak:
        case NodeType::ThematicBreak:
        case NodeType::PageBreak:
            buffer += "}";
            break;

        case NodeType::Newline:
            buffer += ",\"Size\":" + std::to_string(static_cast<SizeNode*>(node)->size) + "}";
            break;

        case NodeType::Title:
        case NodeType::Part:
        case NodeType::Chapter:
        case NodeType::Section:
        case NodeType::Subsection:
        case NodeType::Subsubsection:
        case NodeType::Minisection:
            buffer += ",\"Children\":[";
            break;

        case NodeType::List:
        case NodeType::NList:
        case NodeType::CheckList:
            break;

        case NodeType::Item:
        case NodeType::CheckItem:
            break;

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
        if (text[i] == '"') out += '\\';

        out += text[i];
    }

    return out;
}

}
}
