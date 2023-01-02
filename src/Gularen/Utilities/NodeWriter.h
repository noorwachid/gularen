#pragma once 

#include "../Integer.h"
#include "../Node.h"

namespace Gularen {
	class NodeWriter {
	public:
        void Write(const RC<Node>& parentNode, UintSize depth = 0);
	};
}
