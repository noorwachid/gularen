#pragma once

#include <stdlib.h>
#include <string.h>

namespace Gularen {

class StringSlice {
public:
	StringSlice() {
		_size = 0;
		_data = nullptr;
	}

	StringSlice(const char* data) {
		_size = strlen(data);
		_data = data;
	}

	StringSlice(const char* data, unsigned int size) {
		_size = size;
		_data = data;
	}

	StringSlice(const StringSlice& other) {
		_size = other._size;
		_data = other._data;
	}

	void operator=(const StringSlice& other) {
		_data = other._data;
		_size = other._size;
	}

	inline char get(unsigned int index) const {
		return _data[index];
	}

	inline unsigned int size() const {
		return _size;
	}

	inline const char* pointer() const {
		return _data;
	}

	StringSlice cut(unsigned int index, unsigned int size) const {
		return StringSlice(_data + index, size);
	}

	friend bool operator==(const StringSlice& a, const StringSlice& b) {
		if (a._size != b._size) {
			return false;
		}

		unsigned int i = 0;
		for (; i < a._size && a._data[i] == b._data[i]; ++i);
		return i == a._size;
	}

private:
	unsigned int _size;
	const char* _data;
};

}

