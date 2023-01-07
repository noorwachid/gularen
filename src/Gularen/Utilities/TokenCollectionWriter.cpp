#include "TokenCollectionWriter.h"
#include <iostream>

namespace Gularen {
	void TokenCollectionWriter::write(const Array<Token>& tokenCollection) {
        for (const Token& token: tokenCollection)
            std::cout << token.toString() << "\r\n";
	}
}
