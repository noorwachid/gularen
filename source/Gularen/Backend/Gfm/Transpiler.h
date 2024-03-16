#pragma once

#include "Gularen/Frontend/Parser.h"
#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"
#include "Gularen/Library/HashTable.h"

namespace Gularen {
namespace Gfm {

class Transpiler {
public:
	StringSlice transpile(Document* document) {
		_content = String();
		_listItem = false;
		_listCount = 0;
		_blockQuote = 0;
		_indent = 0;

		if (document != nullptr) {
			for (unsigned int i = 0; i < document->children.size(); i += 1) {
				_composeBlock(document->children.get(i));
			}
		}

		return StringSlice(_content.pointer(), _content.size());
	}

private:
	void _composeBlock(const Node* node) {
		switch (node->kind) {
			case NodeKind::paragraph: return _composeParagraph(node);
			case NodeKind::blockquote:
				_blockQuote += 1;
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children.get(i));
				}
				_blockQuote -= 1;
				break;
			case NodeKind::document:
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children.get(i));
				}
				break;
			case NodeKind::heading: {
				switch (static_cast<const Heading*>(node)->type) {
					case Heading::Type::chapter: _content.append("# "); break;
					case Heading::Type::section: _content.append("## "); break;
					case Heading::Type::subsection: _content.append("### "); break;
					default: break;
				}
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					if (node->children.get(i)->kind == NodeKind::heading &&
						static_cast<const Heading*>(node->children.get(i))->type == Heading::Type::subtitle) {
						_content.append(": ");
					}
					_composeInline(node->children.get(i));
				}
				_content.append("\n");
				break;
			}
			case NodeKind::codeBlock: {
				const CodeBlock* block = static_cast<const CodeBlock*>(node);
				_content.append("```");
				_content.append(block->label.pointer(), block->label.size());
				_content.append("\n");
				_content.append(block->content.pointer(), block->content.size());
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
				unsigned int prevListCount = _listCount;
				_listItem = false;
				_listCount = node->kind == NodeKind::numberedList ? 1 : 0;
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children.get(i));
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
					String count = String::fromInt(_listCount);
					_content.append(count.pointer(), count.size());
					_content.append(". ");
					_listCount += 1;
				}
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					if (node->children.get(i)->kind == NodeKind::list ||
						node->children.get(i)->kind == NodeKind::numberedList ||
						node->children.get(i)->kind == NodeKind::checkList) {
						_indent += 1;
						_content.append("\n");
						for (unsigned int i = 0; i < node->children.size(); i += 1) {
							_composeBlock(node->children.get(i));
						}
						_indent -= 1;
						continue;
					}
					_composeInline(node->children.get(i));
				}
				_content.append("\n");
				break;
			}
			case NodeKind::checkItem: {
				_composePrefix();
				_listItem = true;
				_content.append("- [");
				_content.append(static_cast<const CheckItem*>(node)->state == CheckItem::State::unchecked ? " " : "x");
				_content.append("] ");
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					if (node->children.get(i)->kind == NodeKind::list) {
						_indent += 1;
						_content.append("\n");
						for (unsigned int i = 0; i < node->children.size(); i += 1) {
							_composeBlock(node->children.get(i));
						}
						_indent -= 1;
						continue;
					}
					_composeInline(node->children.get(i));
				}
				_content.append("\n");
				break;
			}
			case NodeKind::indent: {
				_indent += 1;
				_content.append("\n");
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children.get(i));
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
		for (unsigned int i = 0; i < _blockQuote; i += 1) {
			_content.append("> ");
		}
		for (unsigned int i = 0; i < _indent; i += 1) {
			_content.append("\t");
		}
	}

	void _composeParagraph(const Node* node) {
		unsigned int i = 0;
		while (i < node->children.size()) {
			_composePrefix();

			while (i < node->children.size()) {
				if (node->children.get(i)->kind == NodeKind::space) {
					_content.append("\n");
					break;
				}
				_composeInline(node->children.get(i));
				i += 1;
			}
			i += 1;
		}
		_content.append("\n\n");
	}

	void _composeInline(const Node* node) {
		switch (node->kind) {
			case NodeKind::text: {
				StringSlice content = static_cast<const Text*>(node)->content;
				_content.append(content.pointer(), content.size());
				break;
			}
			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: {
						_content.append("**");
						for (unsigned int i = 0; i < node->children.size(); i += 1) {
							_composeInline(node->children.get(i));
						}
						_content.append("**");
						break;
					}
					case Style::Type::italic: {
						_content.append("_");
						for (unsigned int i = 0; i < node->children.size(); i += 1) {
							_composeInline(node->children.get(i));
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
			}
			case NodeKind::indent: {
				_indent += 1;
				_content.append("\n");
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeBlock(node->children.get(i));
				}
				_indent -= 1;
				break;
			}
			case NodeKind::accountTag: {
				StringSlice res = static_cast<const AccountTag*>(node)->resource;
				_content.append("@");
				_content.append(res.pointer(), res.size());
				break;
			}
			case NodeKind::hashTag: {
				StringSlice res = static_cast<const HashTag*>(node)->resource;
				_content.append("#");
				_content.append(res.pointer(), res.size());
				break;
			}
			case NodeKind::code: {
				_content.append("`");
				StringSlice content = static_cast<const Code*>(node)->content;
				_content.append(content.pointer(), content.size());
				_content.append("`");
			}
			default: 
				break;
		}
	}

private:
	String _content;
	bool _listItem;
	unsigned int _listCount;
	unsigned int _blockQuote;
	unsigned int _indent;
};

}
}

