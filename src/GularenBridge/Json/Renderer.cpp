#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge
{
	namespace Json
	{
		Renderer::Renderer()
		{
		}

		void Renderer::setTree(Node* tree)
		{
			this->rootNode = tree;
			buffer.clear();
		}

		void Renderer::parse()
		{
			traverse(rootNode);
		}

		std::string Renderer::getBuffer()
		{
			return buffer;
		}

		void Renderer::traverse(Node* node)
		{
			preTraverse(node);

			for (size_t i = 0; i < node->children.size(); ++i) {
				if (i > 0)
					buffer += ",";
				traverse(node->children[i]);
			}

			postTraverse(node);
		}

		void Renderer::preTraverse(Node* node)
		{
			buffer += "{\"type\":\"" + Gularen::toString(node->type) + "\"";

			switch (node->type) {
				case NodeType::text:
				case NodeType::inlineCode:
					buffer += ",\"value\":\"" + escapeBuffer(static_cast<ValueNode*>(node)->value) + "\"}";
					break;

				case NodeType::paragraph:
				case NodeType::indent:
				case NodeType::wrapper:
				case NodeType::formatBold:
				case NodeType::formatItalic:
				case NodeType::formatMonospace:
				case NodeType::root:
					buffer += ",\"children\":[";
					break;

				case NodeType::lineBreak:
				case NodeType::thematicBreak:
				case NodeType::pageBreak:
					buffer += "}";
					break;

				case NodeType::newline:
					buffer += ",\"size\":" + std::to_string(static_cast<SizeNode*>(node)->size) + "}";
					break;

				case NodeType::title:
				case NodeType::part:
				case NodeType::chapter:
				case NodeType::section:
				case NodeType::subsection:
				case NodeType::subsubsection:
				case NodeType::minisection:
				case NodeType::list:
				case NodeType::numericList:
				case NodeType::checkList:
				case NodeType::item:
				case NodeType::checkItem:
				case NodeType::table:
				case NodeType::tableRow:
				case NodeType::tableColumn:
					buffer += ",\"children\":[";
					break;

				case NodeType::code: {
					CodeNode* code = static_cast<CodeNode*>(node);
					if (code->langCode)
						buffer += ",\"langCode\":\"" + static_cast<ValueNode*>(code->langCode)->value + "\"";
					buffer += ",\"buffer\":\"" + escapeBuffer(code->value) + "\"";
					buffer += "}";
					break;
				}

				case NodeType::link:
					break;

				default:
					break;
			}
		}

		void Renderer::postTraverse(Gularen::Node* node)
		{
			switch (node->type) {
				case NodeType::paragraph:
				case NodeType::indent:
				case NodeType::wrapper:
				case NodeType::formatBold:
				case NodeType::formatItalic:
				case NodeType::formatMonospace:
				case NodeType::root:
				case NodeType::title:
				case NodeType::part:
				case NodeType::chapter:
				case NodeType::section:
				case NodeType::subsection:
				case NodeType::subsubsection:
				case NodeType::minisection:
				case NodeType::list:
				case NodeType::numericList:
				case NodeType::checkList:
				case NodeType::item:
				case NodeType::checkItem:
				case NodeType::table:
				case NodeType::tableRow:
				case NodeType::tableColumn:
					buffer += "]}";
					break;

				default:
					break;
			}
		}

		std::string Renderer::escapeBuffer(const std::string& buffer)
		{
			std::string out;

			for (size_t i = 0; i < buffer.size(); ++i) {
				switch (buffer[i]) {
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
