#include "Gularen/Frontend/Parser.h"
#include "Gularen/Backend/Html/Composer.h"
#include "Gularen/Backend/Html/TemplateManager.h"
#include "Gularen/Backend/Gfm/Composer.h"
#include "Gularen/Backend/Json/Composer.h"

using namespace Gularen;

void print(Node* node, size_t depth = 0) {
	for (size_t i = 0; i < depth; i += 1) {
		std::cout << "  ";
	}
	std::cout << (node->range.startLine + 1) << "," << (node->range.startColumn + 1) << "-";
	std::cout << (node->range.endLine + 1) << "," << (node->range.endColumn + 1) << " ";
	node->print();

	for (size_t j = 0; j < node->annotations.size(); j += 1) {
		for (size_t i = 0; i < depth + 1; i += 1) {
			std::cout << "  ";
		}
		const Annotation& annotation = node->annotations[j];
		std::cout << "~~ " << annotation.key << ": " << annotation.value << "\n";
	}

	for (size_t i = 0; i < node->children.size(); i += 1) {
		print(node->children[i], depth + 1);
	}
}

int main(int argc, char** argv) {
	if (argv == 0) {
		std::cout << "invalid process\n";
		return 1;
	}

	std::string_view program = argv[0];

	if (argc < 2) {
		std::cout << "please specify the action\n";
		std::cout << "  " << program << " [help|to|parse]\n";
		return 1;
	}

	std::string_view action = argv[1];

	if (action == "help") {
		std::cout << program << " help\n";
		std::cout << "  display help page\n\n";
		std::cout << program << " to target input-path.gr\n";
		std::cout << "  convert specified input path to specified target\n";
		std::cout << "  target can be one of:\n";
		std::cout << "    - html\n";
		std::cout << "    - gfm (github flavored markdown)\n";
		std::cout << "    - json\n";
		std::cout << "\n";
		std::cout << program << " parse input-path.gr\n";
		std::cout << "  convert specified input path to abstract syntax tree\n\n";
		return 0;
	}

	if (action == "to") {
		if (argc < 3) {
			std::cout << "please specify the target\n";
			std::cout << "  " << program << " to target [options] input-path.gr\n";

			return 1;
		}

		std::string_view target = argv[2];
		std::string_view outputPath;
		std::string_view inputPath;
		std::string_view templatePath;

		for (int i = 3; i < argc; i += 1) {
			if (i + 1 < argc) {
				if (std::string_view("--output") == argv[i]) {
					outputPath = argv[i + 1];
					i += 1;
					continue;
				}
				if (std::string_view("--input") == argv[i]) {
					inputPath = argv[i + 1];
					i += 1;
					continue;
				}
				if (std::string_view("--template") == argv[i]) {
					templatePath = argv[i + 1];
					i += 1;
					continue;
				}
			}

			inputPath = argv[i];
		}

		if (inputPath.size() == 0) {
			std::cout << "please specify the input path\n";
			std::cout << "  " << program << " to target [options] input-path.gr\n";
			return 1;
		}

		if (!std::filesystem::exists(inputPath)) {
			std::cout << "input \"" << inputPath << "\" does not exist\n";
			return 0;
		}

		Parser parser;
		Document* document = parser.parseFile(inputPath);

		if (document == nullptr) {
			std::cout << "failed to parse \"" << inputPath << "\"\n";

			return 1;
		}

		if (target == "html") {
			if (templatePath.size() != 0) {
				if (!std::filesystem::exists(templatePath)) {
					std::cout << "template \"" << templatePath << "\" does not exist\n";
					return 0;
				}

				Html::TemplateManager templateManager;
				templateManager.setDocument(document);
				templateManager.setTemplateFile(templatePath);
				std::cout << templateManager.render();
				return 0;
			}

			Html::Composer composer;
			std::cout << composer.compose(document);

			return 0;
		}

		if (target == "gfm") {
			Gfm::Composer composer;
			std::cout << composer.compose(document);

			return 0;
		}

		if (target == "json") {
			Json::Composer composer;
			std::cout << composer.compose(document);

			return 0;
		}

		std::cout << "unknown target\n";
		return 1;
	}

	if (action == "parse") {
		if (argc < 3) {
			std::cout << "please specify the input path\n";
			std::cout << "  " << program << " parse input-path.gr\n";

			return 1;
		}

		Parser parser;
		Document* document = parser.parseFile(argv[2]);

		if (document == nullptr) {
			std::cout << "failed to parse \"" << argv[2] << "\"\n";

			return 1;
		}

		print(document);
		return 0;
	}

	std::cout << "unknown action\n";
	return 0;
}
