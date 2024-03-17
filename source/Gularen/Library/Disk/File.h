#pragma once

#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"
#include <stdio.h>

namespace Gularen {
namespace Disk {

class File {
public:
	static bool exists(StringSlice path) {
		char pathC[512];
		unsigned int pathSize = path.size() < 511 ? path.size() : 511;
		memcpy(pathC, path.pointer(), pathSize);
		pathC[pathSize] = 0;

		FILE* file = fopen(pathC, "r");

		if (file == nullptr) {
			return false;
		}

		fclose(file);

		return true;
	}

	static String readAll(StringSlice path) {
		String content;

		char pathC[512];
		unsigned int pathSize = path.size() < 511 ? path.size() : 511;
		memcpy(pathC, path.pointer(), pathSize);
		pathC[pathSize] = 0;

		FILE* file = fopen(pathC, "r");

		if (file == nullptr) {
			return content;
		}

		fseek(file, 0, SEEK_END);
		char* data = content.expand(ftell(file));
		fseek(file, 0, SEEK_SET);

		fread(data, content.size(), sizeof(char), file);
		fclose(file);

		return content;
	}
};

}
}
