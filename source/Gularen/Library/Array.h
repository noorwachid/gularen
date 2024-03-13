#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <new>

namespace Gularen {

template<typename T>
class Array {
public:
	Array() {
		_size = 0;
		_capacity = 0;
		_data = nullptr;
	}

	Array(const Array<T>& other) {
		_size = other._size;
		_data = nullptr;
		_allocate(other._size);
		for (unsigned int i = 0; i < other.size(); i += 1) {
			_data[i] = other.get(i);
		}
	}

	Array(Array<T>&& other) {
		_size = other._size;
		_capacity = other._capacity;
		_data = other._data;
		other._data = nullptr;
	}

	~Array() {
		if (_data) {
			for (unsigned int i = 0; i < _size; i += 1) {
				_data[i].~T();
			}

			free(_data);
			_data = nullptr;
			_size = 0;
		}
	}


	void operator=(const Array<T>& other) {
		_allocate(other.size());

		_size = other._size;
		for (unsigned int i = 0; i < other.size(); i += 1) {
			_data[i] = other.get(i);
		}
	}

	void operator=(Array<T>&& other) {
		this->~Array();

		_size = other._size;
		_capacity = other._capacity;
		_data = other._data;
		other._data = nullptr;
	}

	void append(const T& element) {
		_grow();
		new (_data + _size) T(element);
		_size += 1;
	}

	void append(T&& element) {
		_grow();
		new (_data + _size) T(static_cast<T&&>(element));
		_size += 1;
	}

	void insert(unsigned int index, const T& element) {
		_grow();
		if (_size == 0) {
			return append(element);
		}

		for (unsigned int i = _size; i > index; i -= 1) {
			T* previous = &_data[i - 1];
			memcpy(previous + 1, previous, sizeof(T));
		}

		_data[index] = element;
		_size += 1;
	}

	void insert(unsigned int index, T&& element) {
		_grow();
		if (_size == 0) {
			return append(static_cast<T&&>(element));
		}

		for (unsigned int i = _size; i > index; i -= 1) {
			T* previous = &_data[i - 1];
			memcpy(previous + 1, previous, sizeof(T));
		}

		_data[index] = static_cast<T&&>(element);
		_size += 1;
	}

	T&& drop() {
		assert(_size > 0);
		_size -= 1;
		return static_cast<T&&>(_data[_size]);
	}

	void reserve(unsigned int size) {
		if (_capacity < size) {
			_allocate(size);
		}
	}

	inline const T& get(unsigned int index) const {
		assert(index < _size);

		return _data[index];
	}

	inline T& get(unsigned int index) {
		assert(index < _size);

		return _data[index];
	}

	inline unsigned int size() const {
		return _size;
	}

	const T* pointer() const {
		return _data;
	}

private:
	void _grow() {
		if (_size + 1 > _capacity) {
			_capacity = _capacity < 2 ? 2 : _capacity + _capacity / 2;
			_allocate(_capacity);
		}
	}

	void _allocate(unsigned int size) {
		T* data = static_cast<T*>(malloc(sizeof(T) * size));

		if (_data) {
			unsigned int minSize = (size < _size ? size : _size);
			for (unsigned int i = 0; i < minSize; ++i) {
				new (data + i) T(static_cast<T&&>(_data[i]));
				// _data[i].~T();
			}
			free(_data);
		}

		_data = data;
		_capacity = size;
	}

private:
	unsigned int _size;
	unsigned int _capacity;
	T* _data;
};

}
