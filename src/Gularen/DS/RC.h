#pragma once 

#include <memory>

namespace Gularen {
	template <typename T>
	using RC = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr RC<T> makeRC(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
