#include "HTMLGen.hpp"
#include "EmojiGen.hpp"
#include "Parser.hpp"
#include <stdio.h>

struct HTMLGen {
	String _source;
	Node* _node;
	Node* _activeSection;

	HTMLGen(Node* node) {
		_node = node;
		_activeSection = nullptr;
	}

	String gen() {
		_gen(_node);
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
				Node* oldSection = _activeSection;
				_activeSection = node;
				switch (_activeSection->kind) {
					case NodeKind_section:
						_genHierarchy(node, "<section class=\"section\">\n", "</section>\n");
						break;
					case NodeKind_subsection:
						_genHierarchy(node, "<section class=\"subsection\">\n", "</section>\n");
						break;
					case NodeKind_subsubsection:
						_genHierarchy(node, "<section class=\"subsubsection\">\n", "</section>\n");
						break;
					default:
						break;
				}
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
			case NodeKind_emphasis:
			case NodeKind_row:
			case NodeKind_cell: {
				HierarchyNode* h = static_cast<HierarchyNode*>(node);
				_genArray(h->children);
				break;
			}
			case NodeKind_link:
			case NodeKind_view:
			case NodeKind_cite:
			case NodeKind_footnote: {
				ResourceNode* r = static_cast<ResourceNode*>(node);
				_genArray(r->children);
				break;
			}
			case NodeKind_fencedcode:
			case NodeKind_code: {
				CodeNode* c = static_cast<CodeNode*>(node);
				String code = _escape(c->content);
				break;
			}
			case NodeKind_admon: {
				AdmonNode* a = static_cast<AdmonNode*>(node);
				_genArray(a->children);
				break;
			}
			case NodeKind_table: {
				TableNode* t = static_cast<TableNode*>(node);
				_genArray(t->children);
				break;
			}
			case NodeKind_func: {
				FuncNode* func = static_cast<FuncNode*>(node);
				break;
			}
			default:
				return;
		}
	}

	void _genHierarchy(Node* node, String const& openTag, String const& closeTag) {
		HierarchyNode* hierarchyNode = static_cast<HierarchyNode*>(node);
		_source.append(openTag);
		_genArray(hierarchyNode->children);
		_source.append(closeTag);
	}
};

String genHTML(Node* node) {
	HTMLGen gen(node);
	return gen.gen();
}

