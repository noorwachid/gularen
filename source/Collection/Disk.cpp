#include "Disk.hpp"
#include <stdio.h>
#include <stdlib.h>

FILE* _openFile(String const& path, char const* mode) {
	char cPathStack[256];
	char* cPath;
	if (path.size() > 255) {
		char* cPath = (char*) malloc(path.size() + 1);
	} else {
		cPath = cPathStack;
	}
	for (int i = 0; i < path.size(); i++) {
		cPath[i] = path[i];
	}
	cPath[path.size()] = 0;
	FILE* file = fopen(cPath, mode);
	if (path.size() > 255) {
		free(cPath);
	}
	return file;
}

String readFile(String const& path) {
	FILE* file = _openFile(path, "r");
	if (file == nullptr) {
		return "";
	}
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	String content = String::allocate(size);
	Byte* items = (Byte*) content.items();
	fread(items, sizeof(Byte), size, file);
	fclose(file);
	return content;
}

void writeFile(String const& path, String const& content) {
	FILE* file = _openFile(path, "w");
	if (file == nullptr) {
		return;
	}
	fwrite(content.items(), sizeof(Byte), content.size(), file);
	fclose(file);
}

