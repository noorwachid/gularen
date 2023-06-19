#include "Tester.h"

void primitiveTest(Tester& tester) {
	// Guaranteed to return true, if it's not then something must horibly went
	// wrong
	tester.group("Primitive", [&tester]() { tester.test("Boolean Comparison", []() { return true == true; }); });
}
