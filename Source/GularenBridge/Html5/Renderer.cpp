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

        case NodeType::Title:
            buffer += "<h1>";
            break;

        case NodeType::Part:
            buffer += "<div class=\"part\">";
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

        default:
            break;
    }
}

}
}
