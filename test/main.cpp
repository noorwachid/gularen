#include "Gularen/Frontend/Parser.h"
#include "Gularen/Frontend/Node.h"
#include "Gularen/Library/String.h"
#include <stdio.h>

using namespace Gularen;

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

	if (argc < 3) {
		printf("please specify the action [parse|expect]\n");
		return 1;
	}

	String content = readFile(argv[2]);
	StringSlice contentSlice(content.pointer(), content.size());

	Parser parser;
	Document* document = parser.parse(contentSlice);

	if (document->children.size() < 2 || 
		document->children.get(0)->kind != NodeKind::codeBlock ||
		document->children.get(1)->kind != NodeKind::codeBlock
	) {
		printf("invalid test file\n");
		return 0;
	}

	if (StringSlice(argv[1]) == "parse") {
		Parser parser;
		StringSlice content = static_cast<CodeBlock*>(document->children.get(0))->content;
		Document* contentDocument = parser.parse(content);
		if (contentDocument != nullptr) {
			print(contentDocument);
		}
	}

	if (StringSlice(argv[1]) == "expect") {
		Parser parser;
		StringSlice content = static_cast<CodeBlock*>(document->children.get(1))->content;
		printf("%.*s\n", content.size(), content.pointer());
	}

	return 0;
}
