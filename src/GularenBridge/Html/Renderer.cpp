#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge
{
	namespace Html
	{
		Renderer::Renderer()
		{
		}

		void Renderer::SetTree(Node* tree)
		{
			this->mTree = tree;
			mBuffer.clear();
		}

		void Renderer::Parse()
		{
			mTitleBuffer.clear();
			Traverse(mTree);
		}

		std::string Renderer::GetBuffer()
		{
			return R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>)" + (!mTitleBuffer.empty() ? mTitleBuffer : "Untitled") + R"(</title>
    )" + mStyleBuffer + R"(
</head>
<body>
)" + mBuffer + R"(
</body>
</html>
)";
		}

		std::string Renderer::GetContentBuffer()
		{
			return mBuffer;
		}

		void Renderer::SetStyle(const std::string& style)
		{
			if (style == "none")
				mStyleBuffer.clear();
			else if (style == "dark")
				mStyleBuffer = R"(
            <style>
            *{margin:0;padding:0;box-sizing:border-box}
            html{font-size:62.8%}
            body{font-size:1.6rem;font-family:sans-serif;background-color:#232324;color:#fafafa;line-height:1.3;max-width:80rem;padding:1rem;margin-left:auto; margin-right:auto;}
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
            .checklist>.item>mInput[type=checkbox]{margin-right:0.5rem}
            .checklist>.item>label{flex-grow:1}
            hr{border:0;border-top:0.1rem solid #e4e5e6}
            code{background: #f4f5f6;font-size:1.5rem;padding:.2rem.5rem;white-space: nowrap}
            pre>code{display:block;white-space:pre;padding:1rem 1.5rem;line-height:1.2;background-color:#010102;}
            </style>
        )";
			else
				mStyleBuffer = R"(
            <style>
            *{margin:0;padding:0;box-sizing:border-box}
            html{font-size:62.8%}
            body{font-size:1.6rem;font-family:sans-serif;color:#232326;line-height:1.3;max-width:80rem;padding:1rem;margin-left:auto; margin-right:auto;}
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
            .checklist>.item>mInput[type=checkbox]{margin-right:0.5rem}
            .checklist>.item>label{flex-grow:1}
            hr{border:0;border-top:0.1rem solid #e4e5e6}
            code{background: #f4f5f6;font-size:1.5rem;padding:.2rem.5rem;white-space: nowrap}
            pre>code{display:block;white-space:pre;padding:1rem 1.5rem;line-height:1.2;background-color:#eee;}
            </style>
        )";

		}

		void Renderer::Traverse(Node* node)
		{
			PreTraverse(node);

			for (Node* child: node->children)
				Traverse(child);

			PostTraverse(node);
		}

		void Renderer::PreTraverse(Node* node)
		{
			switch (node->type)
			{
				case NodeType::Text:
				{
					std::string value = static_cast<ValueNode*>(node)->value;
					mBuffer += value;

					if (mInTitle)
						mTitleBuffer += value;
					break;
				}

				case NodeType::FBold:
					mBuffer += "<b>";
					break;
				case NodeType::FItalic:
					mBuffer += "<i>";
					break;
				case NodeType::FMonospace:
					mBuffer += "<code>";
					break;

				case NodeType::Paragraph:
					mBuffer += "<p>";
					break;

				case NodeType::Indent:
					mBuffer += "<blockquote>";
					break;

				case NodeType::LineBreak:
					mBuffer += "<br>";
					break;
				case NodeType::ThematicBreak:
					mBuffer += "<hr>\n";
					break;
				case NodeType::PageBreak:
					break;
				case NodeType::Newline:
					mBuffer += " ";
					break;

				case NodeType::Title:
					mBuffer += "<h1>";
					if (mTitleBuffer.empty())
						mInTitle = true;
					break;

				case NodeType::Part:
					mBuffer += "<div class=\"part\">";
					break;
				case NodeType::Chapter:
					mBuffer += "<div class=\"chapter\">";
					break;

				case NodeType::Section:
					mBuffer += "<h2>";
					break;
				case NodeType::Subsection:
					mBuffer += "<h3>";
					break;
				case NodeType::Subsubsection:
					mBuffer += "<h4>";
					break;
				case NodeType::Minisection:
					mBuffer += "<h5>";
					break;

				case NodeType::List:
					mBuffer += "<ul>\n";
					break;
				case NodeType::NList:
					mBuffer += "<ol>\n";
					break;
				case NodeType::Item:
					mBuffer += "<li>";
					break;

				case NodeType::CheckList:
					mBuffer += "<ul class=\"checklist\">\n";
					break;
				case NodeType::CheckItem:
					mBuffer += "<li class=\"item\">";
					mBuffer += "<mInput type=\"checkbox\"";
					if (static_cast<TernaryNode*>(node)->state == TernaryState::True)
						mBuffer += " checked";
					if (static_cast<TernaryNode*>(node)->state == TernaryState::InBetween)
						mBuffer += " indeterminate";
					mBuffer += "><label>";
					break;

				case NodeType::Link:
				{
					Node* packageNode = static_cast<ContainerNode*>(node)->package;
					ValueNode* valueNode = static_cast<ValueNode*>(packageNode);
					mBuffer += "<a href=\"" + valueNode->value + "\">";
					if (packageNode->children.empty())
						mBuffer += valueNode->value;
					break;
				}
				case NodeType::LocalLink:
				{
					Node* packageNode = static_cast<ContainerNode*>(node)->package;
					ValueNode* valueNode = static_cast<ValueNode*>(packageNode);
					mBuffer += "<a href=\"#" + valueNode->value + "\">";
					if (packageNode->children.empty())
						mBuffer += valueNode->value;
					break;
				}

				case NodeType::Table:
					mBuffer += "<table>\n";
					break;
				case NodeType::TableRow:
					mBuffer += "<tr>\n";
					break;
				case NodeType::TableColumn:
					mBuffer += "<td>";
					break;

				case NodeType::InlineImage:
				case NodeType::Image:
				{
					ValueNode* package = static_cast<ValueNode*>(static_cast<ContainerNode*>(node)->package);
					mBuffer += "<img src=\"" + package->value + "\">";
					break;
				}

				case NodeType::InlineCode:
					mBuffer += "<code>";
					mBuffer += EscapeBuffer(static_cast<ValueNode*>(node)->value);
					mBuffer += "</code>\n";
					break;

				case NodeType::Code:
				{
					CodeNode* codeNode = static_cast<CodeNode*>(node);
					mBuffer += "<pre><code>";
					mBuffer += EscapeBuffer(codeNode->value);
					mBuffer += "</code></pre>\n";
					break;
				}

				default:
					break;
			}
		}

		void Renderer::PostTraverse(Node* node)
		{
			switch (node->type)
			{
				case NodeType::Title:
					mBuffer += "</h1>\n";
					mInTitle = false;
					break;

				case NodeType::FBold:
					mBuffer += "</b>";
					break;
				case NodeType::FItalic:
					mBuffer += "</i>";
					break;
				case NodeType::FMonospace:
					mBuffer += "</code>";
					break;

				case NodeType::Paragraph:
					mBuffer += "</p>\n";
					break;

				case NodeType::Indent:
					mBuffer += "</blockquote>";
					break;

				case NodeType::Part:
					mBuffer += "</div>\n";
					break;
				case NodeType::Chapter:
					mBuffer += "</div>\n";
					break;

				case NodeType::Section:
					mBuffer += "</h2>\n";
					break;
				case NodeType::Subsection:
					mBuffer += "</h3>\n";
					break;
				case NodeType::Subsubsection:
					mBuffer += "</h4>\n";
					break;
				case NodeType::Minisection:
					mBuffer += "</h5>\n";
					break;

				case NodeType::List:
				case NodeType::CheckList:
					mBuffer += "</ul>\n";
					break;
				case NodeType::NList:
					mBuffer += "</ol>\n";
					break;
				case NodeType::Item:
					break;
				case NodeType::CheckItem:
					mBuffer += "</label></li>\n";
					break;


				case NodeType::Link:
				case NodeType::LocalLink:
					mBuffer += "</a>";
					break;

				case NodeType::Table:
					mBuffer += "</table>\n";
					break;
				case NodeType::TableRow:
					mBuffer += "</tr>\n";
					break;
				case NodeType::TableColumn:
					mBuffer += "</td>\n";
					break;

				default:
					break;
			}
		}

		std::string Renderer::EscapeBuffer(const std::string& buffer)
		{
			std::string escaped;

			for (size_t i = 0; i < buffer.size(); ++i)
			{
				switch (buffer[i])
				{
					case '<':
						escaped += "&lt;";
						break;
					case '>':
						escaped += "&gt;";
						break;
					default:
						escaped += buffer[i];
				}
			}

			return escaped;
		}
	}
}
