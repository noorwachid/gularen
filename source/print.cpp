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
		printf("- kind: %d\n", token.kind);
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
			case NodeKind_paragraph: printf("paragraph"); break;
			case NodeKind_heading: printf("heading"); break;

			case NodeKind_text: printf("text"); break;
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

	void keyArray(char const* key, Array<Node*> nodes) {
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
			case NodeKind_text: {
				TextNode* text = static_cast<TextNode*>(node);
				keyString("content", text->content);
				break;
			}
			case NodeKind_document: {
				DocumentNode* document = static_cast<DocumentNode*>(node);
				keyArray("children", document->children);
				break;
			}
			case NodeKind_paragraph:
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
