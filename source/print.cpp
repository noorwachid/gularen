#include "print.hpp"
#include "parse.hpp"
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
			case TokenKind_newline: printf("newline"); break;
			case TokenKind_newlines: printf("newlines"); break;
			case TokenKind_indent: printf("indent"); break;
			case TokenKind_outdent: printf("outdent"); break;
			case TokenKind_asterisk: printf("asterisk"); break;
			case TokenKind_underscore: printf("underscore"); break;
			case TokenKind_heading: printf("heading"); break;
			case TokenKind_subheading: printf("subheading"); break;
			case TokenKind_bullet: printf("bullet"); break;
			case TokenKind_numberpoint: printf("numberpoint"); break;
			case TokenKind_checkbox: printf("checkbox"); break;
			case TokenKind_break: printf("break"); break;
			case TokenKind_hashtag: printf("hashtag"); break;
			case TokenKind_emoji: printf("emoji"); break;
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
			case NodeKind_chapter: printf("chapter"); break;
			case NodeKind_section: printf("section"); break;
			case NodeKind_subsection: printf("subsection"); break;
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

			case NodeKind_text: printf("text"); break;
			case NodeKind_emoji: printf("emoji"); break;
			case NodeKind_strong: printf("strong"); break;
			case NodeKind_emphasis: printf("emphasis"); break;
			case NodeKind_hashtag: printf("hashtag"); break;
			case NodeKind_link: printf("link"); break;
			case NodeKind_view: printf("view"); break;
			case NodeKind_citation: printf("citation"); break;
			case NodeKind_footnote: printf("footnote"); break;
		}
		printf("\n");

		indent();
		printf("range: ");
		printRange(node->range);
		printf("\n");
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
			case NodeKind_emoji: {
				TextNode* text = static_cast<TextNode*>(node);
				keyString("content", text->content);
				break;
			}
			case NodeKind_document: {
				DocumentNode* document = static_cast<DocumentNode*>(node);
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
			case NodeKind_chapter:
			case NodeKind_section:
			case NodeKind_subsection:
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
			case NodeKind_emphasis: {
				HierarchyNode* h = static_cast<HierarchyNode*>(node);
				keyArray("children", h->children);
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
