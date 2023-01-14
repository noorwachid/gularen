#include "Gularen/ASTBuilder.h"
#include "Gularen/Node/NodeWriter.h"
#include "Tester.h"
#include <iostream>
#include <vector>

int main()
{
    using namespace Gularen;

    Lexer lexer;
    auto tokens = lexer.Tokenize(R"(
        1. First
        2. Second
        3. Third
    )");


    for (const Token& token : tokens)
    {
        std::cout << token.ToString() << "\n";
    }

    ASTBuilder builder;
    auto tree = builder.Build(tokens);

    NodeWriter writer;
    writer.Write(tree);

    return 0;

    Tester tester;

    // Guaranteed to return true, if it's not then something must horibly went
    // wrong
    tester.Group("Primitive", [&tester]() { tester.Test("Boolean Comparison", []() { return true == true; }); });

    tester.Group(
        "LS Comments",
        [&tester]()
        {
            tester.Test(
                "Inline",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("Now you see me. # Now you don't.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{CreateRC<TextNode>("Now you see me. ")})});

                    return *generated == *expected;
                }
            );
        }
    );

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

    tester.Group(
        "LS Paragraphs",
        [&tester]()
        {
            tester.Test(
                "Two Paragraphs",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build("First line.\nSecond line.\n\nThird line.\nForth line.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("First line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Second line."),
                        }),
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("Third line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Forth line."),
                        })});

                    return *generated == *expected;
                }
            );

            tester.Test(
                "After Other Block",
                []()
                {
                    ASTBuilder builder;
                    RC<Node> generated = builder.Build(">>> A Title\nFirst line.\nSecond line.");
                    RC<Node> expected = CreateRC<RootNode>(NodeChildren{
                        CreateRC<DocumentNode>(NodeChildren{
                            CreateRC<TitleNode>(NodeChildren{CreateRC<TextNode>("A Title")}),
                        }),
                        CreateRC<ParagraphNode>(NodeChildren{
                            CreateRC<TextNode>("First line."),
                            CreateRC<NewlineNode>(1),
                            CreateRC<TextNode>("Second line."),
                        }),
                    });

                    return *generated == *expected;
                }
            );
        }
    );

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

    tester.Sumarize();

    return 0;
}
