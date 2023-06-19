#pragma once

#include "../Integer.h"
#include "../Node.h"

namespace Gularen {
	class NodeWriter {
	public:
		void write(const RC<Node>& parentNode, UintSize depth = 0);
	};
}
