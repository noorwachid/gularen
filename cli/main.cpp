#include "Gularen/parse.h"
#include "Gularen/transpile.h"
#include "Gularen/Internal/Helper/emoji.h"
#include "Gularen/Internal/Lexer.h"

#include <fstream>
#include <iostream>
#include <sstream>

void toString(std::ostream& out, const Gularen::NodePtr& node, size_t depth = 0) {
	if (!node)
		return;

	out << std::string(depth * 2, ' ') << node->toString() << '\n';

	for (const Gularen::NodePtr& childNode : node->children) {
		toString(out, childNode, depth + 1);
	}
}

std::string readFile(const std::string& filePath) {
	std::ifstream file(filePath);
	std::string content;

	file.seekg(0, std::ios::end);
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	return content;
}

int main(int argc, char** argv) {
	std::string version = "2.0.1";

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
		lexer.set(readFile(input));
		lexer.parse();

		for (const Gularen::Token& token : lexer.getTokens()) {
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

		std::ostringstream out;

		toString(out, Gularen::parse(readFile(input)));

		std::cout << out.str();

		return 0;
	} else if (command == "transpile") {
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
			std::string templateContent = readFile(template_);

			size_t startPos = templateContent.find(templateMarker);

			if (startPos == std::string::npos) {
				std::cout << "pleace add special comment: '" << templateMarker << "' to the template file\n";
				return 0;
			}

			templateContent.replace(
				startPos, templateMarker.length(), Gularen::transpile(readFile(input))
			);

			std::cout << templateContent;
			return 0;
		}

		std::cout << Gularen::transpile(readFile(input));
		return 0;
	} else if (command == "version") {
		std::cout << version << "\n";
	} else {
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
