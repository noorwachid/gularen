#include "Gularen/ASTBuilder.h"
#include "Gularen/Node/NodeWriter.h"
#include "PrimitiveTest.h"
#include "LSCommentTest.h"
#include "LSFontStyleTest.h"
#include "LSHeadingTest.h"
#include "LSParagraphTest.h"
#include "LSIndentationTest.h"
#include "LSBreakingTest.h"
#include <iostream>
#include <vector>

int main()
{
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

	PrimitiveTest(tester);
	LSCommentTest(tester);
	LSFontStyleTest(tester);
	LSHeadingTest(tester);
	LSParagraphTest(tester);
	LSIndentationTest(tester);
	LSBreakingTest(tester);

    tester.Sumarize();

    return 0;
}
