#include "Conv.hpp"
#include <stddef.h>

int wkConvFromInt(long value, char bytes[20]) {
	int count = 0;
	int temp = value;
	int negative = 0;

	if (value == 0) {
		bytes[0] = '0';
		return 1;
	}

	if (temp < 0) {
		negative = 1;
		temp = -temp;
	}

	while (temp != 0) {
		bytes[count] = temp % 10 + '0';
		temp /= 10;
		count++;
	}

	if (negative) {
		bytes[count] = '-';
		count++;
	}

	for (int i = 0; i < count / 2; i++) {
		char b = bytes[i];
		bytes[i] = bytes[count - i - 1];
		bytes[count - i - 1] = b;
	}

	return count;
}

long wkConvToInt(int byteSize, char const* bytes) {
	if (byteSize == 0) {
		return 0;
	}

	long value = 0;
	int negativeScale = 1;
	int index = 0;

	if (bytes[index] == '-') {
		negativeScale = -1;
		index++;
	}

	while (index < byteSize) {
		if (bytes[index] == '\'') {
			index++;
			continue;
		}

		if (bytes[index] >= '0' && bytes[index] <= '9') {
			value *= 10;
			value += bytes[index] - '0';
			index++;
			continue;
		}

		break;
	}

	return negativeScale * value;
}

long stringToLong(String const& value) {
	return wkConvToInt(value.size(), (char const*) value.items());
}

String longToString(long value) {
	char bytes[20];
	int size = wkConvFromInt(value, bytes);

	String s = String::allocate(size);
	char* b = (char*) s.items();
	for (int i = 0; i < size; i++) {
		b[i] = bytes[i];
	}
	return s;
}
