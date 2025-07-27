#include "Disk.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace Disk {
	namespace File {
		String read(String const& path) {
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
			FILE* file = fopen(cPath, "r");
			if (path.size() > 255) {
				free(cPath);
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
	}
}

