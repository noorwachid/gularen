#pragma once

#include "Byte.hpp"
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
		char const* it = literal;
		while (*it) {
			it++;
		}
		_size = it - literal;
		if (_size == 0) {
			return;
		}
		if (_size <= _smallStringSize) {
			for (int i = 0; i < _size; i++) {
				_stack.items[i] = literal[i];
			}
			return;
		}

		Header* header = (Header*) operator new(sizeof(Header) + sizeof(Byte) * _size);
		header->count = 1;
		_heap.items = (Byte*)(header + 1);
		for (int i = 0; i < _size; i++) {
			_heap.items[i] = literal[i];
		}
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
		Header* header = (Header*) operator new(sizeof(Header) + sizeof(Byte) * size);
		header->count = 1;
		s._heap.items = (Byte*)(header + 1);
		s._size = size;
		return s;
	}

	bool operator==(String const& other) const {
		if (_size != other._size) {
			return false;
		}
		Byte const* selfItems = items();
		Byte const* otherItems = other.items();
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

	Byte operator[](int index) const {
		return _size <= _smallStringSize ? _stack.items[index] : _heap.items[index];
	}

	String slice(int index, int size) {
		if (size <= _smallStringSize) {
			String s;
			s._size = size;
			Byte const* oitems = items();
			for (int i = 0; i < size; i++) {
				s._stack.items[i] = oitems[index + i];
			}
			return s;
		}

		String s;
		Header* header = (Header*) operator new(sizeof(Header) + sizeof(Byte) * size);
		header->count = 1;
		s._heap.items = (Byte*)(header + 1);
		s._size = size;
		Byte const* oitems = items();
		for (int i = 0; i < size; i++) {
			s._heap.items[i] = oitems[index + i];
		}
		return s;
	}

	void append(String const& other) {
		// fit in stack
		if (_size + other._size <= _smallStringSize) {
			for (int i = 0; i < other._size; i++) {
				_stack.items[_size + i] = other._stack.items[i];
			}
			_size += other._size;
			return;
		}

		_checkMigration(other);
		_checkOwnership();
		_checkCapacity(other);

		Byte const* otherItems = other.items();
		for (int i = 0; i < other._size; i++) {
			_heap.items[_size + i] = otherItems[i];
		}
		_size += other._size;
	}

	~String() {
		_removeRef();
	}

	int size() const {
		return _size;
	}

	Byte const* items() const {
		return _size <= _smallStringSize ? _stack.items : _heap.items;
	}

private:
	void _checkMigration(String const& other) {
		if (_size <= _smallStringSize) {
			int capacity = _size + other._size;
			Byte* items = _create(capacity);
			for (int i = 0; i < _size; i++) {
				items[i] = _stack.items[i];
			}
			_heap.capacity = capacity;
			_heap.items = items;
		}
	}

	void _checkOwnership() {
		if (_header()->count > 1) {
			int size = _size;
			Byte* items = _create(size);
			for (int i = 0; i < _size; i++) {
				items[i] = _heap.items[i];
			}
			_removeRef();
			_size = size;
			_heap.capacity = size;
			_heap.items = items;
		}
	}

	void _checkCapacity(String const& other) {
		if (_size + other._size > _heap.capacity) {
			int size = _size;
			int minCapacity = _size + other._size;
			int doubledCapacity = _heap.capacity * 2;
			int capacity = minCapacity > doubledCapacity ? minCapacity : doubledCapacity;

			Byte* items = _create(capacity);
			for (int i = 0; i < _size; i++) {
				items[i] = _heap.items[i];
			}

			_removeRef();
			_size = size;
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
			for (int i = 0; i < _smallStringSize; i++) {
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


	Byte* _create(int capacity) {
		Header* header = (Header*) operator new(sizeof(Header) + (capacity) * sizeof(Byte));
		header->count = 1;
		return (Byte*)(header + 1);
	}

	Header* _header() {
		return ((Header*) _heap.items) - 1;
	}

private:
	struct Heap {
		Byte* items;
		int capacity;
	};

	static constexpr int _smallStringSize = sizeof(Heap);

	struct Stack {
		Byte items[_smallStringSize];
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
		Byte const* items = value.items();

		for (int i = 0; i < value.size(); i++) {
			hash *= prime;
			hash ^= items[i];
		}

		return hash;
	}
};

