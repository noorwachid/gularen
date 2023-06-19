#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsParagraphTest(Tester& tester) {
    using namespace Gularen;

    tester.group("LS Paragraphs", [&tester]() {
        tester.test("Two Paragraphs", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("First line.\nSecond line.\n\nThird line.\nForth line.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("First line."),
                    createRC<NewlineNode>(1),
                    createRC<TextNode>("Second line."),
                }),
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("Third line."),
                    createRC<NewlineNode>(1),
                    createRC<TextNode>("Forth line."),
                })});

            return *generated == *expected;
        });

        tester.test("After Other Block", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build(">>> A Title\nFirst line.\nSecond line.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<DocumentNode>(NodeChildren{
                    createRC<TitleNode>(NodeChildren{createRC<TextNode>("A Title")}),
                }),
                createRC<ParagraphNode>(NodeChildren{
                    createRC<TextNode>("First line."),
                    createRC<NewlineNode>(1),
                    createRC<TextNode>("Second line."),
                }),
            });

            return *generated == *expected;
        });
    });
}
