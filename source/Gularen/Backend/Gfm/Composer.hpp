#pragma once

#include "Gularen/Frontend/Parser.hpp"

namespace Gularen {
namespace Gfm {

class Composer {
public:
	std::string_view compose(Document* document) {
		_content = std::string();
		_listItem = false;
		_listCount = 0;
		_indent = 0;

		if (document != nullptr) {
			for (size_t i = 0; i < document->children.size(); i += 1) {
				_composeBlock(document->children[i]);
			}
		}

		return std::string_view(_content.data(), _content.size());
	}

private:
	void _composeBlock(const Node* node) {
		switch (node->kind) {
			case NodeKind::paragraph: return _composeParagraph(node);
			case NodeKind::document:
				for (size_t i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children[i]);
				}
				break;
			case NodeKind::section: {
				switch (static_cast<const Section*>(node)->type) {
					case Section::Type::section: _content.append("# "); break;
					case Section::Type::subsection: _content.append("## "); break;
					case Section::Type::subsubsection: _content.append("### "); break;
					default: break;
				}
				for (size_t i = 0; i < node->children[0]->children.size(); i += 1) {
					_composeInline(node->children[0]->children[i]);
				}
				_content.append("\n");
				for (size_t i = 1; i < node->children.size(); i += 1) {
					_composeBlock(node->children[i]);
				}
				break;
			}
			case NodeKind::codeBlock: {
				const CodeBlock* block = static_cast<const CodeBlock*>(node);
				_content.append("```");
				_content.append(block->label.data(), block->label.size());
				_content.append("\n");
				_content.append(block->content.data(), block->content.size());
				_content.append("\n```\n\n");
				break;
			}
			case NodeKind::dinkus: {
				_content.append("***\n");
				break;
			}
			case NodeKind::list:
			case NodeKind::numberedList:
			case NodeKind::checkList: {
				bool prevListItem = _listItem;
				size_t prevListCount = _listCount;
				_listItem = false;
				_listCount = node->kind == NodeKind::numberedList ? 1 : 0;
				for (size_t i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children[i]);
				}
				if (!prevListItem) {
					_content.append("\n");
				}
				_listItem = prevListItem;
				_listCount = prevListCount;
				break;
			}
			case NodeKind::item: {
				_composePrefix();
				_listItem = true;
				if (_listCount == 0) {
					_content.append("- ");
				} else {
					std::string count = std::to_string(_listCount);
					_content.append(count.data(), count.size());
					_content.append(". ");
					_listCount += 1;
				}
				for (size_t i = 0; i < node->children.size(); i += 1) {
					if (node->children[i]->kind == NodeKind::list ||
						node->children[i]->kind == NodeKind::numberedList ||
						node->children[i]->kind == NodeKind::checkList) {
						_indent += 1;
						_content.append("\n");
						for (size_t i = 0; i < node->children.size(); i += 1) {
							_composeBlock(node->children[i]);
						}
						_indent -= 1;
						continue;
					}
					_composeInline(node->children[i]);
				}
				_content.append("\n");
				break;
			}
			case NodeKind::checkItem: {
				_composePrefix();
				_listItem = true;
				_content.append("- [");
				_content.append(static_cast<const CheckItem*>(node)->checked ? " " : "x");
				_content.append("] ");
				for (size_t i = 0; i < node->children.size(); i += 1) {
					if (node->children[i]->kind == NodeKind::list) {
						_indent += 1;
						_content.append("\n");
						for (size_t i = 0; i < node->children.size(); i += 1) {
							_composeBlock(node->children[i]);
						}
						_indent -= 1;
						continue;
					}
					_composeInline(node->children[i]);
				}
				_content.append("\n");
				break;
			}
			case NodeKind::quote: {
				_indent += 1;
				_content.append("\n");
				for (size_t i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children[i]);
				}
				_indent -= 1;
				break;
			}
			default: {
				break;
			}
		}
	}

	void _composePrefix() {
		for (size_t i = 0; i < _indent; i += 1) {
			_content.append("\t");
		}
	}

	void _composeParagraph(const Node* node) {
		size_t i = 0;
		while (i < node->children.size()) {
			_composePrefix();

			while (i < node->children.size()) {
				if (node->children[i]->kind == NodeKind::space) {
					_content.append("\n");
					break;
				}
				_composeInline(node->children[i]);
				i += 1;
			}
			i += 1;
		}
		_content.append("\n\n");
	}

	void _composeInline(const Node* node) {
		switch (node->kind) {
			case NodeKind::text: {
				std::string_view content = static_cast<const Text*>(node)->content;
				_content.append(content.data(), content.size());
				break;
			}
			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: {
						_content.append("**");
						for (size_t i = 0; i < node->children.size(); i += 1) {
							_composeInline(node->children[i]);
						}
						_content.append("**");
						break;
					}
					case Style::Type::italic: {
						_content.append("_");
						for (size_t i = 0; i < node->children.size(); i += 1) {
							_composeInline(node->children[i]);
						}
						_content.append("_");
						break;
					}
				}
				break;
			}
			case NodeKind::lineBreak: {
				_content.append("  \n");
				break;
			}
			case NodeKind::punct: {
				switch (static_cast<const Punct*>(node)->type) {
					case Punct::Type::quoteOpen: _content.append("“"); break;
					case Punct::Type::quoteClose: _content.append("”"); break;
					case Punct::Type::squoteOpen: _content.append("‘"); break;
					case Punct::Type::squoteClose: _content.append("’"); break;
					case Punct::Type::hypen: _content.append("‐"); break;
					case Punct::Type::enDash: _content.append("–"); break;
					case Punct::Type::emDash: _content.append("—"); break;
				}
				break;
			}
			case NodeKind::quote: {
				_indent += 1;
				_content.append("\n");
				for (size_t i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children[i]);
				}
				_indent -= 1;
				break;
			}
			case NodeKind::accountTag: {
				std::string_view res = static_cast<const AccountTag*>(node)->resource;
				_content.append("@");
				_content.append(res.data(), res.size());
				break;
			}
			case NodeKind::hashTag: {
				std::string_view res = static_cast<const HashTag*>(node)->resource;
				_content.append("#");
				_content.append(res.data(), res.size());
				break;
			}
			case NodeKind::code: {
				_content.append("`");
				std::string_view content = static_cast<const Code*>(node)->content;
				_content.append(content.data(), content.size());
				_content.append("`");
				break;
			}
			case NodeKind::link: {
				const Link* link = static_cast<const Link*>(node);
				_content.append("[");
				if (link->label.size() == 0) {
					_content.append(link->resource.data(), link->resource.size());
				} else {
					_content.append(link->label.data(), link->label.size());
				}
				_content.append("](");
				_content.append(link->resource.data(), link->resource.size());
				_content.append(")");
				break;
			}
			case NodeKind::view: {
				const View* view = static_cast<const View*>(node);
				_content.append("![");
				if (view->label.size() == 0) {
					_content.append(view->resource.data(), view->resource.size());
				} else {
					_content.append(view->label.data(), view->label.size());
				}
				_content.append("](");
				_content.append(view->resource.data(), view->resource.size());
				_content.append(")");
				break;
			}
			case NodeKind::emoji: {
				const Emoji* emoji = static_cast<const Emoji*>(node);

				_content.append(":");
				for (size_t i = 0; i < emoji->code.size(); i += 1) {
					if (emoji->code[i] == '-') {
						_content.append("_");
					} else {
						_content.append(1, emoji->code[i]);
					}
				}
				_content.append(":");
			}
			case NodeKind::subtitle:
				_content.append(": ");
				for (size_t i = 0; i < node->children.size(); i += 1) {
					_composeInline(node->children[i]);
				}
				break;
			default: 
				break;
		}
	}

private:
	std::string _content;
	bool _listItem;
	size_t _listCount;
	size_t _indent;
};

}
}

