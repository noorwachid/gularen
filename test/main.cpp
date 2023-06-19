#include "Gularen/ASTBuilder.h"
#include "Gularen/Node/NodeWriter.h"
#include "lsBreakingTest.h"
#include "lsCommentTest.h"
#include "lsFontStyleTest.h"
#include "lsHeadingTest.h"
#include "lsIndentationTest.h"
#include "lsParagraphTest.h"
#include "primitiveTest.h"
#include <iostream>
#include <vector>

int main() {
	using namespace Gularen;

	//     Lexer lexer;
	//     auto tokens = lexer.Tokenize(R"(hello
	// <<<
	// world)");
	//
	//     for (const Token& token : tokens)
	//     {
	//         std::cout << token.ToString() << "\n";
	//     }
	//
	// 	std::cout << '\n';
	//
	//     ASTBuilder builder;
	//     auto tree = builder.Build(tokens);
	//
	//     NodeWriter writer;
	//     writer.Write(tree);
	//
	//     return 0;

	Tester tester;

	primitiveTest(tester);
	lsCommentTest(tester);
	lsFontStyleTest(tester);
	lsHeadingTest(tester);
	lsParagraphTest(tester);
	lsIndentationTest(tester);
	lsBreakingTest(tester);

	tester.sumarize();

	return 0;
}
