#pragma once

#include "Hash.hpp"

class String {
public:
	struct Header {
		int count;
	};

public:
	String() {
		_size = 0;
	}

	String(char const* literal) {
		int size = 0;
		while (literal[size] != '\0') {
			size++;
		}
		_initialize(size, literal);
	}

	String(int size, char const* items) {
		_initialize(size, items);
	}

	String(String const& other) {
		_addRef(other);
	}

	static String allocate(int size) {
		if (size <= _smallStringSize) {
			String s;
			s._size = size;
			return s;
		}

		String s;
		s._heap.items = s._create(size);
		s._heap.capacity = size;
		s._size = size;
		return s;
	}

	bool operator==(String const& other) const {
		if (_size != other._size) {
			return false;
		}
		char const* selfItems = items();
		char const* otherItems = other.items();
		for (int i = 0; i < _size; i++) {
			if (selfItems[i] != otherItems[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(String const& other) const {
		return !(*this == other);
	}

	void operator=(String const& other) {
		_removeRef();
		_addRef(other);
	}

	char operator[](int index) const {
		return _size <= _smallStringSize ? _stack.items[index] : _heap.items[index];
	}

	String slice(int index, int size) const {
		return String(size, items() + index);
	}

	void append(String const& other) {
		append(other.size(), other.items());
	}

	void append(int size, char const* items) {
		// fit in stack
		if (_size + size <= _smallStringSize) {
			for (int i = 0; i < size; i++) {
				_stack.items[_size + i] = items[i];
			}
			_size += size;
			_stack.items[_size] = '\0';
			return;
		}

		_checkMigration(size);
		_checkOwnership();
		_checkCapacity(size);

		for (int i = 0; i < size; i++) {
			_heap.items[_size + i] = items[i];
		}
		_size += size;
		_heap.items[_size] = '\0';
	}

	~String() {
		_removeRef();
	}

	int size() const {
		return _size;
	}

	char const* items() const {
		return _size <= _smallStringSize ? _stack.items : _heap.items;
	}

private:
	void _copy(int size, char const* source, char* dest) {
		for (int i = 0; i < size; i++) {
			dest[i] = source[i];
		}
	}

	void _initialize(int size, char const* items) {
		if (size == 0) {
			_size = 0;
			_stack.items[0] = '\0';
			return;
		}

		if (size <= _smallStringSize) {
			_size = size;
			_copy(size, items, _stack.items);
			_stack.items[_size] = '\0';
			return;
		}

		_heap.items = _create(size);
		_heap.capacity = size;
		for (int i = 0; i < size; i++) {
			_heap.items[i] = items[i];
		}
		_size = size;
		_heap.items[_size] = '\0';
	}
	void _checkMigration(int size) {
		if (_size <= _smallStringSize) {
			int capacity = _size + size;
			char* items = _create(capacity);
			for (int i = 0; i < _size + 1; i++) {
				items[i] = _stack.items[i];
			}
			_heap.capacity = capacity;
			_heap.items = items;
		}
	}

	void _checkOwnership() {
		if (_header()->count > 1) {
			int size = _size;
			char* items = _create(size);
			for (int i = 0; i < _size + 1; i++) {
				items[i] = _heap.items[i];
			}
			_removeRef();
			_size = size;
			_heap.capacity = size;
			_heap.items = items;
		}
	}

	void _checkCapacity(int size) {
		if (_size + size > _heap.capacity) {
			int oldSize = _size;
			int minCapacity = _size + size;
			int doubledCapacity = _heap.capacity * 2;
			int capacity = minCapacity > doubledCapacity ? minCapacity : doubledCapacity;

			char* items = _create(capacity);
			for (int i = 0; i < _size + 1; i++) {
				items[i] = _heap.items[i];
			}

			_removeRef();
			_size = oldSize;
			_heap.capacity = capacity;
			_heap.items = items;
		}
	}
	void _addRef(String const& other) {
		_size = other._size;
		if (_size == 0) {
			return;
		}
		if (_size <= _smallStringSize) {
			for (int i = 0; i < _size; i++) {
				_stack.items[i] = other._stack.items[i];
			}
			return;
		}
		_heap = other._heap;
		Header* header = _header();
		header->count++;
	}

	void _removeRef() {
		if (_size == 0) {
			return;
		}

		if (_size <= _smallStringSize) {
			return;
		}

		Header* header = ((Header*) _heap.items) - 1;
		header->count--;

		if (header->count == 0) {
			operator delete(header);
			_size = 0;
			_heap.items = nullptr;
		}
	}


	char* _create(int capacity) {
		Header* header = (Header*) operator new(sizeof(Header) + (capacity * (sizeof(char) + 1)));
		header->count = 1;
		return (char*)(header + 1);
	}

	Header* _header() {
		return ((Header*) _heap.items) - 1;
	}

private:
	struct Heap {
		char* items;
		int capacity;
	};
	struct Stack {
		char items[sizeof(Heap)];
	};

	enum {
		_smallStringSize = sizeof(Heap) - 1,
	};

	int _size;
	union {
		Heap _heap;
		Stack _stack;
	};
};

template <>
struct Hash<String> {
	static int compute(String const& value) {
		int prime = 0x811C9DC5;
		int hash = 0;
		char const* items = value.items();

		for (int i = 0; i < value.size(); i++) {
			hash *= prime;
			hash ^= items[i];
		}

		return hash;
	}
};

