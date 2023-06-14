#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSParagraphTest(Tester& tester)
{
	using namespace Gularen;

    tester.Group(
        "LS Paragraphs",
        [&tester]()
        {
            tester.Test(
                "Two Paragraphs",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("First line.\nSecond line.\n\nThird line.\nForth line.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("First line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Second line."),
                        }),
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Third line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Forth line."),
                        })});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "After Other Block",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build(">>> A Title\nFirst line.\nSecond line.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<DocumentNode>(NodeChildren{
                            CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("A Title")}),
                        }),
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("First line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Second line."),
                        }),
                    });

                    return *generated == *expected;
                }
            );
        }
    );
}
