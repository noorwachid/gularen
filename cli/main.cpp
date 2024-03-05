#include "Gularen/Frontend/Parser.h"
#include "Gularen/Backend/Html/Transpiler.h"

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
		printf("please specify the action\n");
		printf("  %s [to|parse]\n", argv[0]);
		return 1;
	}

	StringSlice action = argv[1];

	if (action == "to") {
		if (argc < 4) {
			printf("please specify the target and file path\n");
			printf("  %s to [html|md] file-path.gr\n", argv[0]);

			return 1;
		}

		Parser parser;
		Document* document = parser.parseFile(argv[3]);

		if (document == nullptr) {
			printf("failed to parse %s\n", argv[3]);

			return 1;
		}

		StringSlice target = argv[2];

		if (target == "html") {
			Html::Transpiler transpiler;
			StringSlice content = transpiler.transpile(document);

			printf("%.*s", content.size(), content.pointer());

			return 0;
		}

		printf("unknown target\n");
		return 1;
	}

	if (action == "parse") {
		if (argc < 3) {
			printf("please specify the file path\n");
			printf("  %s parse file-path.gr\n", argv[0]);

			return 1;
		}

		Parser parser;
		Document* document = parser.parseFile(argv[2]);

		if (document == nullptr) {
			printf("failed to parse %s\n", argv[2]);

			return 1;
		}

		print(document);
		return 0;
	}

	printf("unknown action\n");
	return 0;
}
