#pragma once

#include "Hash.hpp"

struct InvalidKey {};

template <typename K, typename V>
class Table {
public:
	struct Entry {
		K key;
		V value;
		Entry* prev;
		Entry* next;
		Entry* bucketNext;

		Entry(K const& key, V const& value): key{key}, value{value} {
			prev = nullptr;
			next = nullptr;
			bucketNext = nullptr;
		}
	};

	struct Header {
		int count;
	};

	class Iterator {
	public:
		Iterator(Entry* entry)  {
			_entry = entry;
		}

		bool hasNext() const {
			return _entry != nullptr;
		}

		void next() {
			if (_entry == nullptr) {
				return;
			}
			_entry = _entry->next;
		}

		K const& key() const {
			return _entry->key;
		}

		V const& value() const {
			return _entry->value;
		}

	private:
		Entry* _entry;
	};

public:
	Table() {
		_size = 0;
		_capacity = 0;
		_entries = nullptr;
	}

	Table(Table const& other) {
		_addRef(other);
	}

	~Table() {
		_removeRef();
	}

	void operator=(Table const& other) {
		_removeRef();
		_addRef(other);
	}

	bool has(K const& key) const {
		if (_entries == nullptr) {
			return false;
		}
		int index = Hash<K>::compute(key) & (_capacity - 1);
		Entry* entry = _entries[index];
		while (entry) {
			if (key == entry->key) {
				return true;
			}
			entry = entry->bucketNext;
		}
		return false;
	}

	V const& operator[](K const& key) const {
		if (_entries == nullptr) {
			throw InvalidKey();
		}

		int index = Hash<K>::compute(key) & (_capacity - 1);
		Entry* entry = _entries[index];
		while (entry) {
			if (key == entry->key) {
				return entry->value;
			}
			entry = entry->bucketNext;
		}
		throw InvalidKey();
	}

	void set(K const& key, V const& value) {
		_checkOwnership();

		// grow and rehash
		if (_entries == nullptr || ((float) _size / (float) _capacity) > _growthFactor) {
			int capacity = _capacity == 0 ? _initialCapacity : _capacity * 2;
			Entry** entries = _create(capacity);

			if (_entries != nullptr) {
				_copy(capacity, entries);
				operator delete(_header());
			}
			
			_capacity = capacity;
			_entries = entries;
		}

		_set(key, value);
	}

	Iterator iterate() {
		return Iterator(_capacity == 0 ? nullptr : _entries[_capacity]);
	}

	int size() const {
		return _size;
	}

private:
	void _copy(int capacity, Entry** entries) {
		// maintain insert order
		Entry** orderedEntries = (Entry**) operator new(_size * sizeof(Entry*));
		Entry* entry = _entries[_capacity];
		int index = 0;

		while (entry) {
			orderedEntries[index] = entry;
			entry = entry->next;
			index++;
		}

		for (int i = 0; i < _size; i++) {
			Entry* newEntry = orderedEntries[i];

			int index = Hash<K>::compute(newEntry->key) & (capacity - 1);
			Entry* prevEntry = nullptr;
			Entry* entry = entries[index];

			while (entry) {
				prevEntry = entry;
				entry = entry->bucketNext;
			}

			newEntry->bucketNext = nullptr;
			newEntry->next = nullptr;
			newEntry->prev = nullptr;

			// insert order
			if (entries[capacity] == nullptr) {
				entries[capacity] = newEntry;
				entries[capacity + 1] = newEntry;
			} else {
				newEntry->prev = entries[capacity + 1];
				entries[capacity + 1]->next = newEntry;
				entries[capacity + 1] = newEntry;
			}

			// bucket order
			if (prevEntry) {
				prevEntry->bucketNext = newEntry;
			} else {
				entries[index] = newEntry;
			}
		}

		operator delete(orderedEntries);
	}

	void _set(K const& key, V const& value) {
		int index = Hash<K>::compute(key) & (_capacity - 1);
		Entry* prevEntry = nullptr;
		Entry* entry = _entries[index];

		while (entry) {
			if (key == entry->key) {
				entry->value = value;
				return;
			}
			prevEntry = entry;
			entry = entry->bucketNext;
		}

		// insert order
		Entry* newEntry = new Entry(key, value);
		if (_entries[_capacity] == nullptr) {
			_entries[_capacity] = newEntry;
			_entries[_capacity + 1] = newEntry;
		} else {
			newEntry->prev = _entries[_capacity + 1];
			_entries[_capacity + 1]->next = newEntry;
			_entries[_capacity + 1] = newEntry;
		}

		// bucket order
		if (prevEntry) {
			prevEntry->bucketNext = newEntry;
		} else {
			_entries[index] = newEntry;
		}
		_size++;
	}

	Entry** _create(int capacity) {
		Header* header = (Header*) operator new(sizeof(Header) + (capacity + 2) * sizeof(Entry*));
		header->count = 1;
		Entry** entries = (Entry**)(header + 1);
		for (int i = 0; i < capacity + 2; i++) {
			entries[i] = nullptr;
		}
		return entries;
	}

	void _addRef(Table const& other) {
		_size = other._size;
		_capacity = other._capacity;
		_entries = other._entries;
		if (_entries == nullptr) {
			return;
		}
		Header* header = _header();
		header->count++;
	}

	void _removeRef() {
		if (_entries == nullptr) {
			return;
		}
		Header* header = _header();
		header->count--;
		if (header->count == 0) {
			for (int i = 0; i < _capacity; i++) {
				Entry* entry = _entries[i];
				while (entry) {
					Entry* next = entry->bucketNext;
					delete entry;
					entry = next;
				}
			}
			operator delete(header);
			_size = 0;
			_capacity = 0;
			_entries = nullptr;
		}
	}

	void _checkOwnership() {
		if (_entries != nullptr && _header()->count > 1) {
			Entry** entries = _create(_size);
			_copy(_size, entries);
			_removeRef();
			_capacity = _size;
			_entries = entries;
		}
	}

	Header* _header() {
		return ((Header*) _entries) - 1;
	}

private:
	static constexpr float _growthFactor = 0.7;
	static constexpr int _initialCapacity = 16;
	int _size;
	int _capacity;
	Entry** _entries;
};
