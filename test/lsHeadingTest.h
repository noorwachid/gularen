#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsHeadingTest(Tester& tester) {
    using namespace Gularen;

    tester.group("LS Headings", [&tester]() {
        tester.test("Chapter", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build(">>>-> Chapter 1");
            RC<Node> expected = createRC<RootNode>(NodeChildren{createRC<ChapterNode>(NodeChildren{
                createRC<TitleNode>(NodeChildren{createRC<TextNode>("Chapter 1")})})});

            return *generated == *expected;
        });

        tester.test("Section with ID", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build(">>-> Red Apple > red-apple");

            RC<SectionNode> section = createRC<SectionNode>(NodeChildren{
                createRC<TitleNode>(NodeChildren{createRC<TextNode>("Red Apple ")}),
            });

            section->id = "red-apple";

            RC<Node> expected = createRC<RootNode>(NodeChildren{section});

            return *generated == *expected;
        });

        tester.test("Segment", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build("> Amoeba");
            RC<Node> expected = createRC<RootNode>(NodeChildren{
                createRC<SegmentNode>(NodeChildren{createRC<TitleNode>(NodeChildren{createRC<TextNode>("Amoeba")})})});

            return *generated == *expected;
        });

        tester.test("Document with subtitle", []() {
            ASTBuilder builder;
            RC<Node> generated = builder.build(">>> A Song of Ice and Fire\n> A Game of Thrones");
            RC<Node> expected = createRC<RootNode>(NodeChildren{createRC<DocumentNode>(NodeChildren{
                createRC<TitleNode>(NodeChildren{createRC<TextNode>("A Song of Ice and Fire")}),
                createRC<SubtitleNode>(NodeChildren{createRC<TextNode>("A Game of Thrones")})})});

            return *generated == *expected;
        });
    });
}
