#include "Stripper.hpp"
#include "Parser.hpp"

void _strip(String& text, Node const* node);

void _stripAll(String& text, Array<Node*> const& nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		_strip(text, nodes[i]);
	}
}

void _strip(String& text, Node const* node) {
	switch (node->kind) {
		case NodeKind_text: {
			ContentNode const* n = static_cast<ContentNode const*>(node);
			text.append(n->content);
			break;
		}
		case NodeKind_space: {
			text.append(" ");
			break;
		}
		case NodeKind_hyphen: {
			text.append("-");
			break;
		}
		case NodeKind_endash: {
			text.append("–");
			break;
		}
		case NodeKind_emdash: {
			text.append("—");
			break;
		}
		case NodeKind_rightquote: {
			text.append("”");
			break;
		}
		case NodeKind_leftquote: {
			text.append("“");
			break;
		}
		case NodeKind_singlerightquote: {
			text.append("‘");
			break;
		}
		case NodeKind_singleleftquote: {
			text.append("’");
			break;
		}
		case NodeKind_linebreak:
		case NodeKind_thematicbreak:
			break;
		case NodeKind_hashtag: {
			ContentNode const* contentNode = static_cast<ContentNode const*>(node);
			text.append("#");
			text.append(contentNode->content);
			break;
		}
		case NodeKind_emoji: {
			ContentNode const* contentNode = static_cast<ContentNode const*>(node);
			text.append(":");
			text.append(contentNode->content);
			text.append(":");
			break;
		}
		case NodeKind_document: {
			DocumentNode const* document = static_cast<DocumentNode const*>(node);
			_stripAll(text, document->children);
			break;
		}
		case NodeKind_section:
		case NodeKind_subsection:
		case NodeKind_subsubsection:
		case NodeKind_title:
		case NodeKind_subtitle:
		case NodeKind_paragraph:
		case NodeKind_quote:
		case NodeKind_list:
		case NodeKind_item:
		case NodeKind_numberedlist:
		case NodeKind_numbereditem:
		case NodeKind_checklist:
		case NodeKind_checkitem:
		case NodeKind_strong:
		case NodeKind_emphasis:
		case NodeKind_table:
		case NodeKind_row:
		case NodeKind_cell: {
			HierarchyNode const* hierarchyNode = static_cast<HierarchyNode const*>(node);
			_stripAll(text, hierarchyNode->children);
			break;
		}

		case NodeKind_link: {
			ResourceNode const* res = static_cast<ResourceNode const*>(node);
			if (res->children.size() != 0) {
				_stripAll(text, res->children);
				break;
			}
			text.append(res->source);
			break;
		}
		case NodeKind_view:
			break;
		case NodeKind_mention: {
			ResourceNode const* res = static_cast<ResourceNode const*>(node);
			text.append(res->source);
			break;
		}
		case NodeKind_footnote:
		case NodeKind_fencedcode:
		case NodeKind_code:
		case NodeKind_admon: {
			AdmonNode const* admon = static_cast<AdmonNode const*>(node);
			text.append(admon->type);
			text.append("! ");
			_stripAll(text, admon->children);
			break;
		}
		case NodeKind_include:
		case NodeKind_func:
			break;
	}
}

String strip(Node const* node) {
	String text;
	_strip(text, node);
	return text;
}


String stripTitleOnly(HierarchyNode const* titleNode) {
	String text;
	for (int i = 0; i < titleNode->children.size(); i++) {
		Node const* childNode = titleNode->children[i];
		if (childNode->kind == NodeKind_subtitle) {
			break;
		}
		text.append(strip(childNode));
	}
	return text;
}

