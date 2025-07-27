#include "Collection/IO/Disk.hpp"
#include "lexeme.hpp"
#include "parse.hpp"
#include "print.hpp"
#include <stdio.h>

void compile(String const& path) {
	String content = Disk::File::read(path);
	Array<Token> tokens = lexeme(content);
	printArrayToken(tokens);
	printf("---\n");
	Node* node = parse(content);
	printNode(node);
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
