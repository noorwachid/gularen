#pragma once

#include "Gularen/Node.h"

namespace Gularen {
	NodePtr parse(const std::string& content);

	NodePtr parseFile(const std::string& path);
}

