#include "Gularen/Frontend/Parser.h"
#include "Gularen/Frontend/Node.h"

using namespace Gularen;

void print(Node* node, unsigned int depth = 0) {
	for (unsigned int i = 0; i < depth; i += 1) {
		std::cout << "  ";
	}
	node->print();
	for (unsigned int i = 0; i < node->children.size(); i += 1) {
		print(node->children[i], depth + 1);
	}
}

std::string readFile(const char* path) {
	std::string content(std::filesystem::file_size(path), '\0');
	std::ifstream file(path);
	file.read(content.data(), content.size());
	return content;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "please specify the file path\n";
		return 1;
	}

	if (argc < 3) {
		std::cout << "please specify the action [parse|expect]\n";
		return 1;
	}

	std::string content = readFile(argv[2]);
	std::string_view contentSlice(content.data(), content.size());

	Parser parser;
	Document* document = parser.parse(contentSlice);

	if (document->children.size() < 2 || 
		document->children[0]->kind != NodeKind::codeBlock ||
		document->children[1]->kind != NodeKind::codeBlock
	) {
		std::cout << "invalid test file\n";
		return 0;
	}

	if (std::string_view(argv[1]) == "parse") {
		Parser parser;
		std::string_view content = static_cast<CodeBlock*>(document->children[0])->content;
		Document* contentDocument = parser.parse(content);
		if (contentDocument != nullptr) {
			print(contentDocument);
		}
	}

	if (std::string_view(argv[1]) == "expect") {
		Parser parser;
		std::string_view content = static_cast<CodeBlock*>(document->children[1])->content;
		std::cout << content << "\n";
	}

	return 0;
}
