#include "HTMLGen.hpp"
#include "EmojiMaker.hpp"
#include "Parser.hpp"
#include "Collection/Conv.hpp"
#include "Linker.hpp"
#include "Stripper.hpp"
#include <stdio.h>

struct HTMLGen {
	String _source;
	Node* _node;

	Array<HierarchyNode*> _sectionStacks;
	Array<String> _sectionTitleStacks;

	TableNode* _activeTable;
	int _activeTableRow = 0;
	int _activeTableColumn = 0;
	Array<ResourceNode*> _footnotes;

	int _sectionIndex = -1;
	int _subsectionIndex = -1;

	Linker _linker;

	HTMLGen(Node* node) {
		_node = node;
		_activeTable = nullptr;
	}

	String gen() {
		if (_node == nullptr) {
			return _source;
		}
		_linker.collect(_node);

		_sectionIndex = -1;
		_subsectionIndex = -1;

		_gen(_node);
		_genFootnote();
		return _source;
	}

	String _escape(String const& value) {
		String escapedValue;
		char const* items = value.items();
		for (int i = 0; i < value.size(); i++) {
			switch (items[i]) {
				case '&':
					escapedValue.append("&amp;");
					break;
				case '<':
					escapedValue.append("&lt;");
					break;
				case '>':
					escapedValue.append("&gt;");
					break;
				case '"':
					escapedValue.append("&quot;");
					break;
				case '\'':
					escapedValue.append("&apos;");
					break;
				default:
					escapedValue.append(1, items + i);
					break;
			}
		}
		return value;
	}

	void _genArray(Array<Node*> nodes) {
		for (int i = 0; i < nodes.size(); i++) {
			_gen(nodes[i]);
		}
	}

	void _gen(Node* node) {
		switch (node->kind) {
			case NodeKind_text: {
				ContentNode* n = static_cast<ContentNode*>(node);
				_source.append(_escape(n->content));
				break;
			}
			case NodeKind_space: {
				_source.append("\n");
				break;
			}
			case NodeKind_hyphen: {
				_source.append("-");
				break;
			}
			case NodeKind_endash: {
				_source.append("&ndash;");
				break;
			}
			case NodeKind_emdash: {
				_source.append("&mdash;");
				break;
			}
			case NodeKind_rightquote: {
				_source.append("&rdquo;");
				break;
			}
			case NodeKind_leftquote: {
				_source.append("&ldquo;");
				break;
			}
			case NodeKind_singlerightquote: {
				_source.append("&rsquo;");
				break;
			}
			case NodeKind_singleleftquote: {
				_source.append("&lsquo;");
				break;
			}
			case NodeKind_linebreak: {
				_source.append("<br>\n");
				break;
			}
			case NodeKind_thematicbreak: {
				_source.append("<hr>\n");
				break;
			}
			case NodeKind_hashtag: {
				ContentNode* contentNode = static_cast<ContentNode*>(node);
				_source.append("<a class=\"hashtag\" href=\"");
				_source.append(_escape(contentNode->content));
				_source.append("\">#");
				_source.append(_escape(contentNode->content));
				_source.append("</a>");
				break;
			}
			case NodeKind_emoji: {
				ContentNode* contentNode = static_cast<ContentNode*>(node);
				String emoji = EmojiMaker::make(contentNode->content);
				if (emoji.size() == 0) {
					_source.append(":");
					_source.append(contentNode->content);
					_source.append(":");
					break;
				}
				_source.append(emoji);
				break;
			}
			case NodeKind_document: {
				DocumentNode* document = static_cast<DocumentNode*>(node);
				_genArray(document->children);
				break;
			}
			case NodeKind_section:
			case NodeKind_subsection:
			case NodeKind_subsubsection: {
				HierarchyNode* hierarchyNode = static_cast<HierarchyNode*>(node);
				_sectionStacks.append(hierarchyNode);
				switch (hierarchyNode->kind) {
					case NodeKind_section:
						_sectionIndex++;
						_subsectionIndex = -1;
						_source.append("<section class=\"section\">\n");
						break;
					case NodeKind_subsection:
						_subsectionIndex++;
						_source.append("<section class=\"subsection\">\n");
						break;
					case NodeKind_subsubsection:
						_source.append("<section class=\"subsubsection\">\n");
						break;
					default:
						break;
				}
				_genArray(hierarchyNode->children);
				_genFootnote();
				_source.append("</section>\n");

				if (_sectionStacks.size() != 0) {
					_sectionStacks.remove(_sectionStacks.size() - 1);
				}
				if (_sectionTitleStacks.size() != 0) {
					_sectionTitleStacks.remove(_sectionTitleStacks.size() - 1);
				}
				break;
			}
			case NodeKind_title:
				if (_sectionStacks.size() != 0) {
					HierarchyNode* title = static_cast<HierarchyNode*>(node);
					HierarchyNode* section = _sectionStacks[_sectionStacks.size() - 1];
					switch (section->kind) {
						case NodeKind_section:
							_source.append("<h1 id=\"");
							break;
						case NodeKind_subsection:
							_source.append("<h2 id=\"");
							break;
						case NodeKind_subsubsection:
							_source.append("<h3 id=\"");
							break;
						default:
							break;
					}
					String titleText = stripTitleOnly(title);
					_sectionTitleStacks.append(titleText);
					for (int i = 0; i < _sectionTitleStacks.size(); i++) {
						if (i != 0) {
							_source.append("-");
						}
						_source.append(_escapeId(_sectionTitleStacks[i]));
					}
					_source.append("\">");
					_genArray(title->children);
					switch (section->kind) {
						case NodeKind_section:
							_source.append("</h1>\n");
							break;
						case NodeKind_subsection:
							_source.append("</h2>\n");
							break;
						case NodeKind_subsubsection:
							_source.append("</h3>\n");
							break;
						default:
							break;
					}
				}
				break;
			case NodeKind_subtitle:
				_genHierarchy(node, "<small>", "</small>");
				break;
			case NodeKind_paragraph:
				_genHierarchy(node, "<p>", "</p>\n");
				break;
			case NodeKind_quote:
				_genHierarchy(node, "<blockquote>\n", "</blockquote>\n");
				break;
			case NodeKind_list:
				_genHierarchy(node, "<ul>\n", "</ul>\n");
				break;
			case NodeKind_item:
				_genHierarchy(node, "<li>", "</li>\n");
				break;
			case NodeKind_numberedlist:
				_genHierarchy(node, "<ol>\n", "</ol>\n");
				break;
			case NodeKind_numbereditem:
				_genHierarchy(node, "<li>", "</li>\n");
				break;
			case NodeKind_checklist:
				_genHierarchy(node, "<ul class=\"check-list\">\n", "</ul>\n");
				break;
			case NodeKind_checkitem: {
				CheckItemNode* itemNode = static_cast<CheckItemNode*>(node);
				if (itemNode->isChecked){
					_genHierarchy(node, "<li><input type=\"checkbox\" checked>", "</li>\n");
					break;
				} 
				_genHierarchy(node, "<li><input type=\"checkbox\">", "</li>\n");
				break;
			}
			case NodeKind_strong:
				_genHierarchy(node, "<strong>", "</strong>");
				break;
			case NodeKind_emphasis:
				_genHierarchy(node, "<em>", "</em>");
				break;

			case NodeKind_table: {
				TableNode* table = static_cast<TableNode*>(node);
				TableNode* oldTable = _activeTable;
				_activeTable = table;
				_activeTableRow = 0;
				_genHierarchy(node, "<table>\n", "</table>\n");
				_activeTable = oldTable;
				break;
			}
			case NodeKind_row:
				_activeTableColumn = 0;
				_genHierarchy(node, "<tr>\n", "</tr>\n");
				_activeTableRow++;
				break;
			case NodeKind_cell:
				if (_activeTable) {
					if (_activeTableRow == 0 && _activeTable->isHeadered) {
						_genHierarchy(node, "<th>", "</th>\n");
						_activeTableColumn++;
						break;
					}
					if (_activeTableColumn < _activeTable->alignments.size()) {
						switch (_activeTable->alignments[_activeTableColumn]) {
							case Alignment_left:
								_genHierarchy(node, "<td class=\"text-left\">", "</td>\n");
								_activeTableColumn++;
								break;
							case Alignment_center:
								_genHierarchy(node, "<td class=\"text-center\">", "</td>\n");
								_activeTableColumn++;
								break;
							case Alignment_right:
								_genHierarchy(node, "<td class=\"text-right\">", "</td>\n");
								_activeTableColumn++;
								break;
							default:
								_genHierarchy(node, "<td>", "</td>\n");
								_activeTableColumn++;
								break;
						}
						break;
					}
					_genHierarchy(node, "<td>", "</td>\n");
					_activeTableColumn++;
					break;
				}
				break;
			case NodeKind_link:
				_genLink(static_cast<ResourceNode*>(node));
				break;
			case NodeKind_view: {
				ResourceNode* res = static_cast<ResourceNode*>(node);
				String ext;
				for (int i = res->source.size() - 1; i >= 0; i--) {
					if (res->source[i] == '.') {
						ext = res->source.slice(i, res->source.size() - i);
						break;
					}
				}
				if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif") {
					_source.append("<img src=\"");
					_source.append(_escape(res->source));
					_source.append("\">");
					break;
				}
				_genLink(res);
				break;
			}
			case NodeKind_mention: {
				ResourceNode* res = static_cast<ResourceNode*>(node);
				ResourceNode* result = _linker.mention(res, _sectionIndex, _subsectionIndex);
				if (result == nullptr) {
					break;
				}
				_source.append("<a href=\"#");
				_source.append(_escapeId(result->source));
				_source.append("\">");
				_genArray(result->children);
				_source.append("</a>");
				delete result;
				break;
			}
			case NodeKind_footnote: {
				ResourceNode* res = static_cast<ResourceNode*>(node);
				_source.append("<sup><a class=\"footnote\" href=\"#Footnote-");
				_source.append(longToString(_footnotes.size() + 1));
				_source.append("\">");
				_source.append(longToString(_footnotes.size() + 1));
				_source.append("</a></sup>");
				_footnotes.append(res);
				break;
			}
			case NodeKind_fencedcode: {
				CodeNode* code = static_cast<CodeNode*>(node);
				bool isView = code->lang.size() != 0 && code->lang[0] == '!';
				_source.append(isView ? "<div class=\"view\">" : "<pre>");
				_genCode(node);
				_source.append(isView ? "</div>\n" : "</pre>\n");
				break;
			}
			case NodeKind_code:
				_genCode(node);
				break;
			case NodeKind_admon: {
				AdmonNode* admon = static_cast<AdmonNode*>(node);
				if (admon->type == "NOTE") {
					_source.append("<div class=\"admonition note\">\n");
					_source.append("<div class=\"label\">Note</div>\n");
				} else if (admon->type == "HINT") {
					_source.append("<div class=\"admonition hint\">\n");
					_source.append("<div class=\"label\">Hint</div>\n");
				} else if (admon->type == "IMPORTANT") {
					_source.append("<div class=\"admonition important\">\n");
					_source.append("<div class=\"label\">Important</div>\n");
				} else if (admon->type == "WARNING") {
					_source.append("<div class=\"admonition warning\">\n");
					_source.append("<div class=\"label\">Warning</div>\n");
				} else if (admon->type == "SEE") {
					_source.append("<div class=\"admonition see\">\n");
					_source.append("<div class=\"label\">See</div>\n");
				} else if (admon->type == "TIP") {
					_source.append("<div class=\"admonition tip\">\n");
					_source.append("<div class=\"label\">Tip</div>\n");
				} else {
					_source.append("<div class=\"admonition note\">\n");
					_source.append("<div class=\"label\">Note</div>\n");
				}
				_source.append("<div class=\"content\">\n");
				_genArray(admon->children);
				_source.append("</div>\n");
				_source.append("</div>\n");
				break;
			}
			case NodeKind_include: {
				break;
			}
			case NodeKind_func: {
				FuncNode* func = static_cast<FuncNode*>(node);
				if (func->symbol == "outline") {
					_source.append("<ul class=\"outline\">\n");
					Array<Section> sections = _linker.makeOutline();
					for (int i = 0; i < sections.size(); i++) {
						Section const& section = sections[i];
						String sectionId = _escapeId(section.title);
						_source.append("<li class=\"section\">\n");
						_source.append("<a href=\"#");
						_source.append(sectionId);
						_source.append("\">");
						_source.append(section.title);
						if (section.subtitle.size() != 0) {
							_source.append(": ");
							_source.append(section.subtitle);
						}
						_source.append("</a>\n");
						if (section.subsections.size() != 0) {
							_source.append("<ul>\n");
							for (int j = 0; j < section.subsections.size(); j++) {
								Subsection const& subsection = section.subsections[j];
								String subsectionId = _escapeId(subsection.title);
								_source.append("<li class=\"subsection\">\n");
								_source.append("<a href=\"#");
								_source.append(sectionId);
								_source.append("-");
								_source.append(subsectionId);
								_source.append("\">");
								_source.append(subsection.title);
								if (subsection.subtitle.size() != 0) {
									_source.append(": ");
									_source.append(subsection.subtitle);
								}
								_source.append("</a>\n");
								if (subsection.subsubsections.size() != 0) {
									_source.append("<ul>\n");
									for (int k = 0; k < subsection.subsubsections.size(); k++) {
										Subsubsection const& subsubsection = subsection.subsubsections[k];
										String subsubsectionId = _escapeId(subsubsection.title);
										_source.append("<li class=\"subsubsection\">\n");
										_source.append("<a href=\"#");
										_source.append(sectionId);
										_source.append("-");
										_source.append(subsectionId);
										_source.append("-");
										_source.append(subsubsectionId);
										_source.append("\">");
										_source.append(subsubsection.title);
										if (subsubsection.subtitle.size() != 0) {
											_source.append(": ");
											_source.append(subsubsection.subtitle);
										}
										_source.append("</a>\n");
										_source.append("</li>\n");
									}
									_source.append("</ul>\n");
								}
								_source.append("</li>\n");
							}
							_source.append("</ul>\n");
						}
						_source.append("</li>\n");
					}
					_source.append("</ul>\n");
					break;
				}
				if (func->symbol == "bibliography") {
					_source.append("<div class=\"bibliography\" id=\"");
					_source.append(_escapeId(func->arguments.has("id") ? func->arguments["id"] : ""));
					_source.append("\">\n");
					Array<Node*> nodes = _linker.makeBibliography(func);
					_genArray(nodes);
					for (int i = 0; i < nodes.size(); i++) {
						delete nodes[i];
					}
					_source.append("</div>\n");
					break;
				}
				break;
			}
		}
	}

	void _genCode(Node* node) {
		CodeNode* codeNode = static_cast<CodeNode*>(node);
		if (codeNode->lang.size() != 0) {
			if (codeNode->lang[0] == '!') {
				_source.append("<span class=\"view language-");
				_source.append(_escape(codeNode->lang.slice(1, codeNode->lang.size() - 1)));
				_source.append("\">");
				_source.append(_escape(codeNode->content));
				_source.append("</span>");
				return;
			}
			_source.append("<code class=\"language-");
			_source.append(_escape(codeNode->lang));
			_source.append("\">");
			_source.append(_escape(codeNode->content));
			_source.append("</code>");
			return;
		}
		_source.append("<code>");
		_source.append(_escape(codeNode->content));
		_source.append("</code>");
		return;
	}

	void _genHierarchy(Node* node, String const& openTag, String const& closeTag) {
		HierarchyNode* hierarchyNode = static_cast<HierarchyNode*>(node);
		_source.append(openTag);
		_genArray(hierarchyNode->children);
		_source.append(closeTag);
	}

	void _genLink(ResourceNode* res) {
		_source.append("<a href=\"");
		_source.append(_escape(res->source));
		_source.append("\">");
		if (res->children.size() == 0) {
			_source.append(_escape(res->source));
		} else {
			_genArray(res->children);
		}
		_source.append("</a>");
	}

	void _genFootnote() {
		if (_footnotes.size() != 0) {
			_source.append("<div class=\"footnote-description\">\n");
			for (int i = 0; i < _footnotes.size(); i++) {
				_source.append("<div id=\"Footnote-");
				_source.append(longToString(i + 1));
				_source.append("\"><sup>");
				_source.append(longToString(i + 1));
				_source.append("</sup>");
				_genArray(_footnotes[i]->children);
				_source.append("</div>\n");
			}
			_source.append("</div>\n");
			_footnotes = Array<ResourceNode*>();
		}
	}

	String _escapeId(String const& value) {
		String id;
		for (int i = 0; i < value.size(); i++) {
			switch (value[i]) {
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F': case 'G': case 'H':
				case 'I': case 'J': case 'K': case 'L':
				case 'M': case 'N': case 'O': case 'P':
				case 'Q': case 'R': case 'S': case 'T':
				case 'U': case 'V': case 'W': case 'X':
				case 'Y': case 'Z':
				case 'a': case 'b': case 'c': case 'd':
				case 'e': case 'f': case 'g': case 'h':
				case 'i': case 'j': case 'k': case 'l':
				case 'm': case 'n': case 'o': case 'p':
				case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x':
				case 'y': case 'z': case '-':
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					id.append(1, value.items() + i);
					break;
				case ' ':
					id.append("-");
				default:
					break;
			}
		}
		return id;
	}
};

String genHTML(Node* node) {
	HTMLGen gen(node);
	return gen.gen();
}

