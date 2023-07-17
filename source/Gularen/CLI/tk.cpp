#include <Gularen/Parser.h>
#include <Gularen/Helper/emoji.h>
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

std::string getContent(const std::string& filePath) {
	std::ifstream file(filePath);
	std::string content;

	file.seekg(0, std::ios::end);   
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	return content;
}


int main(int argc, char** argv) {
	if (argc <= 2) {
		std::cout << "please specify the command and input file\n";
		return 0;
	}

	std::string command(argv[1]);
	std::string input(argv[2]);

	if (command == "tokenize") {
		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Lexer lexer;
		lexer.set(getContent(input));
		lexer.tokenize();

		for (const Gularen::Token& token : lexer.get()) {
			std::cout << token.toString() << '\n';
		}

		return 0;

	} else if (command == "parse") {
		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Parser parser;
		parser.set(getContent(input));
		parser.parse();
		std::ostringstream out;

		build(out, parser.get());

		std::cout << out.str();

		return 0;
	} else if (command == "to-emoji") {
		std::cout << Gularen::Helper::toEmoji(input);
		return 0;
	} else {
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
