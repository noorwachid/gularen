#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSIndentationTest(Tester& tester)
{
	using namespace Gularen;

    tester.Group(
        "LS Indentation",
        [&tester]()
        {
            tester.Test(
                "Single Indentation",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("Level 0.\n    Level 1.\n");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 0."),
                        }),
                        CreateRC<IndentationNode>(NodeChildren{CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 1."),
                        })})});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Piramid Indentation",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("Level 0.\n    Level 1.\nLevel 0.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 0."),
                        }),
                        CreateRC<IndentationNode>(NodeChildren{CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 1."),
                        })}),
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 0."),
                        }),
                    });

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Irregular Indentation",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("Level 0.\n            Level 3.\n    Level 1.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Level 0."),
                        }),
                        CreateRC<IndentationNode>(NodeChildren{
                            CreateRC<IndentationNode>(NodeChildren{
                                CreateRC<IndentationNode>(NodeChildren{CreateRC<ParagraphNode>(NodeChildren{
                                    CreateRC<TextNode>("Level 3."),
                                })})}),
                            CreateRC<ParagraphNode>(NodeChildren{
                                CreateRC<TextNode>("Level 1."),
                            }),
                        }),
                    });

                    return *generated == *expected;
                }
            );
        }
    );
}
