#include <iostream>
#include <vector>
#include "Gularen/ASTBuilder.h"
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
			builder.SetBuffer("Now you see me. # Now you don't.");
			builder.Parse();

			auto generated = builder.GetAST();
			auto expected = AST(CreateRC<DocumentNode>(NodeChildren {
				CreateRC<TextNode>("Now you see me. ")
			}));

			return generated.GetRootNode() == expected.GetRootNode();
        });
    });

    tester.Sumarize();

    return 0;
}
	
