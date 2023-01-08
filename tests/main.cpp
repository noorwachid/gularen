#include <iostream>
#include <vector>
#include "Gularen/ASTBuilder.h"
#include "Gularen/Utilities/NodeWriter.h"
#include "Tester.h"

int main() {
    using namespace Gularen;

    Tester tester;
    
    // Guaranteed to return true, if it's not then something must horibly went wrong
    tester.group("Primitive", [&tester]() {
        tester.test("Boolean Comparison", []() {
            return true == true;
        });
    });

    tester.group("LS Comments", [&tester]() {
        tester.test("Inline", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("Now you see me. # Now you don't.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Now you see me. ")
                })
			});

            return *generated == *expected;
        });
    });
    
    tester.group("LS Font Styles", [&tester]() {
        tester.test("Serial", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("*Hello* _darkness_ `my old friend`.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
                    makeRC<BoldFSNode>(NodeChildren{
        				makeRC<TextNode>("Hello")
                    }),
                    makeRC<TextNode>(" "),
                    makeRC<ItalicFSNode>(NodeChildren{
        				makeRC<TextNode>("darkness")
                    }),
                    makeRC<TextNode>(" "),
                    makeRC<MonospaceFSNode>(NodeChildren{
        				makeRC<TextNode>("my old friend")
                    }),
                    makeRC<TextNode>("."),
                })
			});

            return *generated == *expected;
        });

        tester.test("Nesting", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("*Hello _darkness `my old friend`_*.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
                    makeRC<BoldFSNode>(NodeChildren{
        				makeRC<TextNode>("Hello "),
                            makeRC<ItalicFSNode>(NodeChildren{
                            makeRC<TextNode>("darkness "),
                            makeRC<MonospaceFSNode>(NodeChildren{
                                makeRC<TextNode>("my old friend")
                            }),
                        }),
                    }),
                    makeRC<TextNode>("."),
                })
			});

            return *generated == *expected;
        });
    });

    tester.group("LS Headings", [&tester]() {
        tester.test("Chapter", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build(">>>-> Chapter 1");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ChapterNode>(NodeChildren{
                    makeRC<TitleNode>(NodeChildren{
                        makeRC<TextNode>("Chapter 1")
                    })
                })
			});

            return *generated == *expected;
        });

        tester.test("Section with ID", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build(">>-> Red Apple > red-apple");

            RC<SectionNode> section = makeRC<SectionNode>(NodeChildren{
                makeRC<TitleNode>(NodeChildren{
                    makeRC<TextNode>("Red Apple ")
                }),
            });

            section->id = "red-apple";

			RC<Node> expected = makeRC<RootNode>(NodeChildren{section});

            return *generated == *expected;
        });

        tester.test("Segment", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("> Amoeba");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<SegmentNode>(NodeChildren{
                    makeRC<TitleNode>(NodeChildren{
                        makeRC<TextNode>("Amoeba")
                    })
                })
			});

            return *generated == *expected;
        });

        tester.test("Document with subtitle", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build(">>> A Song of Ice and Fire\n> A Game of Thrones");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<DocumentNode>(NodeChildren{
                    makeRC<TitleNode>(NodeChildren{
                        makeRC<TextNode>("A Song of Ice and Fire")
                    }),
                    makeRC<SubtitleNode>(NodeChildren{
                        makeRC<TextNode>("A Game of Thrones")
                    })
                })
			});

            return *generated == *expected;
        });
    });

    tester.group("LS Paragraphs", [&tester]() {
        tester.test("Two Paragraphs", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("First line.\nSecond line.\n\nThird line.\nForth line.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("First line."),
                    makeRC<NewlineNode>(1),
    				makeRC<TextNode>("Second line."),
                }),
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Third line."),
                    makeRC<NewlineNode>(1),
    				makeRC<TextNode>("Forth line."),
                })
			});

            return *generated == *expected;
        });

        tester.test("After Other Block", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build(">>> A Title\nFirst line.\nSecond line.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<DocumentNode>(NodeChildren{
                    makeRC<TitleNode>(NodeChildren{
                        makeRC<TextNode>("A Title")
                    }),
                }),
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("First line."),
                    makeRC<NewlineNode>(1),
    				makeRC<TextNode>("Second line."),
                }),
			});

            return *generated == *expected;
        });
    });

    tester.group("LS Indentation", [&tester]() {
        tester.test("Single Indentation", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("Level 0.\n    Level 1.\n");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Level 0."),
                }),
                makeRC<IndentationNode>(NodeChildren{
                    makeRC<ParagraphNode>(NodeChildren{
                        makeRC<TextNode>("Level 1."),
                    })
                })
			});

            return *generated == *expected;
        });

        tester.test("Piramid Indentation", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("Level 0.\n    Level 1.\nLevel 0.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Level 0."),
                }),
                makeRC<IndentationNode>(NodeChildren{
                    makeRC<ParagraphNode>(NodeChildren{
                        makeRC<TextNode>("Level 1."),
                    })
                }),
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Level 0."),
                }),
			});

            return *generated == *expected;
        });

        tester.test("Irregular Indentation", []() {
            ASTBuilder builder;
			RC<Node> generated = builder.build("Level 0.\n            Level 3.\n    Level 1.");
			RC<Node> expected = makeRC<RootNode>(NodeChildren{
                makeRC<ParagraphNode>(NodeChildren{
    				makeRC<TextNode>("Level 0."),
                }),
                makeRC<IndentationNode>(NodeChildren{
                    makeRC<IndentationNode>(NodeChildren{
                        makeRC<IndentationNode>(NodeChildren{
                            makeRC<ParagraphNode>(NodeChildren{
                                makeRC<TextNode>("Level 3."),
                            })
                        })
                    }),
                    makeRC<ParagraphNode>(NodeChildren{
                        makeRC<TextNode>("Level 1."),
                    }),
                }),
			});

            return *generated == *expected;
        });
    });

    tester.sumarize();

    return 0;
}
	
