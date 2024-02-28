#include "Gularen/Frontend/Lexer.h"

using namespace Gularen;

int main() {
	Lexer lexer;
	Slice<Token> tokens = lexer.parse("hello *world*!");

	for (unsigned int i = 0; i < tokens.size(); i += 1) {
		const Token& token = tokens.get(i);
		StringSlice kind = toStringSlice(token.kind);

		printf("%.*s", kind.size(), kind.pointer());

		if (token.value.size() != 0) {
			printf(" = %.*s", token.value.size(), token.value.pointer());
		}

		printf("\n");
	}
}
