#include "Gularen/Frontend/Parser.h"
#include "Gularen/Frontend/Node.h"

using namespace Gularen;

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

	if (!std::filesystem::exists(argv[1])) {
		std::cout << "file does not exists\n";
		return 1;
	}

	if (std::filesystem::is_directory(argv[1])) {
		std::cout << "your specified path is a folder\n";
		return 1;
	}

	if (argc < 3) {
		std::cout << "please specify the codeblock index\n";
		return 1;
	}

	std::string content = readFile(argv[1]);
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

	if (std::string_view(argv[2]) == "0") {
		Parser parser;
		std::string_view content = static_cast<CodeBlock*>(document->children[0])->content;
		std::cout << content << "\n";
	} else if (std::string_view(argv[2]) == "1") {
		Parser parser;
		std::string_view content = static_cast<CodeBlock*>(document->children[1])->content;
		std::cout << content << "\n";
	} else {
		std::cout << "index out of bound\n";
	}

	return 0;
}
