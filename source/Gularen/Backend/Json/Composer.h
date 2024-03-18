#pragma once

#include "Gularen/Frontend/Parser.h"
#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"
#include "Gularen/Library/HashTable.h"

namespace Gularen {
namespace Json {

class Composer {
public:
	StringSlice compose(Document* document) {
		_content = String("[");

		if (document != nullptr) {
			for (unsigned int i = 0; i < document->children.size(); i += 1) {
				if (i != 0) {
					_content.append(",");
				}
				_compose(document->children.get(i));
			}
		}

		_content.append("]");

		return StringSlice(_content.pointer(), _content.size());
	}

private:
	void _compose(const Node* node) {
		_content.append("{");
		switch (node->kind) {
			case NodeKind::comment: {
				_content.append("\"kind\":\"comment\",\"content\":\"");
				_escape(static_cast<const Comment*>(node)->content);
				_content.append("\"");
				break;
			}
			case NodeKind::paragraph: {
				_content.append("\"kind\":\"paragraph\"");
				break;
			}
			case NodeKind::text: {
				_content.append("\"kind\":\"text\",\"content\":\"");
				_escape(static_cast<const Text*>(node)->content);
				_content.append("\"");
				break;
			}
			case NodeKind::space: {
				_content.append("\"kind\":\"space\"");
				break;
			}
			case NodeKind::punct: {
				_content.append("\"kind\":\"punct\",\"type\":\"");
				switch (static_cast<const Punct*>(node)->type) {
					case Punct::Type::hypen:
						_content.append("hyphen");
						break;
					case Punct::Type::enDash:
						_content.append("enDash");
						break;
					case Punct::Type::emDash:
						_content.append("emDash");
						break;
					case Punct::Type::quoteOpen:
						_content.append("quoteOpen");
						break;
					case Punct::Type::quoteClose:
						_content.append("quoteClose");
						break;
					case Punct::Type::squoteOpen:
						_content.append("squoteOpen");
						break;
					case Punct::Type::squoteClose:
						_content.append("squoteClose");
						break;
				}
				_content.append("\"");
				break;
			}
			case NodeKind::style: {
				_content.append("\"kind\":\"style\",\"type\":\"");
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold:
						_content.append("bold");
						break;
					case Style::Type::italic:
						_content.append("italic");
						break;
				}
				_content.append("\"");
				break;
			}
			case NodeKind::highlight: {
				_content.append("\"kind\":\"highlight\"");
				break;
			}
			case NodeKind::lineBreak: {
				_content.append("\"kind\":\"lineBreak\"");
				break;
			}
			case NodeKind::pageBreak: {
				_content.append("\"kind\":\"pageBreak\"");
				break;
			}
			case NodeKind::dinkus: {
				_content.append("\"kind\":\"dinkus\"");
				break;
			}
			case NodeKind::indent: {
				_content.append("\"kind\":\"indent\"");
				break;
			}
			case NodeKind::blockquote: {
				_content.append("\"kind\":\"blockquote\"");
				break;
			}
			case NodeKind::heading: {
				_content.append("\"kind\":\"heading\"");
				break;
			}
			default: {
				_content.append("\"kind\":\"unknown\"");
				break;
			}
		}

		if (node->children.size() != 0) {
			_content.append(",\"children\":[");

			for (unsigned int i = 0; i < node->children.size(); i += 1) {
				if (i != 0) {
					_content.append(",");
				}
				_compose(node->children.get(i));
			}

			_content.append("]");
		}

		_content.append("}");
	}

	void _escape(StringSlice content) {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			char byte = content.get(i);
			if (byte == '"' || byte == '\\' || ('\x00' <= byte && byte <= '\x1f')) {
				char bytes[4];
				snprintf(bytes, 4, "%04x", static_cast<short>(byte));
				_content.append("\\u");
				_content.append(bytes, 4);
			} else {
				_content.append(byte);
			}
		}
	}

private:
	String _content;
};

}
}


