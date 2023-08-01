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

int main(int argc, char** argv) {
	if (argc <= 1) {
		std::cout << "please specify the input file\n";
		return 0;
	}

	if (std::filesystem::is_directory(argv[1])) {
		std::cout << "please specify the input file, make sure its not a directory\n";
		return 0;
	}

	std::ifstream file(argv[1]);
	std::string content;
	file.seekg(0, std::ios::end);   
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	if (argc == 3) {
		if (argv[2][0] == 'x') {
			Gularen::Lexer lexer;
			lexer.set(content);
			lexer.tokenize();

			for (const Gularen::Token& token : lexer.get()) {
				std::cout << token.toString() << '\n';
			}

			return 0;
		}

		if (argv[2][0] == 'y') {
			Gularen::Parser parser;
			parser.set(content);
			parser.parse();
			std::ostringstream out;

			build(out, parser.get());

			std::cout << out.str();

			return 0;
		}
	}

	std::string out = Gularen::Transpiler::HTML::transpile(content);
	std::cout << out << '\n';

	return 0;
}
