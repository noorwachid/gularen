#include "Gularen/Frontend/Parser.h"
#include "Gularen/Frontend/Node.h"
#include "Gularen/Library/String.h"
#include <stdio.h>

using namespace Gularen;

void print(Slice<Token> tokens) {
	for (unsigned int i = 0; i < tokens.size(); i += 1) {
		Position position = tokens.get(i).position;
		printf("%d:%d ", position.line, position.column);
		tokens.get(i).print();
	}
}

void print(Node* node, unsigned int depth = 0) {
	for (unsigned int i = 0; i < depth; i += 1) {
		printf("  ");
	}
	node->print();
	for (unsigned int i = 0; i < node->children.size(); i += 1) {
		print(node->children.get(i), depth + 1);
	}
}

String readFile(const char* path) {
	String content;
	FILE* file = fopen(path, "r");

	if (file == nullptr) {
		printf("failed to open file %s\n", path);
		return String();
	}

	fseek(file, 0, SEEK_END);
	char* data = content.expand(ftell(file));
	fseek(file, 0, SEEK_SET);

	fread(data, content.size(), sizeof(char), file);
	fclose(file);

	return content;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("please specify the file path\n");
		return 1;
	}

	String content = readFile(argv[1]);
	StringSlice contentSlice(content.pointer(), content.size());

	Lexer lexer;
	Slice<Token> tokens = lexer.parse(contentSlice);
	print(tokens);
	printf("\n");

	Parser parser;
	Slice<Node*> nodes = parser.parse(contentSlice);

	for (unsigned int i = 0; i < nodes.size(); i += 1) {
		print(nodes.get(i));
	}

	return 0;
}
