#include "Gularen/Frontend/Parser.h"
#include "Gularen/Backend/Html/Composer.h"
#include "Gularen/Backend/Html/TemplateManager.h"
#include "Gularen/Backend/Gfm/Composer.h"

using namespace Gularen;

void print(Node* node, unsigned int depth = 0) {
	for (unsigned int i = 0; i < depth; i += 1) {
		printf("  ");
	}
	node->print();

	for (unsigned int j = 0; j < node->annotations.size(); j += 1) {
		for (unsigned int i = 0; i < depth + 1; i += 1) {
			printf("  ");
		}
		const Annotation& annotation = node->annotations.get(j);
		printf("~~ %.*s: %.*s\n", 
			annotation.key.size(), annotation.key.pointer(), 
			annotation.value.size(), annotation.value.pointer()
		);
	}

	for (unsigned int i = 0; i < node->children.size(); i += 1) {
		print(node->children.get(i), depth + 1);
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("please specify the action\n");
		printf("  %s [help|to|parse]\n", argv[0]);
		return 1;
	}

	StringSlice action = argv[1];

	if (action == "help") {
		printf("%s help\n", argv[0]);
		printf("  display help page\n\n");
		printf("%s to target input-path.gr\n", argv[0]);
		printf("  convert specified input path to specified target\n");
		printf("  target can be one of:\n");
		printf("    - html\n");
		printf("    - gfm (github flavored markdown)\n");
		printf("\n");
		printf("%s parse input-path.gr\n", argv[0]);
		printf("  convert specified input path to abstract syntax tree\n\n");
		return 0;
	}

	if (action == "to") {
		if (argc < 3) {
			printf("please specify the target\n");
			printf("  %s to target [options] input-path.gr\n", argv[0]);

			return 1;
		}

		StringSlice target = argv[2];
		StringSlice outputPath;
		StringSlice inputPath;
		StringSlice templatePath;

		for (int i = 3; i < argc; i += 1) {
			if (i + 1 < argc) {
				if (StringSlice("--output") == argv[i]) {
					outputPath = argv[i + 1];
					i += 1;
					continue;
				}
				if (StringSlice("--input") == argv[i]) {
					inputPath = argv[i + 1];
					i += 1;
					continue;
				}
				if (StringSlice("--template") == argv[i]) {
					templatePath = argv[i + 1];
					i += 1;
					continue;
				}
			}

			inputPath = argv[i];
		}

		if (inputPath.size() == 0) {
			printf("please specify the input path\n");
			printf("  %s to target [options] input-path.gr\n", argv[0]);
			return 1;
		}

		if (!Disk::File::exists(inputPath)) {
			printf("input \"%.*s\" does not exist\n", inputPath.size(), inputPath.pointer());
			return 0;
		}

		Parser parser;
		Document* document = parser.parseFile(inputPath);

		if (document == nullptr) {
			printf("failed to parse %.*s\n", inputPath.size(), inputPath.pointer());

			return 1;
		}

		if (target == "html") {
			if (templatePath.size() != 0) {
				if (!Disk::File::exists(templatePath)) {
					printf("template \"%.*s\" does not exist\n", templatePath.size(), templatePath.pointer());
					return 0;
				}

				Html::TemplateManager templateManager;
				templateManager.setDocument(document);
				templateManager.setTemplateFile(templatePath);
				StringSlice content = templateManager.render();

				printf("%.*s", content.size(), content.pointer());
				return 0;
			}

			Html::Composer transpiler;
			StringSlice content = transpiler.compose(document);
			printf("%.*s", content.size(), content.pointer());

			return 0;
		}

		if (target == "gfm") {
			Gfm::Composer transpiler;
			StringSlice content = transpiler.compose(document);

			printf("%.*s", content.size(), content.pointer());

			return 0;
		}

		printf("unknown target\n");
		return 1;
	}

	if (action == "parse") {
		if (argc < 3) {
			printf("please specify the input path\n");
			printf("  %s parse input-path.gr\n", argv[0]);

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
