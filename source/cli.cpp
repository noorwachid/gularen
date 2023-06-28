#include <Gularen/Parser.h>
#include <fstream>
#include <iostream>

void print(const Gularen::NodePtr& node, size_t depth = 0) {
	std::cout << std::string(depth * 2, ' ') << node->toString() << '\n';

	for (const Gularen::NodePtr& childNode : node->children) {
		print(childNode, depth + 1);
	}
}

int main(int argc, char** argv) {
	if (argc > 1) {
		std::ifstream file(argv[1]);

		if (!file.is_open()) {
			std::cout << "failed to open " << argv[1] << '\n';
			return 0;
		}

		std::string content;
		file.seekg(0, std::ios::end);   
		content.reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		content.assign(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);

		Gularen::Lexer lexer;
		Gularen::Tokens tokens = lexer.parse(content);

		for (const Gularen::Token& token : tokens) {
			if (argc == 2) {
				std::cout << token.toString() << '\n';
			}

			if (argc == 3) {
				if (token.type == Gularen::TokenType::codeSource) {
					std::cout << '\n';

					Gularen::Parser parser;
					Gularen::NodePtr root = parser.parse(token.value);

					print(root, 0);
				}
			}
		}

		if (argc == 2) {
			std::cout << '\n';
			Gularen::Parser parser;
			Gularen::NodePtr root = parser.parse(content);

			print(root, 0);

			return 0;
		}
		return 0;
	}

	std::cout << "please specify the file.gr\n";

	return 0;
}
