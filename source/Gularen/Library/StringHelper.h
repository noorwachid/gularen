#pragma once

#include "Gularen/Library/StringSlice.h"
#include "Gularen/Library/Array.h"

namespace Gularen {

int toInt(StringSlice slice) {
	int value = 0;

	if (slice.size() == 0) {
		return 0;
	}

	unsigned int index = 0;
	int negate = 1;
	int exponent = 10;

	if (slice.get(index) == '-') {
		negate = -1;
		index += 1;
	}

	if (index + 2 < slice.size() && slice.get(index) == '0') {
		index += 1;
		switch (slice.get(index)) {
			case 'b':
				exponent = 2;
				index += 1;
				break;

			case 'o':
				exponent = 8;
				index += 1;
				break;

			case 'x':
				exponent = 16;
				index += 1;
				break;

			default: break;
		}
	}

	while (index < slice.size()) {
		char c = slice.get(index);
		switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				value *= exponent;
				value += c - '0';
				index += 1;
				break;

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				value *= exponent;
				value += c - 'A' + 10;
				index += 1;
				break;

			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				value *= exponent;
				value += c - 'a' + 10;
				index += 1;
				break;

			case '\'':
				index += 1;
				break;

			default: {
				return negate * value;
			}
		}
	}

	return negate * value;
}

Array<StringSlice> splitCSV(StringSlice from) {
	Array<StringSlice> parts;

	unsigned int begin = 0;
	unsigned int i = 0;
	while (i < from.size()) {
		if (from.get(i) == ',') {
			parts.append(from.cut(begin, i - begin));
			i += 1;
			while (i < from.size() && from.get(i) == ' ') {
				i += 1;
			}
			begin = i;
			continue;
		}
		i += 1;
	}

	if (begin < i) {
		parts.append(from.cut(begin, i - begin));
	}

	return parts;
}

Array<StringSlice> splitName(StringSlice name) {
	Array<StringSlice> parts;

	unsigned int begin = 0;
	unsigned int i = 0;
	while (i < name.size()) {
		if (name.get(i) == ' ') {
			if (name.get(begin) >= 'A' && name.get(begin) <= 'Z') {
				parts.append(name.cut(begin, i - begin));
				i += 1;
				while (i < name.size() && name.get(i) == ' ') {
					i += 1;
				}
				begin = i;
				continue;
			}
		}
		i += 1;
	}

	if (begin < i) {
		parts.append(name.cut(begin, i - begin));
	}

	return parts;
}

}
