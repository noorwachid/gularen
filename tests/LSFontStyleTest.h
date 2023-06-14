#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void LSFontStyleTest(Tester& tester)
{
	using namespace Gularen;

    tester.Group(
        "LS Font Styles",
        [&tester]()
        {
            tester.Test(
                "Serial",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("*Hello* _darkness_ `my old friend`.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{CreateRC<ParagraphNode>(NodeChildren{
                        CreateRC<BoldFSNode>(NodeChildren{CreateRC<TextNode>("Hello")}),
                        CreateRC<TextNode>(" "),
                        CreateRC<ItalicFSNode>(NodeChildren{CreateRC<TextNode>("darkness")}),
                        CreateRC<TextNode>(" "),
                        CreateRC<MonospaceFSNode>(NodeChildren{CreateRC<TextNode>("my old friend")}),
                        CreateRC<TextNode>("."),
                    })});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "Nesting",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("*Hello _darkness `my old friend`_*.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{CreateRC<ParagraphNode>(NodeChildren{
                        CreateRC<BoldFSNode>(NodeChildren{
                            CreateRC<TextNode>("Hello "),
                            CreateRC<ItalicFSNode>(NodeChildren{
                                CreateRC<TextNode>("darkness "),
                                CreateRC<MonospaceFSNode>(NodeChildren{CreateRC<TextNode>("my old friend")}),
                            }),
                        }),
                        CreateRC<TextNode>("."),
                    })});

                    return *generated == *expected;
                }
            );
        }
    );
}
