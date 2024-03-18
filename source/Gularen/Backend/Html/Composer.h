#pragma once

#include "Gularen/Frontend/Parser.h"
#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"
#include "Gularen/Library/HashTable.h"

namespace Gularen {
namespace Html {

class Composer {
public:
	StringSlice compose(Document* document) {
		_content = String();
		_tableAlignments = Slice<Table::Alignment>(nullptr, 0);
		_tableColumnIndex = 0;
		_tableLabel = false;

		if (document != nullptr) {
			_collectReferences(document);

			for (unsigned int i = 0; i < document->children.size(); i += 1) {
				_compose(document->children.get(i), _content);
			}
		}

		_composeFootnote();

		return StringSlice(_content.pointer(), _content.size());
	}

	StringSlice composeToc(Document* document) {
		_toc.append("<ul class=\"toc\">\n");
		_composeToc(document);
		_toc.append("</ul>\n");
		return StringSlice(_toc.pointer(), _toc.size());
	}

private:
	void _composeToc(const Node* node) {
		if (node->kind == NodeKind::heading) {
			auto heading = static_cast<const Heading*>(node);

			if (heading->type == Heading::Type::section ||
				heading->type == Heading::Type::subsection ||
				heading->type == Heading::Type::subsubsection) {

				String content;

				for (unsigned int i = 0; i < heading->children.size(); i += 1) {
					if (heading->children.get(i)->kind == NodeKind::subtitle) {
						break;
					}
					_compose(heading->children.get(i), content);
				}

				if (content.size() != 0) {
					while (content.get(content.size() - 1) == ' ') {
						content.drop();
					}
				}

				_toc.append("<li class=\"");

				switch (heading->type) {
					case Heading::Type::section:
						_toc.append("section");
						break;
					case Heading::Type::subsection:
						_toc.append("subsection");
						break;
					case Heading::Type::subsubsection:
						_toc.append("subsubsection");
						break;
					default: break;
				}

				String id;
				_toc.append("\"><a href=\"#");
				_escapeID(StringSlice(content.pointer(), content.size()), id);
				_toc.append(id);
				_toc.append("\">");
				_toc.append(content);
				_toc.append("</a></li>\n");
			}
		}

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_composeToc(node->children.get(i));
		}
	}

	void _composeFootnote() {
		if (_footnotes.size() != 0) {
			_content.append("<div class=\"footnote-desc\">\n");
			for (unsigned int i = 0; i < _footnotes.size(); i += 1) {
				const Footnote* footnote = _footnotes.get(i);

				_content.append("<p>");
				_content.append("<sup>");
				_content.append(String::fromInt(i + 1));
				_content.append("</sup> ");
				_content.append(footnote->desc.pointer(), footnote->desc.size());
				_content.append("</p>\n");
			}
			_content.append("</div>\n");

			_footnotes = Array<const Footnote*>();
		}
	}
	void _collectReferences(const Node* node) {
		if (node->kind == NodeKind::reference) {
			const Reference* ref = static_cast<const Reference*>(node);
			HashTable<const ReferenceInfo*> refTable;

			for (unsigned int i = 0; i < ref->children.size(); i += 1) {
				const ReferenceInfo* info = static_cast<const ReferenceInfo*>(ref->children.get(i));
				refTable.set(info->key, info);
			}

			_references.set(ref->id, refTable);
			return;
		}

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_collectReferences(node->children.get(i));
		}
	}

	void _compose(const Node* node, String& content) {
		_preCompose(node, content);

		if (node->kind == NodeKind::reference) {
			return;
		}

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_compose(node->children.get(i), content);
		}

		_postCompose(node, content);
	}

	void _preCompose(const Node* node, String& content) {
		switch (node->kind) {
			case NodeKind::text: {
				const Text* text = static_cast<const Text*>(node);
				return content.append(text->content.pointer(), text->content.size());
			}

			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: return content.append("<b>");
					case Style::Type::italic: return content.append("<i>");
				}
			}

			case NodeKind::highlight: {
				return content.append("<mark>");
			}

			case NodeKind::heading: {
				_composeFootnote();

				const Heading* heading = static_cast<const Heading*>(node);
				switch (heading->type) {
					case Heading::Type::section:
						content.append("<h1 id=\"");
						_escapeID(heading, content);
						content.append("\">");
						return;
					case Heading::Type::subsection:
						content.append("<h2 id=\"");
						_escapeID(heading, content);
						content.append("\">");
						return;
					case Heading::Type::subsubsection:
						content.append("<h3 id=\"");
						_escapeID(heading, content);
						content.append("\">");
						return;
				}
			}

			case NodeKind::subtitle: {
				return content.append("<small>");
			}

			case NodeKind::paragraph: {
				return content.append("<p>");
			}

			case NodeKind::space: {
				return content.append("\n");
			}

			case NodeKind::lineBreak: {
				return content.append("<br>");
			}

			case NodeKind::pageBreak: {
				_composeFootnote();
				return content.append("<div class=\"page-break\"></div>\n\n");
			}

			case NodeKind::dinkus: {
				return content.append("<hr>\n\n");
			}

			case NodeKind::indent: {
				return content.append("<blockquote>\n");
			}

			case NodeKind::list: {
				return content.append("<ul>\n");
			}

			case NodeKind::numberedList: {
				return content.append("<ol>\n");
			}

			case NodeKind::checkList: {
				return content.append("<ul class=\"check-list\">\n");
			}

			case NodeKind::definitionList: {
				return content.append("<dl>\n");
			}

			case NodeKind::definitionTerm: {
				return content.append("<dt>");
			}

			case NodeKind::definitionDesc: {
				return content.append("<dd>");
			}

			case NodeKind::item: {
				return content.append("<li>");
			}

			case NodeKind::checkItem: {
				switch (static_cast<const CheckItem*>(node)->state) {
					case CheckItem::State::unchecked: return content.append("<li> <input type=\"checkbox\"> ");
					case CheckItem::State::checked: return content.append("<li> <input type=\"checkbox\" checked> ");
				}
			}

			case NodeKind::table: {
				const Table* table = static_cast<const Table*>(node);
				_tableAlignments = Slice<Table::Alignment>(table->alignments.pointer(), table->alignments.size());
				return content.append("<table>\n");
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

				return content.append("<tr>\n");
			}

			case NodeKind::cell: {
				if (_tableColumnIndex < _tableAlignments.size()) {
					Table::Alignment alignment = _tableAlignments.get(_tableColumnIndex);
					_tableColumnIndex += 1;

					switch (alignment) {
						case Table::Alignment::left: return content.append(_tableLabel ? "<th class=\"cell-left\">" : "<td class=\"cell-center\">");
						case Table::Alignment::center: return content.append(_tableLabel ? "<th class=\"cell-center\">" : "<td class=\"cell-center\">");
						case Table::Alignment::right: return content.append(_tableLabel ? "<th class=\"cell-right\">" : "<td class=\"cell-right\">");
					}
				}

				_tableColumnIndex += 1;
				return content.append(_tableLabel ? "<th>" : "<td>");
			}

			case NodeKind::code: {
				const Code* code = static_cast<const Code*>(node);

				if (code->label.size() != 0) {
					content.append("<code class=\"language-");
					_escapeAttribute(code->label, content);
					content.append("\">");
					_escape(code->content, content);
					content.append("</code>");
					return;
				}

				content.append("<code>");
				_escape(code->content, content);
				content.append("</code>");
				return;
			}

			case NodeKind::codeBlock: {
				const CodeBlock* code = static_cast<const CodeBlock*>(node);

				if (code->label.size() != 0) {
					content.append("<pre><code class=\"language-");
					_escapeAttribute(code->label, content);
					content.append("\">");
					_escape(code->content, content);
					content.append("</code></pre>\n\n");
					return;
				}

				content.append("<pre><code>");
				_escape(code->content, content);
				content.append("</code></pre>\n\n");
				return;
			}

			case NodeKind::link: {
				const Link* link = static_cast<const Link*>(node);
				content.append("<a href=\"");
				_escapeAttribute(link->resource, content);

				if (link->heading.size() != 0) {
					content.append("#");
					_escapeID(link->heading, content);
				}

				content.append("\">");

				if (link->label.size() == 0) {
					_escape(link->resource, content);

					if (link->heading.size() != 0) {
						content.append(" ");
						_escape(link->heading, content);
					}
				} else {
					_escape(link->label, content);
				}
				content.append("</a>");
				return;
			}

			case NodeKind::view: {
				const View* view = static_cast<const View*>(node);

				for (unsigned int i = view->resource.size(); i > 0; i -= 1) {
					if (view->resource.get(i - 1) == '.') {
						StringSlice extension(view->resource.pointer() + i, view->resource.size() - i);
						if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "gif") {
							if (view->label.size() != 0) {
								content.append("<figure>");
								content.append("<img src=\"");
								_escapeAttribute(view->resource, content);
								content.append("\">");
								content.append("<figcaption>");
								_escape(view->label, content);
								content.append("</figcaption>");
								content.append("</figure>");
								return;
							}

							content.append("<img src=\"");
							_escapeAttribute(view->resource, content);
							content.append("\">");
							return;
						}
						break;
					}
				}

				content.append("<a href=\"");
				_escapeAttribute(view->resource, content);
				content.append("\">");

				if (view->label.size() == 0) {
					_escape(view->resource, content);
				} else {
					_escape(view->label, content);
				}
				content.append("</a>");
				return;
			}

			case NodeKind::footnote: {
				const Footnote* ref = static_cast<const Footnote*>(node);
				String id = String::fromInt(_footnotes.size() + 1);
				content.append("<sup><a href=\"#Footnote-");
				content.append(id);
				content.append("\">");
				content.append(id);
				content.append("</a></sup>");
				_footnotes.append(ref);
				return;
			}

			case NodeKind::inText: {
				const InText* inText = static_cast<const InText*>(node);
				content.append("<a class=\"in-text\" href=\"#Reference-");
				_escapeAttribute(inText->id, content);
				content.append("\">");
				if (inText->label.size() != 0) {
					_escape(inText->label, content);
				} else {
					_cite(inText->id, content);
				}
				content.append("</a>");
				return;
			}

			case NodeKind::reference: {
				const Reference* ref = static_cast<const Reference*>(node);
				content.append("<div class=\"reference\" id=\"Reference-");
				_escapeAttribute(ref->id, content);
				content.append("\">");
				_refer(ref->id, content);
				content.append("</div>\n");
				return;
			}

			case NodeKind::admon: {
				const Admon* ref = static_cast<const Admon*>(node);
				content.append("<div class=\"admon ");
				_escapeClass(ref->label, content);
				content.append("\">\n");
				content.append("<div class=\"label\">");
				_escape(ref->label, content);
				content.append("</div>\n");
				content.append("<div class=\"content\">");

				if (ref->children.size() != 0) {
					content.append("\n");
				}
				return;
			}

			case NodeKind::punct: {
				const Punct* punct = static_cast<const Punct*>(node);
				switch (punct->type) {
					case Punct::Type::hypen: return content.append("&#8208;");
					case Punct::Type::enDash: return content.append("&#8211;");
					case Punct::Type::emDash: return content.append("&#8212;");
					case Punct::Type::quoteOpen: return content.append("&#8223;");
					case Punct::Type::quoteClose: return content.append("&#8221;");
					case Punct::Type::squoteOpen: return content.append("&#8216;");
					case Punct::Type::squoteClose: return content.append("&#8217;");
				}
			}

			case NodeKind::emoji: {
				const Emoji* emoji = static_cast<const Emoji*>(node);
				content.append("<span class=\"emoji\">");
				_escape(emoji->code, content);
				content.append("</span>");
				return;
			}

			case NodeKind::dateTime: {
				const DateTime* dateTime = static_cast<const DateTime*>(node);
				content.append("<time datetime=\"");
				_escapeAttribute(dateTime->content, content);
				content.append("\">");
				_escape(dateTime->content, content);
				content.append("</time>");
				return;
			}

			case NodeKind::blockquote: {
				return content.append("<blockquote class=\"quote\">\n");
			}

			case NodeKind::accountTag: {
				const AccountTag* accountTag = static_cast<const AccountTag*>(node);
				content.append("<a class=\"account-tag\" href=\"");
				_escapeAttribute(accountTag->resource, content);
				content.append("\">@");
				_escape(accountTag->resource, content);
				content.append("</a>");
				return;
			}

			case NodeKind::hashTag: {
				const HashTag* hashTag = static_cast<const HashTag*>(node);
				content.append("<a class=\"hash-tag\" href=\"");
				_escapeAttribute(hashTag->resource, content);
				content.append("\">#");
				_escape(hashTag->resource, content);
				content.append("</a>");
				return;
			}

			default: break;
		}
	}

	void _postCompose(const Node* node, String& content) {
		switch (node->kind) {
			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: return content.append("</b>");
					case Style::Type::italic: return content.append("</i>");
				}
			}

			case NodeKind::highlight: {
				return content.append("</mark>");
			}

			case NodeKind::heading: {
				switch (static_cast<const Heading*>(node)->type) {
					case Heading::Type::section: return content.append("</h1>\n");
					case Heading::Type::subsection: return content.append("</h2>\n");
					case Heading::Type::subsubsection: return content.append("</h3>\n");
				}
			}

			case NodeKind::subtitle: {
				return content.append("</small>");
			}

			case NodeKind::paragraph: {
				return content.append("</p>\n\n");
			}

			case NodeKind::indent: {
				return content.append("</blockquote>\n");
			}

			case NodeKind::list: {
				return content.append("</ul>\n\n");
			}

			case NodeKind::numberedList: {
				return content.append("</ol>\n\n");
			}

			case NodeKind::checkList: {
				return content.append("</ul>\n\n");
			}

			case NodeKind::definitionList: {
				return content.append("</dl>\n\n");
			}

			case NodeKind::definitionTerm: {
				return content.append("</dt>\n");
			}

			case NodeKind::definitionDesc: {
				return content.append("</dd>\n");
			}

			case NodeKind::item: {
				return content.append("</li>\n");
			}

			case NodeKind::checkItem: {
				switch (static_cast<const CheckItem*>(node)->state) {
					case CheckItem::State::unchecked: return content.append("</li>\n");
					case CheckItem::State::checked: return content.append("</li>\n");
				}
			}

			case NodeKind::table: {
				return content.append("</table>\n\n");
			}

			case NodeKind::row: {
				return content.append("</tr>\n");
			}

			case NodeKind::cell: {
				return content.append(_tableLabel ? "</th>\n" : "</td>\n");
			}

			case NodeKind::admon: {
				return content.append("\n</div>\n</div>\n\n");
			}

			case NodeKind::blockquote: {
				return content.append("</blockquote>\n\n");
			}

			default: break;
		}
	}

	void _escape(StringSlice in, String& content) {
		for (unsigned int i = 0; i < in.size(); i += 1) {
			switch (in.get(i)) {
				case '<': content.append("&lt;"); break;
				case '>': content.append("&gt;"); break;
				case '&': content.append("&amp;"); break;
				case '\"': content.append("&quot;"); break;
				case '\'': content.append("&#39;"); break;
				default: content.append(in.get(i)); break;
			}
		}
	}

	void _escapeAttribute(StringSlice in, String& content) {
		for (unsigned int i = 0; i < in.size(); i += 1) {
			switch (in.get(i)) {
				case '\"': content.append("&quot;"); break;
				case '\'': content.append("&#39;"); break;
				default: content.append(in.get(i)); break;
			}
		}
	}

	void _escapeID(StringSlice in, String& content) {
		for (unsigned int i = 0; i < in.size(); i += 1) {
			switch (in.get(i)) {
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
					content.append(in.get(i));
					break;

				case '-':
				case ' ': 
					content.append('-'); 
					break;

				default: 
					break;
			}
		}
	}

	void _escapeClass(StringSlice in, String& content) {
		for (unsigned int i = 0; i < in.size(); i += 1) {
			switch (in.get(i)) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					content.append(in.get(i));
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
				case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
				case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
					content.append(in.get(i) + ' ');
					break;

				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
				case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
				case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
					content.append(in.get(i));
					break;

				case '-':
				case ' ': 
					content.append('-'); 
					break;

				default: 
					break;
			}
		}
	}

	void _escapeID(const Node* node, String& content) {
		if (node->kind == NodeKind::text) {
			_escapeID(static_cast<const Text*>(node)->content, content);
		}

		if (node->kind == NodeKind::subtitle) {
			return;
		}

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_escapeID(node->children.get(i), content);
		}
	}

	void _cite(StringSlice id, String& content) {
		// APA Style:
		// Single author
		// (Author last name, the year of publication)
		//
		// Two authors
		// (First author last name & second author last name, the year of publication)
		//
		// More than two authors
		// (First author last name et al., the year of publication)
		//
		// Direct page quatation
		// (Same rule as above, the year of publication, p. page number)
		HashTable<const ReferenceInfo*>* table = _references.get(id);

		if (table == nullptr) {
			return;
		}

		content.append("(");
		const ReferenceInfo** author = table->get("author");
		if (author != nullptr) {
			String authorContent;
			_compose(*author, authorContent);
			Array<StringSlice> nameParts = _splitName(StringSlice(authorContent.pointer(), authorContent.size()));
			StringSlice lastName = nameParts.get(nameParts.size() - 1);
			content.append(lastName.pointer(), lastName.size());
		}
		const ReferenceInfo** authors = table->get("authors");
		if (authors != nullptr) {
			String authorsContent;
			_compose(*authors, authorsContent);
			Array<StringSlice> names = _splitByComma(StringSlice(authorsContent.pointer(), authorsContent.size()));
			if (names.size() == 2) {
				Array<StringSlice> nameParts0 = _splitName(names.get(0));
				StringSlice lastName0 = nameParts0.get(nameParts0.size() - 1);
				content.append(lastName0.pointer(), lastName0.size());
				content.append(" & ");

				Array<StringSlice> nameParts1 = _splitName(names.get(1));
				StringSlice lastName1 = nameParts1.get(nameParts1.size() - 1);
				content.append(lastName1.pointer(), lastName1.size());
			} else {
				Array<StringSlice> nameParts0 = _splitName(names.get(0));
				StringSlice lastName0 = nameParts0.get(nameParts0.size() - 1);
				content.append(lastName0.pointer(), lastName0.size());
				content.append(" et al.");
			}
		}
		const ReferenceInfo** year = table->get("year");
		if (year != nullptr) {
			content.append(", ");
			_compose(*year, content);
		}
		content.append(")");
	}

	void _refer(StringSlice id, String& content) {
		// APA Style:
		// Author’s Last Name, First Initial. Second Initial. (Year of publication). <i>Title of the book</i>. Publishing Company. 
		HashTable<const ReferenceInfo*>* table = _references.get(id);

		if (table == nullptr) {
			return;
		}

		const ReferenceInfo** author = table->get("author");
		if (author != nullptr) {
			String authorContent;
			_compose(*author, authorContent);
			Array<StringSlice> nameParts = _splitName(StringSlice(authorContent.pointer(), authorContent.size()));
			_composeLastNameInitials(nameParts, content);
			content.append(",");
		}
		const ReferenceInfo** authors = table->get("authors");
		if (authors != nullptr) {
			String authorsContent;
			_compose(*authors, authorsContent);
			Array<StringSlice> names = _splitByComma(StringSlice(authorsContent.pointer(), authorsContent.size()));
			if (names.size() > 1) {
				for (unsigned int i = 0; i < names.size() - 1; i += 1) {
					if (i != 0) {
						content.append(", ");
					}
					Array<StringSlice> nameParts = _splitName(names.get(i));
					_composeLastNameInitials(nameParts, content);
				}
			}

			content.append(" & ");
			Array<StringSlice> nameParts = _splitName(names.get(names.size() - 1));
			_composeLastNameInitials(nameParts, content);
		}
		const ReferenceInfo** year = table->get("year");
		if (year != nullptr) {
			content.append(" (");
			_compose(*year, content);
			content.append(").");
		}
		const ReferenceInfo** title = table->get("title");
		if (title != nullptr) {
			content.append(" <i>");
			_compose(*title, content);
			content.append("</i>.");
		}
		const ReferenceInfo** pubisher = table->get("publisher");
		if (pubisher != nullptr) {
			content.append(" ");
			_compose(*pubisher, content);
			content.append(".");
		}
	}

	void _composeLastNameInitials(Array<StringSlice>& nameParts, String& content) {
		if (nameParts.size() == 0) {
			return;
		}

		if (nameParts.size() == 1) {
			content.append(nameParts.get(0).pointer(), nameParts.get(0).size());
			return;
		}

		content.append(nameParts.get(nameParts.size() - 1).pointer(), nameParts.get(nameParts.size() - 1).size());
		content.append(", ");

		for (unsigned int i = 0; i < nameParts.size() - 1; i += 1) {
			content.append(nameParts.get(i).pointer(), 1);
			content.append(".");
		}

	}

	Array<StringSlice> _splitByComma(StringSlice from) {
		Array<StringSlice> parts;

		unsigned int begin = 0;
		unsigned int i = 0;
		while (i < from.size()) {
			if (from.get(i) == ',') {
				parts.append(from.cut(begin, i - begin));
				i += 1;
				while (i < from.size() && from.get(i) == ' ') {
					i += 1;
				}
				begin = i;
				continue;
			}
			i += 1;
		}

		if (begin < i) {
			parts.append(from.cut(begin, i - begin));
		}

		return parts;
	}

	Array<StringSlice> _splitName(StringSlice name) {
		Array<StringSlice> parts;

		unsigned int begin = 0;
		unsigned int i = 0;
		while (i < name.size()) {
			if (name.get(i) == ' ') {
				if (name.get(begin) >= 'A' && name.get(begin) <= 'Z') {
					parts.append(name.cut(begin, i - begin));
					i += 1;
					while (i < name.size() && name.get(i) == ' ') {
						i += 1;
					}
					begin = i;
					continue;
				}
			}
			i += 1;
		}

		if (begin < i) {
			parts.append(name.cut(begin, i - begin));
		}

		return parts;
	}

private:
	String _toc;

	String _content;

	Slice<Table::Alignment> _tableAlignments;

	unsigned int _tableColumnIndex;

	bool _tableLabel;

	Array<const Footnote*> _footnotes;

	// referenceID -> infoKey -> infoValue
	HashTable<HashTable<const ReferenceInfo*>> _references;
};

}
}
