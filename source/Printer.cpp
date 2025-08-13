#include "Printer.hpp"
#include "Lexer.hpp"
#include <stdio.h>

void printString(String const& value) {
	printf("\"");
	for (int i = 0; i < value.size(); i++) {
		switch (value[i]) {
			case '\\':
				printf("\\\\");
				break;
			case '\n':
				printf("\\n");
				break;
			case '\r':
				printf("\\r");
				break;
			case '\t':
				printf("\\t");
				break;
			case '"':
				printf("\\\"");
				break;
			default:
				printf("%c", value[i]);
				break;
		}
	}
	printf("\"");
}

void printRange(Range range) {
	if (range.start.line != range.end.line) {
		printf("%d,%d-%d,%d", 1 + range.start.line, 1 + range.start.column, 1 + range.end.line, 1 + range.end.column);
	} else {
		if (range.start.column != range.end.column) {
			printf("%d,%d-%d", 1 + range.start.line, 1 + range.start.column, 1 + range.end.column);
		} else {
			printf("%d,%d", 1 + range.start.line, 1 + range.start.column);
		}
	}
}

void printArrayToken(Array<Token> const& tokens) {
	for (int i = 0; i < tokens.size(); i++) {
		Token const& token = tokens[i];
		printf("- kind: ");
		switch (token.kind) {
			case TokenKind_text: printf("text"); break;
			case TokenKind_escape: printf("escape"); break;
			case TokenKind_newline: printf("newline"); break;
			case TokenKind_newlines: printf("newlines"); break;
			case TokenKind_indent: printf("indent"); break;
			case TokenKind_outdent: printf("outdent"); break;
			case TokenKind_heading: printf("heading"); break;
			case TokenKind_colon: printf("colon"); break;
			case TokenKind_bullet: printf("bullet"); break;
			case TokenKind_numberpoint: printf("numberpoint"); break;
			case TokenKind_checkbox: printf("checkbox"); break;
			case TokenKind_thematicbreak: printf("thematicbreak"); break;

			case TokenKind_hyphen: printf("hyphen"); break;
			case TokenKind_endash: printf("endash"); break;
			case TokenKind_emdash: printf("emdash"); break;
			case TokenKind_rightquote: printf("rightquote"); break;
			case TokenKind_leftquote: printf("leftquote"); break;
			case TokenKind_singlerightquote: printf("singlerightquote"); break;
			case TokenKind_singleleftquote: printf("singleleftquote"); break;

			case TokenKind_linebreak: printf("linebreak"); break;
			case TokenKind_asterisk: printf("asterisk"); break;
			case TokenKind_underscore: printf("underscore"); break;
			case TokenKind_hashtag: printf("hashtag"); break;
			case TokenKind_emoji: printf("emoji"); break;
			case TokenKind_string: printf("string"); break;

			case TokenKind_openbracket: printf("openbracket"); break;
			case TokenKind_unquotedstring: printf("unquotedstring"); break;
			case TokenKind_closebracket: printf("closebracket"); break;
			case TokenKind_footnote: printf("footnote"); break;
			case TokenKind_openparen: printf("openparen"); break;
			case TokenKind_closeparen: printf("closeparen"); break;

			case TokenKind_openfence: printf("openfence"); break;
			case TokenKind_closefence: printf("closefence"); break;
			case TokenKind_symbol: printf("symbol"); break;
			case TokenKind_sources: printf("sources"); break;
			case TokenKind_backtick: printf("backtick"); break;
			case TokenKind_source: printf("source"); break;

			case TokenKind_pipe: printf("pipe"); break;
			case TokenKind_bar: printf("bar"); break;

			case TokenKind_admon: printf("admon"); break;

			case TokenKind_script: printf("script"); break;
			case TokenKind_func: printf("func"); break;
			case TokenKind_openbrace: printf("openbrace"); break;
			case TokenKind_closebrace: printf("closebrace"); break;

		}
		printf("\n");
		printf("  range: ");
		printRange(token.range);
		printf("\n");
		printf("  content: ");
		printString(token.content);
		printf("\n");
	}
}

struct Printer {
	int depth;
	bool firstItem;

	void indent() {
		if (firstItem) {
			firstItem = false;
			for (int i = 0; i < (depth - 1); i++) {
				printf("  ");
			}
			printf("- ");
			return;
		}
		for (int i = 0; i < depth; i++) {
			printf("  ");
		}
	}

	void metadata(Node* node) {
		indent();
		printf("kind: ");
		switch (node->kind) {
			case NodeKind_document: printf("document"); break;
			case NodeKind_section: printf("section"); break;
			case NodeKind_subsection: printf("subsection"); break;
			case NodeKind_subsubsection: printf("subsubsection"); break;
			case NodeKind_title: printf("title"); break;
			case NodeKind_subtitle: printf("subtitle"); break;
			case NodeKind_paragraph: printf("paragraph"); break;
			case NodeKind_list: printf("list"); break;
			case NodeKind_item: printf("item"); break;
			case NodeKind_numberedlist: printf("numberedlist"); break;
			case NodeKind_numbereditem: printf("numbereditem"); break;
			case NodeKind_checklist: printf("checklist"); break;
			case NodeKind_checkitem: printf("checkitem"); break;
			case NodeKind_quote: printf("quote"); break;
			case NodeKind_thematicbreak: printf("thematicbreak"); break;

			case NodeKind_space: printf("space"); break;
			case NodeKind_hyphen: printf("hyphen"); break;
			case NodeKind_endash: printf("endash"); break;
			case NodeKind_emdash: printf("emdash"); break;
			case NodeKind_rightquote: printf("rightquote"); break;
			case NodeKind_leftquote: printf("leftquote"); break;
			case NodeKind_singlerightquote: printf("singlerightquote"); break;
			case NodeKind_singleleftquote: printf("singleleftquote"); break;

			case NodeKind_text: printf("text"); break;
			case NodeKind_emoji: printf("emoji"); break;
			case NodeKind_strong: printf("strong"); break;
			case NodeKind_emphasis: printf("emphasis"); break;
			case NodeKind_linebreak: printf("linebreak"); break;
			case NodeKind_hashtag: printf("hashtag"); break;

			case NodeKind_link: printf("link"); break;
			case NodeKind_view: printf("view"); break;
			case NodeKind_mention: printf("mention"); break;
			case NodeKind_footnote: printf("footnote"); break;

			case NodeKind_fencedcode: printf("fencedcode"); break;
			case NodeKind_code: printf("code"); break;

			case NodeKind_admon: printf("admon"); break;
			case NodeKind_table: printf("table"); break;
			case NodeKind_row: printf("row"); break;
			case NodeKind_cell: printf("cell"); break;

			case NodeKind_include: printf("include"); break;
			case NodeKind_func: printf("func"); break;
		}
		printf("\n");

		indent();
		printf("range: ");
		printRange(node->range);
		printf("\n");
	}

	void keyBool(char const* key, bool value) {
		indent();
		printf("%s: %s\n", key, value ? "true" : "false");
	}

	void keyString(char const* key, String const& value) {
		indent();
		printf("%s: ", key);
		printString(value);
		printf("\n");
	}

	void keyArray(char const* key, Array<Node*> const& nodes) {
		indent();
		printf("%s:\n", key);
		for (int i = 0; i < nodes.size(); i++) {
			int oldDepth = depth;
			depth += 2;
			firstItem = true;
			table(nodes[i]);
			depth = oldDepth;
		}
	}

	void table(Node* node) {
		if (node == nullptr) {
			return;
		}

		metadata(node);

		switch (node->kind) {
			case NodeKind_text:
			case NodeKind_hashtag:
			case NodeKind_emoji:
			case NodeKind_thematicbreak: {
				ContentNode* text = static_cast<ContentNode*>(node);
				keyString("content", text->content);
				break;
			}
			case NodeKind_document: {
				DocumentNode* document = static_cast<DocumentNode*>(node);
				keyString("path", document->path);
				if (document->metadata.size() != 0) {
					indent();
					printf("metadata:\n");
					int oldDepth = depth;
					depth++;
					for (auto i = document->metadata.iterate(); i.hasNext(); i.next()) {
						indent();
						keyString(i.key().items(), i.value());
					}
					depth = oldDepth;
				}
				keyArray("children", document->children);
				break;
			}
			case NodeKind_checkitem: {
				CheckItemNode* item = static_cast<CheckItemNode*>(node);
				indent();
				printf("isChecked: %s\n", item->isChecked ? "true" : "false");
				keyArray("children", item->children);
				break;
			}
			case NodeKind_section:
			case NodeKind_subsection:
			case NodeKind_subsubsection:
			case NodeKind_title:
			case NodeKind_subtitle:
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
				keyArray("children", h->children);
				break;
			}
			case NodeKind_link:
			case NodeKind_view:
			case NodeKind_mention:
			case NodeKind_footnote: {
				ResourceNode* r = static_cast<ResourceNode*>(node);
				keyString("source", r->source);
				keyArray("children", r->children);
				break;
			}
			case NodeKind_fencedcode:
			case NodeKind_code: {
				CodeNode* c = static_cast<CodeNode*>(node);
				keyString("lang", c->lang);
				keyString("content", c->content);
				break;
			}
			case NodeKind_admon: {
				AdmonNode* a = static_cast<AdmonNode*>(node);
				keyString("type", a->type);
				keyArray("children", a->children);
				break;
			}
			case NodeKind_table: {
				TableNode* t = static_cast<TableNode*>(node);
				keyBool("isHeadered", t->isHeadered);
				indent();
				printf("alignments: ");
				for (int i = 0; i < t->alignments.size(); i++) {
					if (i != 0) {
						printf(", ");
					}
					switch (t->alignments[i]) {
						case Alignment_none: printf("none"); break;
						case Alignment_left: printf("left"); break;
						case Alignment_center: printf("center"); break;
						case Alignment_right: printf("right"); break;
					}
				}
				printf("\n");
				keyArray("children", t->children);
				break;
			}
			case NodeKind_func: {
				FuncNode* func = static_cast<FuncNode*>(node);
				keyString("symbol", func->symbol);
				if (func->arguments.size() != 0) {
					indent();
					printf("arguments:\n");
					int oldDepth = depth;
					depth++;
					for (auto i = func->arguments.iterate(); i.hasNext(); i.next()) {
						indent();
						printf("%.*s: %.*s\n",
							i.key().size(), i.key().items(),
							i.value().size(), i.value().items()
						);
					}
					depth = oldDepth;
				}
				break;
			}
			default:
				return;
		}
	}
};

void printNode(Node* node) {
	Printer printer = {};
	printer.table(node);
}
