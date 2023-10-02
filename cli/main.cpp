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

	if (command == "tokenize") {
		if (argc < 2) {
			std::cout << "please specify the input file\n";
			return 0;
		}

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
	} else if (command == "parse") {
		if (argc < 2) {
			std::cout << "please specify the input file\n";
			return 0;
		}

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
	} else if (command == "html") {
		if (argc < 2) {
			std::cout << "please specify the input file\n";
			return 0;
		}

		std::string input;
		std::string template_;

		for (int i = 2; i < argc; ++i) {
			std::string argument = argv[i];
			if (argument == "--template" && i + 1 < argc) {
				template_ = argv[i + 1];
				continue;
			}

			input = argument;
		}

		if (std::filesystem::is_directory(input)) {
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		if (!template_.empty()) {
			if (std::filesystem::is_directory(template_)) {
				std::cout << "please specify the template file, make sure its not a directory\n";
				return 0;
			}

			std::string templateMarker = "<!-- content -->";
			std::string templateContent = read(template_);

			size_t startPos = templateContent.find(templateMarker);

			if (startPos == std::string::npos) {
				std::cout << "pleace add special comment: '" << templateMarker << "' to the template file\n";
				return 0;
			}

			templateContent.replace(startPos, templateMarker.length(), Gularen::Transpiler::HTML::transpile(read(input)));

			std::cout <<  templateContent;
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::transpile(read(input));
		return 0;
	} else {
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
