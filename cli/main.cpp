#include "Gularen/Frontend/Parser.h"

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

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("please specify the file path\n");
		return 1;
	}

	Parser parser;
	Document* document = parser.parseFile(argv[1]);

	print(document);

	return 0;
}
