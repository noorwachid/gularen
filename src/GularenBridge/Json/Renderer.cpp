#include "Renderer.hpp"
#include <Gularen/IO.hpp>

namespace GularenBridge {
	namespace Json {
		
		using Gularen::NodeType;
		using Gularen::Node;
		using Gularen::ValueNode;
		using Gularen::SizeNode;
		using Gularen::BooleanNode;
		using Gularen::ContainerNode;
		using Gularen::CodeNode;
		using Gularen::TableNode;
		using Gularen::IRenderer;

		Renderer::Renderer() {
		}

		void Renderer::setTree(Node* tree) {
			this->rootNode = tree;
			buffer.clear();
		}

		void Renderer::parse() {
			traverse(rootNode);
		}

		std::string Renderer::getBuffer() {
			return buffer;
		}

		void Renderer::traverse(Node* node) {
			preTraverse(node);

			for (size_t i = 0; i < node->children.size(); ++i) {
				if (i > 0)
					buffer += ",";
				traverse(node->children[i]);
			}

			postTraverse(node);
		}

		void Renderer::preTraverse(Node* node) {
			buffer += "{\"type\":\"" + Gularen::toString(node->type) + "\"";

			switch (node->type) {
				case NodeType::string:
					buffer += ",\"value\":\"" + escapeBuffer(static_cast<ValueNode*>(node)->value) + "\"}";
					break;

				case NodeType::paragraph:
				case NodeType::indent:
				case NodeType::container:
				case NodeType::boldFormat:
				case NodeType::italicFormat:
				case NodeType::monospaceFormat:
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
				case NodeType::subSection:
				case NodeType::subSubSection:
				case NodeType::miniSection:
				case NodeType::list:
				case NodeType::numericList:
				case NodeType::checkList:
				case NodeType::item:
				case NodeType::checkItem:
				case NodeType::table:
				case NodeType::row:
				case NodeType::cell:
					buffer += ",\"children\":[";
					break;

				case NodeType::code: {
					CodeNode* code = static_cast<CodeNode*>(node);
					if (code->langCode)
						buffer += ",\"langCode\":\"" + static_cast<ValueNode*>(code->langCode)->value + "\"";
					buffer += ",\"string\":\"" + escapeBuffer(code->value) + "\"";
					buffer += "}";
					break;
				}

				case NodeType::urlLink:
					break;

				default:
					break;
			}
		}

		void Renderer::postTraverse(Gularen::Node* node) {
			switch (node->type) {
				case NodeType::paragraph:
				case NodeType::indent:
				case NodeType::container:
				case NodeType::boldFormat:
				case NodeType::italicFormat:
				case NodeType::monospaceFormat:
				case NodeType::root:
				case NodeType::title:
				case NodeType::part:
				case NodeType::chapter:
				case NodeType::section:
				case NodeType::subSection:
				case NodeType::subSubSection:
				case NodeType::miniSection:
				case NodeType::list:
				case NodeType::numericList:
				case NodeType::checkList:
				case NodeType::item:
				case NodeType::checkItem:
				case NodeType::table:
				case NodeType::row:
				case NodeType::cell:
					buffer += "]}";
					break;

				default:
					break;
			}
		}

		std::string Renderer::escapeBuffer(const std::string& buffer) {
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
