#include "Collection/Disk.hpp"
#include "Parser.hpp"
#include "Printer.hpp"
#include "HtmlGen.hpp"
#include <stdio.h>

static constexpr char const* version = "2.0.1";

void compile(String const& path, Table<String, String> const& option) {
	Node* node = parseFile(path);
	if (node != nullptr) {
		HtmlGen::Option htmlOption;

		for (auto it = option.iterate(); it.hasNext(); it.next()) {
			if (it.key() == "template") {
				htmlOption.template_ = it.value();
			}
		}

		String content = HtmlGen::gen(node, htmlOption);
		delete node;

		if (option.has("out")) {
			writeFile(option["out"], content);
			return;
		}
		printf("%s\n", content.items());
		return;
	}
}

int main(int argc, char** argv) {
	String path;
	Table<String, String> option;

	for (int i = 1; i < argc; i++) {
		String arg = argv[i];
		if (arg == "--debug-token") {
			option.set("debug-token", "1");
			continue;
		}
		if (arg == "--debug-tree") {
			option.set("debug-tree", "1");
			continue;
		}
		if (arg == "-h" || arg == "--help") {
			option.set("help", "1");
			continue;
		}
		if (arg == "-v" || arg == "--version") {
			option.set("version", "1");
			continue;
		}
		if (i + 1 < argc && (arg == "-t" || arg == "--template")) {
			option.set("template", argv[i + 1]);
			i++;
			continue;
		}
		if (i + 1 < argc && (arg == "-o" || arg == "--out")) {
			option.set("out", argv[i + 1]);
			i++;
			continue;
		}
		path = argv[i];
	}

	if (option.has("help")) {
		printf("-h\n");
		printf("--help\n");
		printf("	display help page\n");

		printf("-v\n");
		printf("--version\n");
		printf("	display version\n");

		printf("-o index.html\n");
		printf("--output index.html\n");
		printf("	render to file index.html\n");
		return 0;
	}

	if (option.has("debug-token")) {
		String const& content = readFile(path);
		printArrayToken(lexeme(content));
		return 0;
	}

	if (option.has("debug-tree")) {
		printNode(parseFile(path));
		return 0;
	}

	if (option.has("version")) {
		printf("%s\n", version);
		return 0;
	}

	if (path.size() == 0) {
		printf("please provide the input path\n");
		return 0;
	}

	compile(path, option);

	return 0;
}
