#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge {
namespace Html5 {

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

    for (Node* child: node->children)
        Traverse(child);

    TraverseAfterChildren(node);
}

void Renderer::TraverseBeforeChildren(Node* node)
{
    switch (node->type)
    {
        case NodeType::Text:
            buffer += static_cast<ValueNode*>(node)->value;
            break;

        case NodeType::FBold:
            buffer += "<b>";
            break;
        case NodeType::FItalic:
            buffer += "<i>";
            break;
        case NodeType::FMonospace:
            buffer += "<code>";
            break;

        case NodeType::InlineCode:
            buffer += "<code>";
            buffer += static_cast<ValueNode*>(node)->value;
            break;

        case NodeType::Paragraph:
            buffer += "<p>";
            break;

        case NodeType::LineBreak:
            buffer += "<br>";
            break;
        case NodeType::ThematicBreak:
            buffer += "<hr>\n";
            break;
        case NodeType::PageBreak:
            break;
        case NodeType::Newline:
            buffer += " ";
            break;

        case NodeType::Title:
            buffer += "<h1>";
            break;

        case NodeType::Part:
            buffer += "<div class=\"part\">";
            break;
        case NodeType::Chapter:
            buffer += "<div class=\"chapter\">";
            break;

        case NodeType::Section:
            buffer += "<h2>";
            break;
        case NodeType::Subsection:
            buffer += "<h3>";
            break;
        case NodeType::Subsubsection:
            buffer += "<h4>";
            break;
        case NodeType::Minisection:
            buffer += "<h5>";
            break;

        case NodeType::List:
            buffer += "<ul>\n";
            break;
        case NodeType::NList:
            buffer += "<ol>\n";
            break;
        case NodeType::Item:
            buffer += "<li>";
            break;

        case NodeType::CheckList:
            buffer += "<div class=\"checklist\">\n";
            break;
        case NodeType::CheckItem:
            buffer += "<div class=\"checkitem\">";
            buffer += "<input type=\"checkbox\"";
            if (static_cast<BooleanNode*>(node)->state)
                buffer += " checked";
            buffer += "> ";
            break;

        default:
            break;
    }
}

void Renderer::TraverseAfterChildren(Node* node)
{
    switch (node->type)
    {
        case NodeType::Title:
            buffer += "</h1>\n";
            break;

        case NodeType::FBold:
            buffer += "</b>";
            break;
        case NodeType::FItalic:
            buffer += "</i>";
            break;
        case NodeType::FMonospace:
        case NodeType::InlineCode:
            buffer += "</code>";
            break;

        case NodeType::Paragraph:
            buffer += "</p>\n";
            break;

        case NodeType::Part:
            buffer += "</div>\n";
            break;
        case NodeType::Chapter:
            buffer += "</div>\n";
            break;

        case NodeType::Section:
            buffer += "</h2>\n";
            break;
        case NodeType::Subsection:
            buffer += "</h3>\n";
            break;
        case NodeType::Subsubsection:
            buffer += "</h4>\n";
            break;
        case NodeType::Minisection:
            buffer += "</h5>\n";
            break;

        case NodeType::List:
            buffer += "</ul>\n";
            break;
        case NodeType::NList:
            buffer += "</ol>\n";
            break;
        case NodeType::Item:
            buffer += "</li>\n";
            break;

        case NodeType::CheckList:
        case NodeType::CheckItem:
            buffer += "</div>\n";
            break;

        default:
            break;
    }
}

}
}
