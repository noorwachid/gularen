#include "Gularen/Internal/Parser.h"
#include "Gularen/parse.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string toString(const Gularen::NodePtr& node, size_t depth = 0) {
	std::string content;

	if (!node)
		return content;

	content += std::string(depth * 2, ' ') + node->toString() + "\n";

	for (const Gularen::NodePtr& childNode : node->children) {
		content += toString(childNode, depth + 1);
	}

	return content;
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
	if (argc < 2) {
		std::cout << "please specify the input file\n";

		return 0;
	}

	if (!std::filesystem::exists(argv[1])) {
		std::cout << "file does not exist\n";

		return 0;
	}

	Gularen::NodePtr node = Gularen::parseFile(argv[1]);

	if (node->children.size() < 2 || node->children[0]->group != Gularen::NodeGroup::code || node->children[1]->group != Gularen::NodeGroup::code) {
		std::cout << "invalid test file, please make one codeblock for the gularen markup and one for the expected AST\n";
		std::cout << toString(node);

		return 0;
	}

	auto& inputNode = node->children[0]->as<Gularen::CodeNode>();

	Gularen::Parser parser;
	parser.set(inputNode.source, argv[1]);
	parser.parse();

	std::string output = toString(parser.getRoot());
	std::string expectedOutput = node->children[1]->as<Gularen::CodeNode>().source;
	expectedOutput += '\n';

	if (output != expectedOutput) {
		std::cout << "[FAIL] " << argv[1] << "\n";
		std::cout << "output:\n";
		std::cout << output;
		std::cout << "expected output:\n";
		std::cout << expectedOutput;

		return 0;
	}

	std::cout << "[PASS] " << argv[1] << "\n";

	return 0;
}
