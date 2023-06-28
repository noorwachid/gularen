#include <Gularen/Lexer.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
	if (argc == 2) {
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
			std::cout << static_cast<int>(token.type) << '\t' << token.count << '\t' << token.value << '\n';
		}

		for (const Gularen::Token& token : tokens) {
			if (token.type == Gularen::TokenType::codeSource) {
				std::cout << '\n';

				Gularen::Tokens tokens = lexer.parse(token.value);

				for (const Gularen::Token& token : tokens) {
					std::cout << static_cast<int>(token.type) << '\t' << token.count << '\t' << token.value << '\n';
				}
			}
		}
		return 0;
	}

	std::cout << "please specify the file.gr\n";

	return 0;
}
