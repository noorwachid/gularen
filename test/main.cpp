#include <iostream>
#include <vector>
#include "Gularen/ASTBuilder.h"
#include "Gularen/Utility/NodeWriter.h"
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
        tester.Test("One line", []()
        {
            ASTBuilder builder;
			RC<Node> generated = builder.Parse("Now you see me. # Now you don't.");
			RC<Node> expected = CreateRC<DocumentNode>(NodeChildren {
				CreateRC<TextNode>("Now you see me. ")
			});

            return *generated == *expected;
        });
    });

    tester.Sumarize();

    return 0;
}
	
