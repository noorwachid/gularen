#pragma once

namespace Gularen {

template <typename T>
class Optional {
public:
	using Null = decltype(nullptr);

	Optional(Null value): _occupied(false) {
	}

	Optional(T value): _occupied(true), _data(value) {
	}

	inline bool occupied() const {
		return _occupied;
	}

	inline const T& get() const {
		return _data;
	}

	inline T& get() {
		return _data;
	}

private:
	bool _occupied;
	T _data;
};

}
