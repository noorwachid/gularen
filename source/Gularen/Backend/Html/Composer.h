#pragma once

#include "Gularen/Frontend/Parser.h"
#include <unordered_map>

namespace Gularen {
namespace Html {

class Composer {
public:
	std::string_view compose(Document* document) {
		_content = std::string();
		_tableAlignments = nullptr;
		_tableColumnIndex = 0;
		_tableLabel = false;

		if (document != nullptr) {
			_collectReferences(document);

			for (size_t i = 0; i < document->children.size(); i += 1) {
				_compose(document->children[i], _content);
			}
		}

		_composeFootnote();

		return std::string_view(_content.data(), _content.size());
	}

	std::string_view composeToc(Document* document) {
		_toc.append("<ul class=\"toc\">\n");
		_composeToc(document);
		_toc.append("</ul>\n");
		return std::string_view(_toc.data(), _toc.size());
	}

private:
	void _composeToc(const Node* node) {
		if (node->kind == NodeKind::heading) {
			auto heading = static_cast<const Heading*>(node);

			if (heading->type == Heading::Type::section ||
				heading->type == Heading::Type::subsection ||
				heading->type == Heading::Type::subsubsection) {

				std::string content;

				for (size_t i = 0; i < heading->children.size(); i += 1) {
					if (heading->children[i]->kind == NodeKind::subtitle) {
						break;
					}
					_compose(heading->children[i], content);
				}

				if (content.size() != 0) {
					while (content[content.size() - 1] == ' ') {
						content.pop_back();
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

				std::string id;
				_toc.append("\"><a href=\"#");
				_escapeID(std::string_view(content.data(), content.size()), id);
				_toc.append(id);
				_toc.append("\">");
				_toc.append(content);
				_toc.append("</a></li>\n");
			}
		}

		for (size_t i = 0; i < node->children.size(); i += 1) {
			_composeToc(node->children[i]);
		}
	}

	void _composeFootnote() {
		if (_footnotes.size() != 0) {
			_content.append("<div class=\"footnote-desc\">\n");
			for (size_t i = 0; i < _footnotes.size(); i += 1) {
				const Footnote* footnote = _footnotes[i];

				_content.append("<p>");
				_content.append("<sup>");
				_content.append(std::to_string(i + 1));
				_content.append("</sup> ");
				_content.append(footnote->desc.data(), footnote->desc.size());
				_content.append("</p>\n");
			}
			_content.append("</div>\n");

			_footnotes.clear();
		}
	}
	void _collectReferences(const Node* node) {
		if (node->kind == NodeKind::reference) {
			const Reference* ref = static_cast<const Reference*>(node);
			auto& refTable = _references[ref->id];

			for (size_t i = 0; i < ref->children.size(); i += 1) {
				const ReferenceInfo* info = static_cast<const ReferenceInfo*>(ref->children[i]);
				refTable[info->key] = info;
			}

			return;
		}

		for (size_t i = 0; i < node->children.size(); i += 1) {
			_collectReferences(node->children[i]);
		}
	}

	void _compose(const Node* node, std::string& content) {
		_preCompose(node, content);

		if (node->kind == NodeKind::reference) {
			return;
		}

		for (size_t i = 0; i < node->children.size(); i += 1) {
			_compose(node->children[i], content);
		}

		_postCompose(node, content);
	}

	void _preCompose(const Node* node, std::string& content) {
		switch (node->kind) {
			case NodeKind::text: {
				const Text* text = static_cast<const Text*>(node);
				content.append(text->content);
				return;
			}

			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: content.append("<b>"); return;
					case Style::Type::italic: content.append("<i>"); return;
				}
			}

			case NodeKind::highlight: {
				content.append("<mark>");
				return;
			}

			case NodeKind::heading: {
				_composeFootnote();

				const Heading* heading = static_cast<const Heading*>(node);
				switch (heading->type) {
					case Heading::Type::section:
						content.append("<h1 id=\"");
						_escapeID(heading, content);
						content.append("\"");
						break;
					case Heading::Type::subsection:
						content.append("<h2 id=\"");
						_escapeID(heading, content);
						content.append("\"");
						break;
					case Heading::Type::subsubsection:
						content.append("<h3 id=\"");
						_escapeID(heading, content);
						content.append("\"");
						break;
				}
				_composeAnnotations(node->annotations);
				content.append(">");
				return;
			}

			case NodeKind::subtitle: {
				content.append("<small>");
				return;
			}

			case NodeKind::paragraph: {
				content.append("<p");
				_composeAnnotations(node->annotations);
				content.append(">");
				return;
			}

			case NodeKind::space: {
				content.append("\n");
				return;
			}

			case NodeKind::lineBreak: {
				content.append("<br>");
				return;
			}

			case NodeKind::pageBreak: {
				_composeFootnote();
				content.append("<div class=\"page-break\"></div>\n\n");
				return;
			}

			case NodeKind::dinkus: {
				content.append("<hr");
				_composeAnnotations(node->annotations);
				content.append(">\n\n");
				return;
			}

			case NodeKind::indent: {
				content.append("<blockquote");
				_composeAnnotations(node->annotations);
				content.append(">\n");
				return;
			}

			case NodeKind::list: {
				content.append("<ul");
				_composeAnnotations(node->annotations);
				content.append(">\n");
				return;
			}

			case NodeKind::numberedList: {
				content.append("<ol");
				_composeAnnotations(node->annotations);
				content.append(">\n");
				return;
			}

			case NodeKind::checkList: {
				content.append("<ul class=\"check-list");
				_composeInnerAnnotations(node->annotations);
				content.append("\">\n");
				return;
			}

			case NodeKind::definitionList: {
				content.append("<dl");
				_composeAnnotations(node->annotations);
				content.append(">\n");
				return;
			}

			case NodeKind::definitionTerm: {
				content.append("<dt>");
				return;
			}

			case NodeKind::definitionDesc: {
				content.append("<dd>");
				return;
			}

			case NodeKind::item: {
				content.append("<li>");
				return;
			}

			case NodeKind::checkItem: {
				if (static_cast<const CheckItem*>(node)->checked) {
					content.append("<li> <input type=\"checkbox\" checked> "); 
					return;
				}

				content.append("<li> <input type=\"checkbox\"> "); 
				return;
			}

			case NodeKind::table: {
				const Table* table = static_cast<const Table*>(node);
				_tableAlignments = &table->alignments;
				content.append("<table");
				_composeAnnotations(node->annotations);
				content.append(">\n");
				return;
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

				content.append("<tr>\n");
				return;
			}

			case NodeKind::cell: {
				if (_tableColumnIndex < _tableAlignments->size()) {
					Table::Alignment alignment = _tableAlignments->at(_tableColumnIndex);
					_tableColumnIndex += 1;

					switch (alignment) {
						case Table::Alignment::left: content.append(_tableLabel ? "<th class=\"cell-left\">" : "<td class=\"cell-center\">"); return;
						case Table::Alignment::center: content.append(_tableLabel ? "<th class=\"cell-center\">" : "<td class=\"cell-center\">"); return;
						case Table::Alignment::right: content.append(_tableLabel ? "<th class=\"cell-right\">" : "<td class=\"cell-right\">"); return;
					}
				}

				_tableColumnIndex += 1;
				content.append(_tableLabel ? "<th>" : "<td>");
				return;
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
					_composeAnnotations(node->annotations);
					content.append(">");
					_escape(code->content, content);
					content.append("</code></pre>\n\n");
					return;
				}

				content.append("<pre><code");
				_composeAnnotations(node->annotations);
				content.append(">");
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

				for (size_t i = view->resource.size(); i > 0; i -= 1) {
					if (view->resource[i - 1] == '.') {
						std::string_view extension(view->resource.data() + i, view->resource.size() - i);
						if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "gif") {
							if (view->label.size() != 0) {
								content.append("<figure");
								_composeAnnotations(node->annotations);
								content.append(">");
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
							content.append("\"");
							_composeAnnotations(node->annotations);
							content.append(">");
							return;
						}
						break;
					}
				}

				content.append("<a href=\"");
				_escapeAttribute(view->resource, content);
				content.append("\"");
				_composeAnnotations(node->annotations);
				content.append(">");

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
				std::string id = std::to_string(_footnotes.size() + 1);
				content.append("<sup><a href=\"#Footnote-");
				content.append(id);
				content.append("\">");
				content.append(id);
				content.append("</a></sup>");
				_footnotes.push_back(ref);
				return;
			}

			case NodeKind::inText: {
				const InText* inText = static_cast<const InText*>(node);
				content.append("<a class=\"in-text\" href=\"#Reference-");
				_escapeAttribute(inText->id, content);
				content.append("\">");
				_inText(inText->id, content);
				content.append("</a>");
				return;
			}

			case NodeKind::reference: {
				const Reference* ref = static_cast<const Reference*>(node);
				content.append("<div class=\"reference");
				_composeInnerAnnotations(node->annotations);
				content.append("\" id=\"Reference-");
				_escapeAttribute(ref->id, content);
				content.append("\">");
				_reference(ref->id, content);
				content.append("</div>\n");
				return;
			}

			case NodeKind::admon: {
				const Admon* ref = static_cast<const Admon*>(node);
				content.append("<div class=\"admon ");
				_escapeClass(ref->label, content);
				_composeInnerAnnotations(node->annotations);
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
					case Punct::Type::hypen: content.append("&#8208;"); return;
					case Punct::Type::enDash: content.append("&#8211;"); return;
					case Punct::Type::emDash: content.append("&#8212;"); return;
					case Punct::Type::quoteOpen: content.append("&#8223;"); return;
					case Punct::Type::quoteClose: content.append("&#8221;"); return;
					case Punct::Type::squoteOpen: content.append("&#8216;"); return;
					case Punct::Type::squoteClose: content.append("&#8217;"); return;
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
				content.append("<blockquote class=\"quote");
				_composeInnerAnnotations(node->annotations);
				content.append("\">\n");
				return;
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

	void _postCompose(const Node* node, std::string& content) {
		switch (node->kind) {
			case NodeKind::style: {
				switch (static_cast<const Style*>(node)->type) {
					case Style::Type::bold: content.append("</b>"); return;
					case Style::Type::italic: content.append("</i>"); return;
				}
			}

			case NodeKind::highlight: {
				content.append("</mark>");
				return;
			}

			case NodeKind::heading: {
				switch (static_cast<const Heading*>(node)->type) {
					case Heading::Type::section: content.append("</h1>\n"); return;
					case Heading::Type::subsection: content.append("</h2>\n"); return;
					case Heading::Type::subsubsection: content.append("</h3>\n"); return;
				}
			}

			case NodeKind::subtitle: {
				content.append("</small>"); 
				return;
			}

			case NodeKind::paragraph: {
				content.append("</p>\n\n");
				return;
			}

			case NodeKind::indent: {
				content.append("</blockquote>\n");
				return;
			}

			case NodeKind::list: {
				content.append("</ul>\n\n");
				return;
			}

			case NodeKind::numberedList: {
				content.append("</ol>\n\n");
				return;
			}

			case NodeKind::checkList: {
				content.append("</ul>\n\n");
				return;
			}

			case NodeKind::definitionList: {
				content.append("</dl>\n\n");
				return;
			}

			case NodeKind::definitionTerm: {
				content.append("</dt>\n");
				return;
			}

			case NodeKind::definitionDesc: {
				content.append("</dd>\n");
				return;
			}

			case NodeKind::item: {
				content.append("</li>\n");
				return;
			}

			case NodeKind::checkItem: {
				content.append("</li>\n"); 
				return;
			}

			case NodeKind::table: {
				content.append("</table>\n\n");
				return;
			}

			case NodeKind::row: {
				content.append("</tr>\n");
				return;
			}

			case NodeKind::cell: {
				content.append(_tableLabel ? "</th>\n" : "</td>\n");
				return;
			}

			case NodeKind::admon: {
				content.append("\n</div>\n</div>\n\n");
				return;
			}

			case NodeKind::blockquote: {
				content.append("</blockquote>\n\n");
				return;
			}

			default: break;
		}
	}

	void _composeAnnotations(const std::vector<Annotation>& annotations) {
		if (!annotations.empty()) {
			_content.append(" class=\"");
			for (size_t i = 0; i < annotations.size(); i += 1) {
				if (i != 0) {
					_content.append(" ");
				}
				_escapeClass(annotations[i].key, _content);
				_content.append("--");
				_escapeClass(annotations[i].value, _content);
			}
			_content.append("\"");
		}
	}

	void _composeInnerAnnotations(const std::vector<Annotation>& annotations) {
		for (size_t i = 0; i < annotations.size(); i += 1) {
			_content.append(" ");
			_escapeClass(annotations[i].key, _content);
			_content.append("--");
			_escapeClass(annotations[i].value, _content);
		}
	}

	void _escape(std::string_view in, std::string& content) {
		for (size_t i = 0; i < in.size(); i += 1) {
			switch (in[i]) {
				case '<': content.append("&lt;"); break;
				case '>': content.append("&gt;"); break;
				case '&': content.append("&amp;"); break;
				case '\"': content.append("&quot;"); break;
				case '\'': content.append("&#39;"); break;
				default: content.append(1, in[i]); break;
			}
		}
	}

	void _escapeAttribute(std::string_view in, std::string& content) {
		for (size_t i = 0; i < in.size(); i += 1) {
			switch (in[i]) {
				case '\"': content.append("&quot;"); break;
				case '\'': content.append("&#39;"); break;
				default: content.append(1, in[i]); break;
			}
		}
	}

	void _escapeID(std::string_view in, std::string& content) {
		for (size_t i = 0; i < in.size(); i += 1) {
			switch (in[i]) {
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
					content.append(1, in[i]);
					break;

				case '-':
				case ' ': 
					content.append(1, '-'); 
					break;

				default: 
					break;
			}
		}
	}

	void _escapeClass(std::string_view in, std::string& content) {
		for (size_t i = 0; i < in.size(); i += 1) {
			switch (in[i]) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					content.append(1, in[i]);
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
				case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
				case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
					content.append(1, in[i] + ' ');
					break;

				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
				case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
				case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
					content.append(1, in[i]);
					break;

				case '-':
				case ' ': 
					content.append(1, '-'); 
					break;

				default: 
					break;
			}
		}
	}

	void _escapeID(const Node* node, std::string& content) {
		if (node->kind == NodeKind::text) {
			_escapeID(static_cast<const Text*>(node)->content, content);
		}

		if (node->kind == NodeKind::subtitle) {
			return;
		}

		for (size_t i = 0; i < node->children.size(); i += 1) {
			_escapeID(node->children[i], content);
		}
	}

	void _inText(std::string_view id, std::string& content) {
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
		if (!_references.count(id)) {
			return;
		}

		auto& table = _references[id];

		content.append("(");
		if (table.count("author")) {
			const ReferenceInfo* author = table["author"];
			std::string authorContent;
			_compose(author, authorContent);
			std::vector<std::string_view> nameParts = _splitName(std::string_view(authorContent.data(), authorContent.size()));
			std::string_view lastName = nameParts.back();
			content.append(lastName.data(), lastName.size());
		}
		if (table.count("authors")) {
			const ReferenceInfo* authors = table["authors"];
			std::string authorsContent;
			_compose(authors, authorsContent);
			std::vector<std::string_view> names = _splitByComma(std::string_view(authorsContent.data(), authorsContent.size()));
			if (names.size() == 2) {
				std::vector<std::string_view> nameParts0 = _splitName(names[0]);
				std::string_view lastName0 = nameParts0.back();
				content.append(lastName0.data(), lastName0.size());
				content.append(" & ");

				std::vector<std::string_view> nameParts1 = _splitName(names[1]);
				std::string_view lastName1 = nameParts1.back();
				content.append(lastName1.data(), lastName1.size());
			} else {
				std::vector<std::string_view> nameParts0 = _splitName(names[0]);
				std::string_view lastName0 = nameParts0.back();
				content.append(lastName0.data(), lastName0.size());
				content.append(" et al.");
			}
		}
		if (table.count("year")) {
			const ReferenceInfo* year = table["year"];
			content.append(", ");
			_compose(year, content);
		}
		content.append(")");
	}

	void _reference(std::string_view id, std::string& content) {
		// APA Style:
		// Author’s Last Name, First Initial. Second Initial. (Year of publication). <i>Title of the book</i>. Publishing Company. 

		if (!_references.count(id)) {
			return;
		}

		auto table = _references[id];

		if (table.count("author")) {
			const ReferenceInfo* author = table["author"];
			std::string authorContent;
			_compose(author, authorContent);
			std::vector<std::string_view> nameParts = _splitName(std::string_view(authorContent.data(), authorContent.size()));
			_composeLastNameInitials(nameParts, content);
			content.append(",");
		}
		if (table.count("authors")) {
			const ReferenceInfo* authors = table["authors"];
			std::string authorsContent;
			_compose(authors, authorsContent);
			std::vector<std::string_view> names = _splitByComma(std::string_view(authorsContent.data(), authorsContent.size()));
			if (names.size() > 1) {
				for (size_t i = 0; i < names.size() - 1; i += 1) {
					if (i != 0) {
						content.append(", ");
					}
					std::vector<std::string_view> nameParts = _splitName(names[i]);
					_composeLastNameInitials(nameParts, content);
				}
			}

			content.append(" & ");
			std::vector<std::string_view> nameParts = _splitName(names.back());
			_composeLastNameInitials(nameParts, content);
		}
		if (table.count("year")) {
			const ReferenceInfo* year = table["year"];
			content.append(" (");
			_compose(year, content);
			content.append(").");
		}
		if (table.count("title")) {
			const ReferenceInfo* title = table["title"];
			content.append(" <i>");
			_compose(title, content);
			content.append("</i>.");
		}
		if (table.count("publisher")) {
			const ReferenceInfo* pubisher = table["publisher"];
			content.append(" ");
			_compose(pubisher, content);
			content.append(".");
		}
	}

	void _composeLastNameInitials(std::vector<std::string_view>& nameParts, std::string& content) {
		if (nameParts.size() == 0) {
			return;
		}

		if (nameParts.size() == 1) {
			content.append(nameParts[0]);
			return;
		}

		content.append(nameParts.back());
		content.append(", ");

		for (size_t i = 0; i < nameParts.size() - 1; i += 1) {
			content.append(nameParts[i].data(), 1);
			content.append(".");
		}

	}

	std::vector<std::string_view> _splitByComma(std::string_view from) {
		std::vector<std::string_view> parts;

		size_t begin = 0;
		size_t i = 0;
		while (i < from.size()) {
			if (from[i] == ',') {
				parts.push_back(from.substr(begin, i - begin));
				i += 1;
				while (i < from.size() && from[i] == ' ') {
					i += 1;
				}
				begin = i;
				continue;
			}
			i += 1;
		}

		if (begin < i) {
			parts.push_back(from.substr(begin, i - begin));
		}

		return parts;
	}

	std::vector<std::string_view> _splitName(std::string_view name) {
		std::vector<std::string_view> parts;

		size_t begin = 0;
		size_t i = 0;
		while (i < name.size()) {
			if (name[i] == ' ') {
				if (name[begin] >= 'A' && name[begin] <= 'Z') {
					parts.push_back(name.substr(begin, i - begin));
					i += 1;
					while (i < name.size() && name[i] == ' ') {
						i += 1;
					}
					begin = i;
					continue;
				}
			}
			i += 1;
		}

		if (begin < i) {
			parts.push_back(name.substr(begin, i - begin));
		}

		return parts;
	}

private:
	std::string _toc;

	std::string _content;

	const std::vector<Table::Alignment>* _tableAlignments;

	size_t _tableColumnIndex;

	bool _tableLabel;

	std::vector<const Footnote*> _footnotes;

	// referenceID -> infoKey -> infoValue
	std::unordered_map<std::string_view, std::unordered_map<std::string_view, const ReferenceInfo*>> _references;
};

}
}
