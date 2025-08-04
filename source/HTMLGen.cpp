#include "HTMLGen.hpp"
#include "EmojiGen.hpp"
#include "Parser.hpp"
#include "Collection/Conv.hpp"
#include <stdio.h>

struct HTMLGen {
	String _source;
	Node* _node;
	Node* _activeSection;
	TableNode* _activeTable;
	int _activeTableRow = 0;
	int _activeTableColumn = 0;
	Array<ResourceNode*> _footnotes;

	HTMLGen(Node* node) {
		_node = node;
		_activeSection = nullptr;
		_activeTable = nullptr;
	}

	String gen() {
		_gen(_node);
		_genFootnote();
		return _source;
	}

	String _escape(String const& value) {
		String escapedValue;
		Byte const* items = value.items();
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
				String emoji = genEmoji(contentNode->content);
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
				Node* oldSection = _activeSection;
				_activeSection = node;
				switch (_activeSection->kind) {
					case NodeKind_section:
						_source.append("<section class=\"section\">\n");
						break;
					case NodeKind_subsection:
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
				_activeSection = oldSection;
				break;
			}
			case NodeKind_title:
				if (_activeSection != nullptr) {
					switch (_activeSection->kind) {
						case NodeKind_section:
							_genHierarchy(node, "<h1>", "</h1>\n");
							break;
						case NodeKind_subsection:
							_genHierarchy(node, "<h2>", "</h2>\n");
							break;
						case NodeKind_subsubsection:
							_genHierarchy(node, "<h3>", "</h3>\n");
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
			case NodeKind_cite: {
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
					_source.append("<div class=\"label\">Important</div>\n");
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
				// FuncNode* func = static_cast<FuncNode*>(node);
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
};

String genHTML(Node* node) {
	HTMLGen gen(node);
	return gen.gen();
}

