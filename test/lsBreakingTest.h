#include "Gularen/ASTBuilder.h"
#include "Tester.h"

void lsBreakingTest(Tester& tester) {
	using namespace Gularen;

	tester.group("LS Breaking", [&tester]() {
		tester.test("Line Break", []() {
			ASTBuilder builder;
			RC<Node> generated = builder.build("See you!<Night!");
			RC<Node> expected = createRC<RootNode>(NodeChildren{createRC<ParagraphNode>(NodeChildren{
				createRC<TextNode>("See you!"), createRC<LineBreakNode>(), createRC<TextNode>("Night!")})});

			return *generated == *expected;
		});

		tester.test("Page Break", []() {
			ASTBuilder builder;
			RC<Node> generated = builder.build("See you!\n<<\nNight!");
			RC<Node> expected = createRC<RootNode>(NodeChildren{
				createRC<ParagraphNode>(NodeChildren{
					createRC<TextNode>("See you!"),
				}),
				createRC<PageBreakNode>(), createRC<ParagraphNode>(NodeChildren{createRC<TextNode>("Night!")})});

			return *generated == *expected;
		});

		tester.test("Document Break", []() {
			ASTBuilder builder;
			RC<Node> generated = builder.build("See you!\n<<<\nNight!");
			RC<Node> expected = createRC<RootNode>(NodeChildren{
				createRC<ParagraphNode>(NodeChildren{
					createRC<TextNode>("See you!"),
				}),
			});

			return *generated == *expected;
		});
	});
}
