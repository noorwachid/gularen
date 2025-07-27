#include "print.hpp"
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

