#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSCommentTest(Tester& tester)
{
    tester.Group(
        "LS Comments",
        [&tester]()
        {
            tester.Test(
                "Inline",
                []()
                {
					using namespace Gularen;

                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("Now you see me. # Now you don't.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{CreateRC<TextNode>("Now you see me. ")})});

                    return *generated == *expected;
                }
            );
        }
    );
}
