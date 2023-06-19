#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsIndentationTest(Tester& tester) {
    using namespace Gularen;

    tester.group("LS Indentation", [&tester]() {
        tester.test("Single Indentation", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("Level 0.\n    Level 1.\n");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 0."),
                }),
                createRC<IndentationNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 1."),
                })})});

            return *generated == *expected;
        });

        tester.test("Piramid Indentation", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("Level 0.\n    Level 1.\nLevel 0.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 0."),
                }),
                createRC<IndentationNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 1."),
                })}),
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 0."),
                }),
            });

            return *generated == *expected;
        });

        tester.test("Irregular Indentation", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("Level 0.\n            Level 3.\n    Level 1.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Level 0."),
                }),
                createRC<IndentationNode>(NodeChildren{
                    createRC<IndentationNode>(NodeChildren{
                        createRC<IndentationNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
                            createRC<TextNode>("Level 3."),
                        })})}),
                    createRC<ParagraphNode>(NodeChildren{
                        createRC<TextNode>("Level 1."),
                    }),
                }),
            });

            return *generated == *expected;
        });
    });
}
