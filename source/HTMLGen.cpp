#include "HTMLGen.hpp"
#include "Parser.hpp"
#include <stdio.h>

struct HTMLGen {
	String _source;
	Node* _node;

	HTMLGen(Node* node) {
		_node = node;
	}

	String gen() {
		_gen(_node);
		return _source;
	}

	String _escape(String const& value) {
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
			case NodeKind_hashtag:
			case NodeKind_emoji:
			case NodeKind_thematicbreak: {
				break;
			}
			case NodeKind_document: {
				DocumentNode* document = static_cast<DocumentNode*>(node);
				_genArray(document->children);
				break;
			}
			case NodeKind_checkitem: {
				CheckItemNode* item = static_cast<CheckItemNode*>(node);
				break;
			}
			case NodeKind_section: {
				HierarchyNode* n = static_cast<HierarchyNode*>(node);
				_source.append("<section class=\"section\">\n");
				_genArray(n->children);
				_source.append("</section>\n");
				break;
			}
			case NodeKind_subsection: {
				HierarchyNode* n = static_cast<HierarchyNode*>(node);
				_source.append("<section class=\"subsection\">");
				_genArray(n->children);
				_source.append("</section>\n");
				break;
			}
			case NodeKind_subsubsection: {
				HierarchyNode* n = static_cast<HierarchyNode*>(node);
				_source.append("<section class=\"subsubsection\">");
				_genArray(n->children);
				_source.append("</section>\n");
				break;
			}
			case NodeKind_title: {
				HierarchyNode* n = static_cast<HierarchyNode*>(node);
				_source.append("<h1>");
				_genArray(n->children);
				_source.append("</h1>");
				break;
			}
			case NodeKind_subtitle: {
				HierarchyNode* n = static_cast<HierarchyNode*>(node);
				_source.append("<small>");
				_genArray(n->children);
				_source.append("</small>\n");
				break;
			}
			case NodeKind_quote:
			case NodeKind_paragraph:
			case NodeKind_list:
			case NodeKind_item:
			case NodeKind_numberedlist:
			case NodeKind_numbereditem:
			case NodeKind_checklist:
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
};

String genHTML(Node* node) {
	HTMLGen gen(node);
	return gen.gen();
}

