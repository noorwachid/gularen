#pragma once

namespace Gularen {

template <typename T, typename E>
class TableIter {
public:
	TableIter(const T& table): _table(table) {
		if (table._buckets.size() == 0) {
			_bucketIndex = 0;
			return;
		}

		for (unsigned int i = 0; i < _table._buckets.size(); ++i) {
			if (_table._buckets.get(i) != nullptr) {
				_bucketIndex = i;
				_entry = _table._buckets.get(i);
				return;
			}
		}
	}

	const E& get() const {
		return *_entry;
	}

	bool hasNext() const {
		return _bucketIndex < _table._buckets.size();
	}

	void next() {
		if (_entry->next != nullptr) {
			_entry = _entry->next;
			return;
		}

		_bucketIndex += 1;

		for (unsigned int i = _bucketIndex; i < _table._buckets.size(); ++i) {
			if (_table._buckets.get(i) != nullptr) {
				_bucketIndex = i;
				_entry = _table._buckets.get(i);
				return;
			}
		}
		_bucketIndex = _table._buckets.size();
	}

private:
	const T& _table;
	E* _entry;
	unsigned int _bucketIndex;
};

}
