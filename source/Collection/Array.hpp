#pragma once

#include <new>
#include "Byte.hpp"

template <typename T>
class Array {
	struct Header {
		int count;
	};

public:
	Array() {
		_size = 0;
		_capacity = 0;
		_items = nullptr;
	}

	Array(Array const& other) {
		_addRef(other);
	}

	~Array() {
		_removeRef();
	}

	void operator=(Array const& other) {
		_removeRef();
		_addRef(other);
	}

	T const& operator[](int index) const {
		return _items[index];
	}

	void set(int index, T const& item) {
		_checkOwnership();

		_items[index] = item;
	}

	int size() const {
		return _size;
	}

	T const* items() const {
		return _items;
	}

	void append(T const& item) {
		_checkOwnership();
		_checkCapacity();

		new (_items + _size) T(item);
		_size++;
	}

	void insert(int index, T const& item) {
		_checkOwnership();
		_checkCapacity();
		_size++;
		Byte* bytes = (Byte*) _items;
		for (int i = (_size * sizeof(T)) - 1; i >= (index * sizeof(T)); i--) {
			bytes[i] = bytes[i - sizeof(T)];
		}
		new (_items + index) T(item);
	}

	void remove(int index) {
		_checkOwnership();
		_items[index].~T();
		Byte* bytes = (Byte*) _items;
		for (int i = index * sizeof(T); i < ((_size - 1) * sizeof(T)); i++) {
			bytes[i] = bytes[i + sizeof(T)];
		}
		_size--;
	}

private:
	void _copy(int size, T* dest, T* source) {
		Byte* destBytes = (Byte*) dest;
		Byte* sourceBytes = (Byte*) source;

		for (int i = 0; i < size * sizeof(T); i++) {
			destBytes[i] = sourceBytes[i];
		}
	}

	void _checkOwnership() {
		if (_items != nullptr && _header()->count > 1) {
			int size = _size;
			T* items = _create(size);
			for (int i = 0; i < size; i++) {
				new (items + i) T(_items[i]);
			}
			_removeRef();
			_size = size;
			_capacity = size;
			_items = items;
		}
	}

	void _checkCapacity() {
		if (_size + 1 > _capacity) {
			int capacity = _capacity == 0 ? 2 : _capacity * 2;
			T* items = _create(capacity);
			if (_items != nullptr) {
				int size = _size;
				_copy(_size, items, _items);
				_removeRef();
				_size = size;
			}
			_capacity = capacity;
			_items = items;
		}
	}

	T* _create(int capacity) {
		Header* header = (Header*) operator new(sizeof(Header) + (capacity * sizeof(T)));
		header->count = 1;
		return (T*)(header + 1);
	}

	Header* _header() {
		return ((Header*) _items) - 1;
	}

	void _addRef(Array const& other) {
		_size = other._size;
		_capacity = other._capacity;
		_items = other._items;
		if (_items == nullptr) {
			return;
		}
		Header* header = _header();
		header->count++;
	}

	void _removeRef() {
		if (_items == nullptr) {
			return;
		}
		Header* header = _header();
		header->count--;
		if (header->count == 0) {
			for (int i = 0; i < _size; i++) {
				_items[i].~T();
			}
			operator delete(header);
			_size = 0;
			_capacity = 0;
			_items = nullptr;
		}
	}

private:
	int _size;
	int _capacity;
	T* _items;
};
