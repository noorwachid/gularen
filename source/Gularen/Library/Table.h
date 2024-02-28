#pragma once

#include "Gularen/Library/Array.h"
#include "Gularen/Library/StringSlice.h"
#include "Gularen/Library/Optional.h"
#include "Gularen/Library/TableIter.h"

namespace Gularen {

template <typename T>
struct Table {
public:
	struct Entry {
		StringSlice key;
		T value;
		Entry* next;
	};

	friend TableIter<Table<T>, Entry>;

public:
	Table() {
		_buckets.reserve(16);
		for (unsigned int i = 0; i < 16; ++i) {
			_buckets.append(nullptr);
		}

		_size = 0;
	}

	Table(const Table<T>& other) {
		_buckets.reserve(16);
		for (unsigned int i = 0; i < 16; ++i) {
			_buckets.append(nullptr);
		}

		_size = 0;

		for (auto it = other.iterate(); it.hasNext(); it.next()) {
			const Entry& entry = it.get();
			set(entry.key, entry.value);
		}
	}

	Table(Table<T>&& other) {
		_size = other._size;
		_buckets = static_cast<Array<Entry*>&&>(other._buckets);
		other._size = 0;
	}

	~Table() {
		if (_size) {
			for (unsigned int i = 0; i < _buckets.size(); ++i) {
				Entry* entry = _buckets.get(i);

				while (entry) {
					Entry* next = entry->next;
					free(reinterpret_cast<char*>(entry) - entry->key.size());
					entry = nullptr;
					entry = next;
				}
			}
			_size = 0;
		}
	}

	void operator=(const Table<T>& other) {
		this->~Table();

		_buckets.reserve(16);
		for (unsigned int i = 0; i < 16; ++i) {
			_buckets.append(nullptr);
		}

		_size = 0;

		for (auto it = other.iterate(); it.hasNext(); it.next()) {
			const Entry& entry = it.get();
			set(entry.key, entry.value);
		}
	}

	void operator=(Table<T>&& other) {
		this->~Table();

		_size = other._size;
		_buckets = static_cast<Array<Entry*>&&>(other._buckets);
		other._size = 0;
	}

	TableIter<Table<T>, Entry> iterate() const {
		return TableIter<Table<T>, Entry>(*this);
	}

	Optional<T> get(StringSlice key) const {
		Entry* entry = _buckets.get(_hash(key));

		while (entry) {
			if (entry->key == key) {
				return entry->value;
			}

			entry = entry->next;
		}

		return nullptr;
	}

	bool has(StringSlice key) {
		Entry* entry = _buckets.get(_hash(key));

		while (entry) {
			if (entry->key == key) {
				return true;
			}

			entry = entry->next;
		}

		return false;
	}

	void set(StringSlice key, const T& value) {
		_grow();

		unsigned int index = _hash(key);
		Entry* entry = _buckets.get(index);

		// best case scenario
		if (entry == nullptr) {
			_buckets.set(index, createEntry(key, value));
			return;
		}

		Entry* previousEntry = nullptr;
		while (entry) {
			// key already exists
			if (entry->key == key) {
				entry->value = value;
				return;
			}

			previousEntry = entry;
			entry = entry->next;
		}

		// worst case scenario
		previousEntry->next = createEntry(key, value);

		// printf("PLACING: %.*s\n", key.size(), key.data());
		//
		// for (auto i = this->iterate(); i.hasNext(); i.next()) {
		// 	auto entry = i.get();
		// 	auto key = entry.key;
		// 	auto value = toSlice(entry.value);
		//
		// 	printf("%.*s = %.*s\n", key.size(), key.data(), value.size(), value.data());
		// }
		// printf("/PLACING\n\n");
	}

	inline unsigned int size() const {
		return _size;
	}

private:
	struct SearchResult {
		Entry* entry;
		Entry* previousEntry;
		bool found;
	};


private:
	Table(unsigned int size) {
		_buckets.reserve(size);
		for (unsigned int i = 0; i < size; ++i) {
			_buckets.append(nullptr);
		}

		_size = 0;
	}

	unsigned int _hash(StringSlice key) const {
		unsigned int h = 2166136261UL;
		for (unsigned int i = 0; i < key.size(); i++) {
			h ^= key.get(i);
			h *= 16777619UL;
		}

		return h & _buckets.size() - 1;
	}


	Entry* createEntry(StringSlice key, const T& value) {
		// make the key data + Entry in single allocation 
		// since the key never changes length
		char* bytes = static_cast<char*>(malloc(sizeof(char) + key.size() + sizeof(Entry)));

		memcpy(bytes, key.pointer(), key.size());

		Entry* entry = reinterpret_cast<Entry*>(bytes + key.size());
		entry->next = nullptr;
		entry->key = StringSlice(bytes, key.size()); 
		entry->value = value;

		_size += 1;

		return entry;
	}

	void _grow() {
		float loadFactor = 0.8f;

		if (!(static_cast<float>(_size) / static_cast<float>(_buckets.size()) > loadFactor)) {
			return;
		}

		Table<T> newTable(_buckets.size() * 2);

		for (unsigned int i = 0; i < _buckets.size(); ++i) {
			Entry* entry = _buckets.get(i);

			while (entry) {
				newTable._place(entry);
				entry = entry->next;
			}
		}

		_buckets.~Array();

		_size = newTable.size();
		_buckets = static_cast<Array<Entry*>&&>(newTable._buckets);
		newTable._size = 0;
	}

	void _place(Entry* oldEntry) {
		unsigned int index = _hash(oldEntry->key);
		Entry* entry = _buckets.get(index);

		// best case scenario
		if (entry == nullptr) {
			_buckets.set(index, oldEntry);
			return;
		}

		Entry* previousEntry = nullptr;
		while (entry) {
			// key already exists
			if (entry->key == oldEntry->key) {
				return;
			}
			
			previousEntry = entry;
			entry = entry->next;
		}
		
		previousEntry->next = oldEntry;
		_size += 1;
	}

private:
	Array<Entry*> _buckets;
	unsigned int _size;
};

}
