#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsFontStyleTest(Tester& tester) {
    using namespace Gularen;

    tester.group("LS Font Styles", [&tester]() {
        tester.test("Serial", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("*Hello* _darkness_ `my old friend`.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
                createRC<BoldFSNode>(NodeChildren{createRC<TextNode>("Hello")}),
                createRC<TextNode>(" "),
                createRC<ItalicFSNode>(NodeChildren{createRC<TextNode>("darkness")}),
                createRC<TextNode>(" "),
                createRC<MonospaceFSNode>(NodeChildren{createRC<TextNode>("my old friend")}),
                createRC<TextNode>("."),
            })});

            return *generated == *expected;
        });

        tester.test("Nesting", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("*Hello _darkness `my old friend`_*.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
                createRC<BoldFSNode>(NodeChildren{
                    createRC<TextNode>("Hello "),
                    createRC<ItalicFSNode>(NodeChildren{
                        createRC<TextNode>("darkness "),
                        createRC<MonospaceFSNode>(NodeChildren{createRC<TextNode>("my old friend")}),
                    }),
                }),
                createRC<TextNode>("."),
            })});

            return *generated == *expected;
        });
    });
}
