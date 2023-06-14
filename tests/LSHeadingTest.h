#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSHeadingTest(Tester& tester)
{
	using namespace Gularen;

    tester.Group(
        "LS Headings",
        [&tester]()
        {
            tester.Test(
                "Chapter",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build(">>>-> Chapter 1");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{CreateRC<ChapterNode>(NodeChildren{
                        CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("Chapter 1")})})});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Section with ID",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build(">>-> Red Apple > red-apple");

                    RC<SectionNode> section = CreateRC<SectionNode>(NodeChildren{
                        CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("Red Apple ")}),
                    });

                    section->id = "red-apple";

                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{section});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Segment",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("> Amoeba");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{CreateRC<SegmentNode>(NodeChildren{
                        CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("Amoeba")})})});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Document with subtitle",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build(">>> A Song of Ice and Fire\n> A Game of Thrones");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{CreateRC<DocumentNode>(NodeChildren{
                        CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("A Song of Ice and Fire")}),
                        CreateRC<SubtitleNode>(NodeChildren{CreateRC<TextNode>("A Game of Thrones")})})});

                    return *generated == *expected;
                }
            );
        }
    );
}
