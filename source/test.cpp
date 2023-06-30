#include <Gularen/Parser.h>
#include <fstream>
#include <iostream>
#include <sstream>

void build(std::ostream& out, const Gularen::NodePtr& node, size_t depth = 0) {
	if (!node) return;

	out << std::string(depth * 2, ' ') << node->toString() << '\n';

	for (const Gularen::NodePtr& childNode : node->children) {
		build(out, childNode, depth + 1);
	}
}

void buildText(std::ostream& out, const Gularen::NodePtr& node) {
	if (!node) return;

	if (node->group == Gularen::NodeGroup::text) {
		out << static_cast<Gularen::TextNode*>(node.get())->value;
	}

	for (const Gularen::NodePtr& childNode : node->children) {
		buildText(out, childNode);
	}
}

void print(const Gularen::NodePtr& node) {
	build(std::cout, node);
}

int main(int argc, char** argv) {
	if (argc > 1) {
		std::string prevSection;
		std::string prevCode;

		bool detailed = argc > 2 && argv[2] == std::string("detailed");
		bool showTokens = argc > 2 && argv[2] == std::string("show-tokens");

		if (showTokens) {
			Gularen::Lexer lexer;
			lexer.load(argv[1]);
			lexer.parse();
			for (const Gularen::Token& token : lexer.get()) {
				std::cout << token.toString() << '\n';
			}
			return 0;
		}

		Gularen::Parser parser;
		parser.load(argv[1]);
		parser.parse();
		parser.visit([&prevSection, &prevCode, argv, detailed](const Gularen::NodePtr& node) {
			if (node->group == Gularen::NodeGroup::heading) {
				Gularen::HeadingType type = static_cast<Gularen::HeadingNode*>(node.get())->type;
				std::ostringstream out;
				buildText(out, node);

				if (type == Gularen::HeadingType::chapter) {
					std::cout << out.str() << '\n';
					std::cout << std::string(out.str().size(), '=') << '\n';
				} else if (type == Gularen::HeadingType::section) {
					prevSection = out.str();
				}

				return;
			}

			if (node->group == Gularen::NodeGroup::code) {
				Gularen::CodeNode* codeNode = static_cast<Gularen::CodeNode*>(node.get());

				if (codeNode->lang == "gr") {
					Gularen::Parser parser;
					parser.set(codeNode->source, argv[1]);
					parser.parse();

					std::ostringstream out;
					build(out, parser.get());
					prevCode = out.str();
					return;
				}

				if (codeNode->lang == "yaml" && !prevCode.empty()) {
					prevCode.pop_back();
					bool passed = prevCode == codeNode->source;

					std::cout << '\n' << (passed ? "[\x1B[38:5:10mPASS\x1B[0m]" : "[\x1B[38:5:9mFAILED\x1B[0m]") << ' ' << prevSection << '\n';

					if (detailed) {
						std::cout << "\x1B[38:5:247m--- eval\n";
						std::cout << "\x1B[38:5:214m" << prevCode << '\n';
						std::cout << "\x1B[38:5:247m--- expect\n";
						std::cout << "\x1B[38:5:214m" << codeNode->source << '\n';
						std::cout << "\x1B[38:5:247m---\x1B[0m\n";
					}
				}

				prevCode.clear();
				return;
			}
		});

		return 0;
	}

	std::cout << "please specify the file.gr\n";

	return 0;
}
