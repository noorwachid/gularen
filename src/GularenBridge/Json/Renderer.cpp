#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge
{
	namespace Json
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
			Traverse(mTree);
		}

		std::string Renderer::GetBuffer()
		{
			return mBuffer;
		}

		void Renderer::Traverse(Node* node)
		{
			PreTraverse(node);

			for (size_t i = 0; i < node->children.size(); ++i)
			{
				if (i > 0)
					mBuffer += ",";
				Traverse(node->children[i]);
			}

			PostTraverse(node);
		}

		void Renderer::PreTraverse(Node* node)
		{
			mBuffer += "{\"type\":\"" + Gularen::ToString(node->type) + "\"";

			switch (node->type)
			{
				case NodeType::Text:
				case NodeType::InlineCode:
					mBuffer += ",\"value\":\"" + EscapeBuffer(static_cast<ValueNode*>(node)->value) + "\"}";
					break;

				case NodeType::Paragraph:
				case NodeType::Indent:
				case NodeType::Wrapper:
				case NodeType::FBold:
				case NodeType::FItalic:
				case NodeType::FMonospace:
				case NodeType::Root:
					mBuffer += ",\"children\":[";
					break;

				case NodeType::LineBreak:
				case NodeType::ThematicBreak:
				case NodeType::PageBreak:
					mBuffer += "}";
					break;

				case NodeType::Newline:
					mBuffer += ",\"size\":" + std::to_string(static_cast<SizeNode*>(node)->size) + "}";
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
					mBuffer += ",\"children\":[";
					break;

				case NodeType::Code:
				{
					CodeNode* code = static_cast<CodeNode*>(node);
					if (code->lang)
						mBuffer += ",\"lang\":\"" + static_cast<ValueNode*>(code->lang)->value + "\"";
					mBuffer += ",\"mBuffer\":\"" + EscapeBuffer(code->value) + "\"";
					mBuffer += "}";
					break;
				}

				case NodeType::Link:
					break;

				default:
					break;
			}
		}

		void Renderer::PostTraverse(Gularen::Node* node)
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
					mBuffer += "]}";
					break;

				default:
					break;
			}
		}

		std::string Renderer::EscapeBuffer(const std::string& buffer)
		{
			std::string out;

			for (size_t i = 0; i < buffer.size(); ++i)
			{
				switch (buffer[i])
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
						out += buffer[i];
						break;
				}
			}

			return out;
		}
	}
}
