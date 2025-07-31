#pragma once

#include "lexeme.hpp"

enum NodeKind {
	NodeKind_document,
	NodeKind_chapter,
	NodeKind_section,
	NodeKind_subsection,
	NodeKind_quote,
	NodeKind_title,
	NodeKind_subtitle,
	NodeKind_paragraph,

	NodeKind_text,
	NodeKind_strong,
	NodeKind_emphasis,
	NodeKind_hashtag,
	NodeKind_emoji,

	NodeKind_link,
	NodeKind_view,
	NodeKind_citation,
	NodeKind_footnote,

	NodeKind_list,
	NodeKind_item,

	NodeKind_numberedlist,
	NodeKind_numbereditem,

	NodeKind_checklist,
	NodeKind_checkitem,

	NodeKind_thematicbreak,
	NodeKind_linebreak,

	NodeKind_hyphen,
	NodeKind_endash,
	NodeKind_emdash,
	NodeKind_rightquote,
	NodeKind_leftquote,
	NodeKind_singlerightquote,
	NodeKind_singleleftquote,

	NodeKind_fencedcode,
	NodeKind_code,

	NodeKind_admon,

	NodeKind_table,
	NodeKind_row,
	NodeKind_cell,
};

struct Node {
	NodeKind kind;
	Range range;

	virtual ~Node() {
	}
};

struct ContentNode: Node {
	String content;
};

struct HierarchyNode: Node {
	Array<Node*> children;

	virtual ~HierarchyNode() {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
	}
};

struct DocumentNode: HierarchyNode {
	String path;
};

struct ResourceNode: HierarchyNode {
	String source;
};

struct CheckItemNode: HierarchyNode {
	bool isChecked;
};

struct AdmonNode: HierarchyNode {
	String type;
};

struct CodeNode: Node {
	String lang;
	String content;
};

enum Alignment {
	Alignment_left,
	Alignment_center,
	Alignment_right,
};

struct TableNode: HierarchyNode {
	Array<Alignment> alignments;
	bool isHeadered = false;
};

Node* parse(String const& source);
