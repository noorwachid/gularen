#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge {
namespace Html {

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
    titleBuffer.clear();
    Traverse(tree);
}

std::string Renderer::GetBuffer()
{
    return R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>)" + (!titleBuffer.empty() ? titleBuffer : "Untitled") + R"(</title>
    )" + styleBuffer + R"(
</head>
<body>
)" + buffer + R"(
</body>
</html>
)";
}

std::string Renderer::GetContentBuffer()
{
    return buffer;
}

void Renderer::SetStyle(const std::string& style)
{
    if (style == "none")
        styleBuffer.clear();
    else if (style == "dark")
        styleBuffer = R"(
            <style>
            *{margin:0;padding:0;box-sizing:border-box}
            html{font-size:62.8%}
            body{font-size:1.6rem;font-family:sans-serif;background-color:#232324;color:#fafafa;line-height:1.3;max-width:80rem;padding:1rem}
            h1{font-size:4rem}
            h2{font-size:3rem}
            h3{font-size:2.4rem}
            h4{font-size:2.0rem}
            h5{font-size:1.8rem}
            p,ul,ol,table,pre>code,hr{margin-bottom:1rem}
            ul,ol,blockquote{margin-left:2.5rem}
            table{border-collapse:collapse}
            table td{border:1px solid #889;padding:0.75rem 1rem}
            .checklist{list-style:none;margin-left:0}
            .checklist>.item{display:flex;flex-direction:row;align-items:baseline}
            .checklist>.item>input[type=checkbox]{margin-right:0.5rem}
            .checklist>.item>label{flex-grow:1}
            hr{border:0;border-top:0.1rem solid #e4e5e6}
            code{background: #f4f5f6;font-size:1.5rem;padding:.2rem.5rem;white-space: nowrap}
            pre>code{display:block;white-space:pre;padding:1rem 1.5rem;line-height:1.2}
            </style>
        )";
    else
        styleBuffer = R"(
            <style>
            *{margin:0;padding:0;box-sizing:border-box}
            html{font-size:62.8%}
            body{font-size:1.6rem;font-family:sans-serif;color:#232326;line-height:1.3;max-width:80rem;padding:1rem}
            h1{font-size:4rem}
            h2{font-size:3rem}
            h3{font-size:2.4rem}
            h4{font-size:2.0rem}
            h5{font-size:1.8rem}
            p,ul,ol,table,pre>code,hr{margin-bottom:1rem}
            ul,ol,blockquote{margin-left:2.5rem}
            table{border-collapse:collapse}
            table td{border:1px solid #889;padding:0.75rem 1rem}
            .checklist{list-style:none;margin-left:0}
            .checklist>.item{display:flex;flex-direction:row;align-items:baseline}
            .checklist>.item>input[type=checkbox]{margin-right:0.5rem}
            .checklist>.item>label{flex-grow:1}
            hr{border:0;border-top:0.1rem solid #e4e5e6}
            code{background: #f4f5f6;font-size:1.5rem;padding:.2rem.5rem;white-space: nowrap}
            pre>code{display:block;white-space:pre;padding:1rem 1.5rem;line-height:1.2;background-color:#010102;}
            </style>
        )";

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
        {
            std::string value = static_cast<ValueNode*>(node)->value;
            buffer += value;

            if (inTitle)
                titleBuffer += value;
            break;
        }

        case NodeType::FBold:
            buffer += "<b>";
            break;
        case NodeType::FItalic:
            buffer += "<i>";
            break;
        case NodeType::FMonospace:
            buffer += "<code>";
            break;

        case NodeType::Paragraph:
            buffer += "<p>";
            break;

        case NodeType::Indent:
            buffer += "<blockquote>";
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
            if (titleBuffer.empty())
                inTitle = true;
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
            buffer += "<ul class=\"checklist\">\n";
            break;
        case NodeType::CheckItem:
            buffer += "<li class=\"item\">";
            buffer += "<input type=\"checkbox\"";
            if (static_cast<TernaryNode*>(node)->state == TernaryState::True)
                buffer += " checked";
            if (static_cast<TernaryNode*>(node)->state == TernaryState::InBetween)
                buffer += " indeterminate";
            buffer += "><label>";
            break;

        case NodeType::Link:
        {
            Node* packageNode = static_cast<ContainerNode*>(node)->package;
            ValueNode* valueNode = static_cast<ValueNode*>(packageNode);
            buffer += "<a href=\"" + valueNode->value + "\">";
            if (packageNode->children.empty())
                buffer += valueNode->value;
            break;
        }
        case NodeType::LocalLink:
        {
            Node* packageNode = static_cast<ContainerNode*>(node)->package;
            ValueNode* valueNode = static_cast<ValueNode*>(packageNode);
            buffer += "<a href=\"#" + valueNode->value + "\">";
            if (packageNode->children.empty())
                buffer += valueNode->value;
            break;
        }

        case NodeType::Table:
            buffer += "<table>\n";
            break;
        case NodeType::TableRow:
            buffer += "<tr>\n";
            break;
        case NodeType::TableColumn:
            buffer += "<td>";
            break;

        case NodeType::InlineCode:
            buffer += "<code>";
            buffer += Escape(static_cast<ValueNode*>(node)->value);
            buffer += "</code>\n";
            break;

        case NodeType::Code:
        {
            CodeNode* codeNode = static_cast<CodeNode*>(node);
            buffer += "<pre><code>";
            buffer += Escape(codeNode->value);
            buffer += "</code></pre>\n";
            break;
        }

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
            inTitle = false;
            break;

        case NodeType::FBold:
            buffer += "</b>";
            break;
        case NodeType::FItalic:
            buffer += "</i>";
            break;
        case NodeType::FMonospace:
            buffer += "</code>";
            break;

        case NodeType::Paragraph:
            buffer += "</p>\n";
            break;

        case NodeType::Indent:
            buffer += "</blockquote>";
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
        case NodeType::CheckList:
            buffer += "</ul>\n";
            break;
        case NodeType::NList:
            buffer += "</ol>\n";
            break;
        case NodeType::Item:
            break;
        case NodeType::CheckItem:
            buffer += "</label></li>\n";
            break;


        case NodeType::Link:
        case NodeType::LocalLink:
            buffer += "</a>";
            break;

        case NodeType::Table:
            buffer += "</table>\n";
            break;
        case NodeType::TableRow:
            buffer += "</tr>\n";
            break;
        case NodeType::TableColumn:
            buffer += "</td>\n";
            break;

        default:
            break;
    }
}

std::string Renderer::Escape(const std::string &raw)
{
    std::string escaped;

    for (size_t i = 0; i < raw.size(); ++i)
    {
        switch (raw[i]) {
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            default:
                escaped += raw[i];
        }
    }

    return escaped;
}

}
}
