#include "Tester.h"

void PrimitiveTest(Tester& tester)
{
    // Guaranteed to return true, if it's not then something must horibly went
    // wrong
    tester.Group("Primitive", [&tester]() { tester.Test("Boolean Comparison", []() { return true == true; }); });
}
