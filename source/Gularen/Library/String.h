#pragma once

#include "Gularen/Library/Slice.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace Gularen {

class String {
public:
	String() {
		_data = nullptr;
		_size = 0;
		_capacity = 0;
	}

	String(const String& other) {
		_data = nullptr;
		_size = other._size;
		_allocate(other._size);
		memcpy(_data, other._data, _size);
	}

	String(String&& other) {
		_data = other._data;
		_size = other._size;
		_capacity = other._capacity;
		other._data = nullptr;
	}

	explicit String(const char* data) {
		_size = strlen(data);
		_capacity = _size;
		_data = static_cast<char*>(malloc(_size));

		memcpy(_data, data, _size);
	}

	explicit String(const char* data, unsigned int size) {
		_size = size;
		_capacity = _size;
		_data = static_cast<char*>(malloc(_size));

		memcpy(_data, data, _size);
	}

	~String() {
		if (_data) {
			free(_data);
			_data = nullptr;
		}
	}

	void operator=(const String& other) {
		this->~String();

		_size = other._size;
		_allocate(other._size);

		for (unsigned int i = 0; i < other._size; ++i) {
			_data[i] = other._data[i];
		}
	}

	void operator=(String&& other) {
		this->~String();

		_data = other._data;
		_size = other._size;
		_capacity = other._capacity;
		other._data = nullptr;
	}

	void reserve(unsigned int size) {
		if (_capacity < _size) {
			_allocate(size);
		}
	}

	char* expand(unsigned int size) {
		_allocate(size);
		_size = size;
		return _data;
	}

	void append(char byte) {
		_grow();
		_data[_size] = byte;
		++_size;
	}

	void append(const String& other) {
		for (unsigned int i = 0; i < other.size(); i += 1) {
			append(other.get(i));
		}
	}

	void append(const char* other) {
		unsigned int size = strlen(other);
		for (unsigned int i = 0; i < size; i += 1) {
			append(other[i]);
		}
	}

	void append(const char* other, unsigned int size) {
		for (unsigned int i = 0; i < size; i += 1) {
			append(other[i]);
		}
	}

	inline char get(unsigned int index) const {
		return _data[index];
	}

	inline void set(unsigned int index, char element) {
		_data[index] = element;
	}

	inline unsigned int size() const {
		return _size;
	}

	inline const char* pointer() const {
		return _data;
	}

	friend bool operator==(const String& a, const String& b) {
		if (a._size != b._size) {
			return false;
		}
		
		unsigned int i = 0;
		for (; i < a._size && a._data[i] == b._data[i]; ++i);
		return i == a._size;
	}

	friend String operator+(const String& a, const String& b) {
		String c = a;
		for (unsigned int i = 0 ; i < b.size(); i += 1) {
			c.append(b.get(i));
		}
		return c;
	}

	static String fromInt(int value) {
		String c;
		char* data = c.expand(11);
		c._size = snprintf(data, 11, "%d", value);
		return c;
	}

private:
	void _grow() {
		if (_size + 1 > _capacity) {
			_capacity = _capacity < 2 ? 2 : _capacity + _capacity / 2;
			_allocate(_capacity);
		}
	}

	void _allocate(unsigned int size) {
		char* data = static_cast<char*>(malloc(size));

		if (_data) {
			memcpy(data, _data, (size < _size ? size : _size));
			free(_data);
		}
		_data = data;
		_capacity = size;
	}

private:
	unsigned int _size;
	unsigned int _capacity;
	char* _data;
};

}
