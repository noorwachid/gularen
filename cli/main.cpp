#include "gularen/helper/emoji.h"
#include "gularen/parser.h"
#include "gularen/transpiler/html.h"
#include <fstream>
#include <iostream>
#include <sstream>

void build(std::ostream& out, const Gularen::NodePtr& node, size_t depth = 0) {
	if (!node)
		return;

	out << std::string(depth * 2, ' ') << node->toString() << '\n';

	for (const Gularen::NodePtr& childNode : node->children) {
		build(out, childNode, depth + 1);
	}
}

std::string read(const std::string& filePath) {
	std::ifstream file(filePath);
	std::string content;

	file.seekg(0, std::ios::end);
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	return content;
}

int main(int argc, char** argv) {
	if (argc <= 1) {
		std::cout << "please specify the command\n";
		return 0;
	}

	std::string command(argv[1]);

	if (command == "tokenize" && argc > 2) {
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Lexer lexer;
		lexer.set(read(input));
		lexer.tokenize();

		for (const Gularen::Token& token : lexer.get()) {
			std::cout << token.toString() << '\n';
		}

		return 0;
	} else if (command == "parse" && argc > 2) {
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Parser parser;
		parser.set(read(input));
		parser.parse();
		std::ostringstream out;

		build(out, parser.get());

		std::cout << out.str();

		return 0;
	} else if (command == "shortcode-to-emoji" && argc > 2) {
		std::cout << Gularen::Helper::shortcodeToEmoji(argv[2]);
		return 0;
	} else if (command == "get-shortcodes") {
		for (const std::string& shortcode : Gularen::Helper::getShortcodes()) {
			std::cout << shortcode << '\n';
		}
		return 0;
	} else if (command == "to-html") {
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::transpile(read(input));
		return 0;
	} else if (command == "to-html-ls") {
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::transpileSL(read(input));
		return 0;
	} else {
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
