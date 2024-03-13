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
			for (unsigned int i = 0; i < document->children.size(); i += 1) {
				_compose(document->children.get(i), 0);

				if (_footnotes.size() != 0) {
					_content.append("<div class=\"footnote-desc\">\n");
					for (unsigned int i = 0; i < _footnotes.size(); i += 1) {
						const Footnote* footnote = _footnotes.get(i);

						_content.append("<p>");
						_content.append("<sup>");
						_content.append(String::fromInt(i + 1));
						_content.append("</sup> ");
						_content.append(footnote->description.pointer(), footnote->description.size());
						_content.append("</p>\n");
					}
					_content.append("</div>\n");

					_footnotes = Array<const Footnote*>();
				}
			}
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
					case Heading::Type::subtitle: return _content.append("<small>");
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

			case NodeKind::checkList: {
				return _content.append("<ul class=\"todo\">\n");
			}

			case NodeKind::definitionList: {
				return _content.append("<dl>\n");
			}

			case NodeKind::definitionTerm: {
				return _content.append("<dt>");
			}

			case NodeKind::definitionDesc: {
				return _content.append("<dd>");
			}

			case NodeKind::item: {
				return _content.append("<li>");
			}

			case NodeKind::checkItem: {
				switch (static_cast<const CheckItem*>(node)->state) {
					case CheckItem::State::unchecked: return _content.append("<li> <input type=\"checkbox\"> ");
					case CheckItem::State::checked: return _content.append("<li> <input type=\"checkbox\" checked> ");
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

				if (code->label.size() != 0) {
					_content.append("<code class=\"language-");
					_escapeAttribute(code->label);
					_content.append("\">");
					_escape(code->content);
					_content.append("</code>");
					return;
				}

				_content.append("<code>");
				_escape(code->content);
				_content.append("</code>");
				return;
			}

			case NodeKind::codeBlock: {
				const CodeBlock* code = static_cast<const CodeBlock*>(node);

				if (code->label.size() != 0) {
					_content.append("<pre><code class=\"language-");
					_escapeAttribute(code->label);
					_content.append("\">");
					_escape(code->content);
					_content.append("</code></pre>\n\n");
					return;
				}

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
						_escape(link->heading);
					}
				} else {
					_escape(link->label);
				}
				_content.append("</a>");
				return;
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
				return;
			}

			case NodeKind::footnote: {
				const Footnote* ref = static_cast<const Footnote*>(node);
				String id = String::fromInt(_footnotes.size() + 1);
				_content.append("<sup><a href=\"#Footnote-");
				_content.append(id);
				_content.append("\">");
				_content.append(id);
				_content.append("</a></sup>");
				_footnotes.append(ref);
				return;
			}

			case NodeKind::admon: {
				const Admon* ref = static_cast<const Admon*>(node);
				_content.append("<div class=\"admon ");
				_escapeClass(ref->label);
				_content.append("\" data-label=\"");
				_escapeAttribute(ref->label);
				_content.append("\">");

				if (ref->children.size() != 0) {
					_content.append("\n");
				}
				return;
			}

			case NodeKind::punct: {
				const Punct* punct = static_cast<const Punct*>(node);
				switch (punct->type) {
					case Punct::Type::hypen: return _content.append("&#8208;");
					case Punct::Type::enDash: return _content.append("&#8211;");
					case Punct::Type::emDash: return _content.append("&#8212;");
					case Punct::Type::quoteOpen: return _content.append("&#8223;");
					case Punct::Type::quoteClose: return _content.append("&#8221;");
					case Punct::Type::squoteOpen: return _content.append("&#8216;");
					case Punct::Type::squoteClose: return _content.append("&#8217;");
				}
			}

			case NodeKind::emoji: {
				const Emoji* emoji = static_cast<const Emoji*>(node);
				_content.append("<span class=\"emoji\">");
				_escape(emoji->code);
				_content.append("</span>");
				return;
			}

			case NodeKind::dateTime: {
				const DateTime* dateTime = static_cast<const DateTime*>(node);
				_content.append("<time datetime=\"");
				_escapeAttribute(dateTime->content);
				_content.append("\">");
				_escape(dateTime->content);
				_content.append("</time>");
				return;
			}

			case NodeKind::blockquote: {
				return _content.append("<blockquote class=\"quote\">\n");
			}

			case NodeKind::accountTag: {
				const AccountTag* accountTag = static_cast<const AccountTag*>(node);
				_content.append("<a class=\"account-tag\" href=\"");
				_escapeAttribute(accountTag->resource);
				_content.append("\">@");
				_escape(accountTag->resource);
				_content.append("</a>");
				return;
			}

			case NodeKind::hashTag: {
				const HashTag* hashTag = static_cast<const HashTag*>(node);
				_content.append("<a class=\"hash-tag\" href=\"");
				_escapeAttribute(hashTag->resource);
				_content.append("\">#");
				_escape(hashTag->resource);
				_content.append("</a>");
				return;
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
				}
			}

			case NodeKind::heading: {
				switch (static_cast<const Heading*>(node)->type) {
					case Heading::Type::chapter: return _content.append("</h1>\n");
					case Heading::Type::section: return _content.append("</h2>\n");
					case Heading::Type::subsection: return _content.append("</h3>\n");
					case Heading::Type::title: return _content.append(" ");
					case Heading::Type::subtitle: return _content.append("</small>");
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

			case NodeKind::checkList: {
				return _content.append("</ul>\n\n");
			}

			case NodeKind::definitionList: {
				return _content.append("</dl>\n\n");
			}

			case NodeKind::definitionTerm: {
				return _content.append("</dt>\n");
			}

			case NodeKind::definitionDesc: {
				return _content.append("</dd>\n");
			}

			case NodeKind::item: {
				return _content.append("</li>\n");
			}

			case NodeKind::checkItem: {
				switch (static_cast<const CheckItem*>(node)->state) {
					case CheckItem::State::unchecked: return _content.append("</li>\n");
					case CheckItem::State::checked: return _content.append("</li>\n");
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

			case NodeKind::admon: {
				return _content.append("</div>\n\n");
			}

			case NodeKind::blockquote: {
				return _content.append("</blockquote>\n\n");
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

	void _escapeClass(StringSlice content) {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			switch (content.get(i)) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					_content.append(content.get(i));
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
				case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
				case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
					_content.append(content.get(i) + ' ');
					break;

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

	Array<const Footnote*> _footnotes;
};

}
}
