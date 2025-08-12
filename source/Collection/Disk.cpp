#include "Disk.hpp"
#include <stdio.h>
#include <stdlib.h>

String readFile(String const& path) {
	FILE* file = fopen(path.items(), "r");
	if (file == nullptr) {
		return "";
	}
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	String content = String::allocate(size);
	char* items = (char*) content.items();
	fread(items, sizeof(char), size, file);
	fclose(file);
	return content;
}

void writeFile(String const& path, String const& content) {
	FILE* file = fopen(path.items(), "w");
	if (file == nullptr) {
		return;
	}
	fwrite(content.items(), sizeof(char), content.size(), file);
	fclose(file);
}

