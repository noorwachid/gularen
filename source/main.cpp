#include "Collection/Disk.hpp"
#include "Parser.hpp"
#include "Printer.hpp"
#include "HTMLGen.hpp"
#include <stdio.h>

static constexpr char const* version = "2.0.1";

void compile(String const& inPath, String const& outPath, bool isEmbedded) {
	Node* node = parseFile(inPath);
	if (node != nullptr) {
		String content = genHTML(node);
		delete node;
		if (outPath.size() == 0) {
			printf("%.*s\n", content.size(), content.items());
			return;
		}
		writeFile(outPath, content);
		return;
	}
}

int main(int argc, char** argv) {
	String inPath;
	String outPath;
	bool isEmbedded = false;
	bool showHelp = false;
	bool showVersion = false;
	bool showDebugToken = false;
	bool showDebugTree = false;

	for (int i = 1; i < argc; i++) {
		String arg = argv[i];
		if (arg == "--debug-token") {
			showDebugToken = true;
			continue;
		}
		if (arg == "--debug-tree") {
			showDebugTree = true;
			continue;
		}
		if (arg == "-h" || arg == "--help") {
			showHelp = true;
			continue;
		}
		if (arg == "-v" || arg == "--version") {
			showVersion = true;
			continue;
		}
		if (arg == "-e" || arg == "--embedded") {
			isEmbedded = true;
			continue;
		}
		if (i + 1 < argc && (arg == "-o" || arg == "--output")) {
			outPath = argv[i + 1];
			i++;
			continue;
		}
		inPath = argv[i];
	}

	if (showHelp) {
		printf("-h\n");
		printf("--help\n");
		printf("	display help page\n");

		printf("-v\n");
		printf("--version\n");
		printf("	display version\n");

		printf("-e\n");
		printf("--embedded\n");
		printf("	render without header\n");

		printf("-o index.html\n");
		printf("--output index.html\n");
		printf("	render to file index.html\n");
		return 0;
	}

	if (showDebugToken) {
		String const& content = readFile(inPath);
		printArrayToken(lexeme(content));
		return 0;
	}

	if (showDebugTree) {
		printNode(parseFile(inPath));
		return 0;
	}

	if (showVersion) {
		printf("%s\n", version);
		return 0;
	}

	if (inPath.size() == 0) {
		printf("please provide the input path\n");
		return 0;
	}

	compile(inPath, outPath, isEmbedded);

	return 0;
}
