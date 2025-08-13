#pragma once

#include "Parser.hpp"

namespace HtmlGen {
	struct Option {
		String template_;
	};

	String gen(Node* node, Option const& option);
};
