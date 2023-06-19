#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsCommentTest(Tester& tester) {
    tester.group("LS Comments", [&tester]() {
        tester.test("Inline", []() {
            using namespace Gularen;

            ASTBuilder builder;
            RC<Node> generated = builder.build("Now you see me. # Now you don't.");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<ParagraphNode>(NodeChildren{createRC<TextNode>("Now you see me. ")})});

            return *generated == *expected;
        });
    });
}
