#include <iostream>
#include <vector>
#include "Gularen/ASTBuilder.h"
#include "Gularen/Utilities/NodeWriter.h"
#include "Tester.h"

int main()
{
    using namespace Gularen;

    Tester tester;
    
    tester.Group("Primitive", [&tester]() 
    {
        tester.Test("Boolean Comparison", []()
        {
            return true == true;
        });
    });

    tester.Group("LF Comments", [&tester]() 
    {
        tester.Test("Inline", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse("Now you see me. # Now you don't.");
			RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                CreateRC<ParagraphNode>(NodeChildren{
    				CreateRC<TextNode>("Now you see me. ")
                })
			});

            return *generated == *expected;
        });
    });
    
    tester.Group("LF Font Styles", [&tester]() 
    {
        tester.Test("Serial", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse("*Hello* _darkness_ `my old friend`.");
			RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                CreateRC<ParagraphNode>(NodeChildren{
                    CreateRC<BoldFSNode>(NodeChildren{
        				CreateRC<TextNode>("Hello")
                    }),
                    CreateRC<TextNode>(" "),
                    CreateRC<ItalicFSNode>(NodeChildren{
        				CreateRC<TextNode>("darkness")
                    }),
                    CreateRC<TextNode>(" "),
                    CreateRC<MonospaceFSNode>(NodeChildren{
        				CreateRC<TextNode>("my old friend")
                    }),
                    CreateRC<TextNode>("."),
                })
			});

            return *generated == *expected;
        });

        tester.Test("Nesting", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse("*Hello _darkness `my old friend`_*.");
			RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                CreateRC<ParagraphNode>(NodeChildren{
                    CreateRC<BoldFSNode>(NodeChildren{
        				CreateRC<TextNode>("Hello "),
                            CreateRC<ItalicFSNode>(NodeChildren{
                            CreateRC<TextNode>("darkness "),
                            CreateRC<MonospaceFSNode>(NodeChildren{
                                CreateRC<TextNode>("my old friend")
                            }),
                        }),
                    }),
                    CreateRC<TextNode>("."),
                })
			});

            return *generated == *expected;
        });
    });

    tester.Group("LF Headings", [&tester]() 
    {
        tester.Test("Simple heading", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse(">>>-> Chapter 1");
			RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                CreateRC<ChapterNode>(NodeChildren{
                    CreateRC<TitleNode>(NodeChildren{
                        CreateRC<TextNode>("Chapter 1")
                    })
                })
			});

            return *generated == *expected;
        });
    });

    tester.Group("LF Paragraphs", [&tester]() 
    {
        tester.Test("Two Paragraphs", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse("First line.\nSecond line.\n\nThird line.\nForth line.");
			RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                CreateRC<ParagraphNode>(NodeChildren{
    				CreateRC<TextNode>("First line."),
    				CreateRC<TextNode>("Second line."),
                }),
                CreateRC<ParagraphNode>(NodeChildren{
    				CreateRC<TextNode>("Third line."),
    				CreateRC<TextNode>("Forth line."),
                })
			});

            return *generated == *expected;
        });
    });

    tester.Sumarize();

    return 0;
}
	
