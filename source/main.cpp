#include "Collection/Disk.hpp"
#include "lexeme.hpp"
#include "parse.hpp"
#include "print.hpp"
#include <stdio.h>

void compile(String const& path) {
	String content = readFile(path);
	Array<Token> tokens = lexeme(content);
	printf("%.*s\n", content.size(), content.items());
	printf("---\n");
	printArrayToken(tokens);
	printf("---\n");
	Node* node = parseFile(path);
	if (node != nullptr) {
		printNode(node);
		delete node;
	}
}

int main(int argc, char** argv) {
	String path;
	for (int i = 1; i < argc; i++) {
		path = argv[i];
	}
	if (path.size() == 0) {
		return 1;
	}
	compile(path);

	return 0;
}
