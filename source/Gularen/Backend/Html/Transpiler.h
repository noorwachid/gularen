#pragma once

#include "Gularen/Frontend/Parser.h"
#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"

namespace Gularen {
namespace Html {

class Transpiler {
public:
	StringSlice transpile(Document* document) {
		_content = String();
		_tableAlignments = Slice<Table::Alignment>(nullptr, 0);
		_tableColumnIndex = 0;
		_tableLabel = false;

		if (document != nullptr) {
			_compose(document, 0);
		}

		return StringSlice(_content.pointer(), _content.size());
	}

private:
	void _compose(const Node* node, unsigned int depth) {
		_preCompose(node, depth);

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_compose(node->children.get(i), depth + 1);
		}

		_postCompose(node, depth);
	}

	void _preCompose(const Node* node, unsigned int depth) {
		switch (node->kind) {
			case NodeKind::text: {
				const Text* text = static_cast<const Text*>(node);
				return _content.append(text->content.pointer(), text->content.size());
			}

			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: return _content.append("<b>");
					case Style::Type::italic: return _content.append("<i>");
					case Style::Type::monospaced: return _content.append("<samp>");
				}
			}

			case NodeKind::heading: {
				const Heading* heading = static_cast<const Heading*>(node);
				switch (heading->type) {
					case Heading::Type::chapter:
						_content.append("<h1 id=\"");
						_escapeID(heading);
						_content.append("\">");
						return;
					case Heading::Type::section:
						_content.append("<h2 id=\"");
						_escapeID(heading);
						_content.append("\">");
						return;
					case Heading::Type::subsection:
						_content.append("<h3 id=\"");
						_escapeID(heading);
						_content.append("\">");
						return;

					case Heading::Type::title: return;
					case Heading::Type::subtitle: return _content.append("<span>");
				}
			}

			case NodeKind::paragraph: {
				return _content.append("<p>");
			}

			case NodeKind::space: {
				return _content.append("\n");
			}

			case NodeKind::lineBreak: {
				return _content.append("<br>");
			}

			case NodeKind::pageBreak: {
				return _content.append("<div class=\"page-break\"></div>\n\n");
			}

			case NodeKind::dinkus: {
				return _content.append("<hr>\n\n");
			}

			case NodeKind::indent: {
				return _content.append("<blockquote>\n");
			}

			case NodeKind::list: {
				return _content.append("<ul>\n");
			}

			case NodeKind::numberedList: {
				return _content.append("<ol>\n");
			}

			case NodeKind::todoList: {
				return _content.append("<ul class=\"todo\">\n");
			}

			case NodeKind::item: {
				return _content.append("<li>");
			}

			case NodeKind::todoItem: {
				switch (static_cast<const TodoItem*>(node)->state) {
					case TodoItem::State::todo: return _content.append("<li> <input type=\"checkbox\"> ");
					case TodoItem::State::done: return _content.append("<li> <input type=\"checkbox\" checked> ");
					case TodoItem::State::cancelled: return _content.append("<li> <input type=\"checkbox\" disabled> <del>");
				}
			}

			case NodeKind::table: {
				const Table* table = static_cast<const Table*>(node);
				_tableAlignments = Slice<Table::Alignment>(table->alignments.pointer(), table->alignments.size());
				return _content.append("<table>\n");
			}

			case NodeKind::row: {
				_tableColumnIndex = 0;

				switch (static_cast<const Row*>(node)->type) {
					case Row::Type::header: 
					case Row::Type::footer: 
						_tableLabel = true;
						break;

					case Row::Type::content: 
						_tableLabel = false;
						break;
				}

				return _content.append("<tr>\n");
			}

			case NodeKind::cell: {
				if (_tableColumnIndex < _tableAlignments.size()) {
					Table::Alignment alignment = _tableAlignments.get(_tableColumnIndex);
					_tableColumnIndex += 1;

					switch (alignment) {
						case Table::Alignment::left: return _content.append(_tableLabel ? "<th class=\"cell-left\">" : "<td class=\"cell-center\">");
						case Table::Alignment::center: return _content.append(_tableLabel ? "<th class=\"cell-center\">" : "<td class=\"cell-center\">");
						case Table::Alignment::right: return _content.append(_tableLabel ? "<th class=\"cell-right\">" : "<td class=\"cell-right\">");
					}
				}

				_tableColumnIndex += 1;
				return _content.append(_tableLabel ? "<th>" : "<td>");
			}

			case NodeKind::code: {
				const Code* code = static_cast<const Code*>(node);
				_content.append("<code>");
				_escape(code->content);
				_content.append("</code>");
				return;
			}

			case NodeKind::codeBlock: {
				const CodeBlock* code = static_cast<const CodeBlock*>(node);
				_content.append("<pre><code>");
				_escape(code->content);
				_content.append("</code></pre>\n\n");
				return;
			}

			case NodeKind::link: {
				const Link* link = static_cast<const Link*>(node);
				_content.append("<a href=\"");
				_escapeAttribute(link->resource);

				if (link->heading.size() != 0) {
					_content.append("#");
					_escapeID(link->heading);
				}

				_content.append("\">");

				if (link->label.size() == 0) {
					_escape(link->resource);

					if (link->heading.size() != 0) {
						_content.append(" ");
						_escapeID(link->heading);
					}
				} else {
					_escape(link->label);
				}
				_content.append("</a>");
			}

			case NodeKind::view: {
				const View* view = static_cast<const View*>(node);

				for (unsigned int i = view->resource.size(); i > 0; i -= 1) {
					if (view->resource.get(i - 1) == '.') {
						StringSlice extension(view->resource.pointer() + i, view->resource.size() - i);
						if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "gif") {
							if (view->label.size() != 0) {
								_content.append("<figure>");
								_content.append("<img src=\"");
								_escapeAttribute(view->resource);
								_content.append("\">");
								_content.append("<figcaption>");
								_escape(view->label);
								_content.append("</figcaption>");
								_content.append("</figure>");
							}

							_content.append("<img src=\"");
							_escapeAttribute(view->resource);
							_content.append("\">");
							return;
						}
						break;
					}
				}

				_content.append("<a href=\"");
				_escapeAttribute(view->resource);
				_content.append("\">");

				if (view->label.size() == 0) {
					_escape(view->resource);
				} else {
					_escape(view->label);
				}
				_content.append("</a>");
			}

			case NodeKind::include: {
				const Include* include = static_cast<const Include*>(node);
				_content.append("<a href=\"");
				_escapeAttribute(include->resource);
				_content.append("\">");
				_escape(include->resource);
				_content.append("</a>");
			}

			default: break;
		}
	}

	void _postCompose(const Node* node, unsigned int depth) {
		switch (node->kind) {
			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: return _content.append("</b>");
					case Style::Type::italic: return _content.append("</i>");
					case Style::Type::monospaced: return _content.append("</samp>");
				}
			}

			case NodeKind::heading: {
				switch (static_cast<const Heading*>(node)->type) {
					case Heading::Type::chapter: return _content.append("</h1>\n");
					case Heading::Type::section: return _content.append("</h2>\n");
					case Heading::Type::subsection: return _content.append("</h3>\n");
					case Heading::Type::title: return _content.append(" ");
					case Heading::Type::subtitle: return _content.append("</span>");
				}
			}

			case NodeKind::paragraph: {
				return _content.append("</p>\n\n");
			}

			case NodeKind::indent: {
				return _content.append("</blockquote>\n");
			}

			case NodeKind::list: {
				return _content.append("</ul>\n\n");
			}

			case NodeKind::numberedList: {
				return _content.append("</ol>\n\n");
			}

			case NodeKind::todoList: {
				return _content.append("</ul>\n\n");
			}

			case NodeKind::item: {
				return _content.append("</li>\n");
			}

			case NodeKind::todoItem: {
				switch (static_cast<const TodoItem*>(node)->state) {
					case TodoItem::State::todo: return _content.append("</li>\n");
					case TodoItem::State::done: return _content.append("</li>\n");
					case TodoItem::State::cancelled: return _content.append("</del></li>\n");
				}
			}

			case NodeKind::table: {
				return _content.append("</table>\n\n");
			}

			case NodeKind::row: {
				return _content.append("</tr>\n");
			}

			case NodeKind::cell: {
				return _content.append(_tableLabel ? "</th>\n" : "</td>\n");
			}

			default: break;
		}
	}

	void _escape(StringSlice content) {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			switch (content.get(i)) {
				case '<': _content.append("&lt;"); break;
				case '>': _content.append("&gt;"); break;
				case '&': _content.append("&amp;"); break;
				case '\"': _content.append("&quot;"); break;
				case '\'': _content.append("&#39;"); break;
				default: _content.append(content.get(i)); break;
			}
		}
	}

	void _escapeAttribute(StringSlice content) {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			switch (content.get(i)) {
				case '\"': _content.append("&quot;"); break;
				case '\'': _content.append("&#39;"); break;
				default: _content.append(content.get(i)); break;
			}
		}
	}

	void _escapeID(StringSlice content) {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			switch (content.get(i)) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
				case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
				case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':

				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
				case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
				case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
					_content.append(content.get(i));
					break;

				case '-':
				case ' ': 
					_content.append('-'); 
					break;

				default: 
					break;
			}
		}
	}

	void _escapeID(const Node* node) {
		if (node->kind == NodeKind::text) {
			_escapeID(static_cast<const Text*>(node)->content);
		}

		if (node->kind == NodeKind::heading && static_cast<const Heading*>(node)->type == Heading::Type::subtitle) {
			return;
		}

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_escapeID(node->children.get(i));
		}
	}

private:
	String _content;

	Slice<Table::Alignment> _tableAlignments;

	unsigned int _tableColumnIndex;

	bool _tableLabel;
};

}
}
