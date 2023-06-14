#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSBreakingTest(Tester& tester)
{
	using namespace Gularen;

    tester.Group(
        "LS Breaking",
        [&tester]()
        {
            tester.Test(
                "Line Break",
                []()
                {

                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("See you!<Night!");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
							CreateRC<TextNode>("See you!"),
							CreateRC<LineBreakNode>(),
							CreateRC<TextNode>("Night!")
						})
					});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Page Break",
                []()
                {

                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("See you!\n<<\nNight!");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
							CreateRC<TextNode>("See you!"),
						}),
						CreateRC<PageBreakNode>(),
                        CreateRC<ParagraphNode>(NodeChildren{
							CreateRC<TextNode>("Night!")
						})
					});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Document Break",
                []()
                {

                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("See you!\n<<<\nNight!");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
							CreateRC<TextNode>("See you!"),
						}),
					});

                    return *generated == *expected;
                }
            );
        }
    );
}

