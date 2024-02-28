#pragma once

#include <stdlib.h>
#include <string.h>

namespace Gularen {

template<typename T>
class Slice {
public:
	Slice() {
		_size = 0;
		_data = nullptr;
	}

	Slice(const T* data) {
		_size = strlen(data);
		_data = data;
	}

	Slice(const T* data, unsigned int size) {
		_size = size;
		_data = data;
	}

	Slice(const Slice& other) {
		_size = other._size;
		_data = other._data;
	}

	void operator=(const Slice& other) {
		_data = other._data;
		_size = other._size;
	}

	inline const T& get(unsigned int index) const {
		return _data[index];
	}

	inline unsigned int size() const {
		return _size;
	}

	inline const T* data() const {
		return _data;
	}

	Slice<T> cut(unsigned int index, unsigned int size) const {
		return Slice<T>(_data + index, size);
	}

	friend bool operator==(const Slice& a, const Slice& b) {
		if (a._size != b._size) {
			return false;
		}

		unsigned int i = 0;
		for (; i < a._size && a._data[i] == b._data[i]; ++i);
		return i == a._size;
	}

private:
	unsigned int _size;
	const T* _data;
};

}
