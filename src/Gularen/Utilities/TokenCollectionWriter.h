#pragma once 

#include "../Integer.h"
#include "../Token.h"
#include "../DS/Array.h"

namespace Gularen {
	class TokenCollectionWriter {
	public:
        void write(const Array<Token>& tokenCollection);
	};
}
