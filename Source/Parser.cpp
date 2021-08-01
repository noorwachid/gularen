#include "Gularen/IO.hpp"
#include "Gularen/Lexer.hpp"
#include "Gularen/AstBuilder.hpp"

using namespace Gularen;

int main()
{
    std::string buffer = IO::ToString("../Test/Files/Partition.gr");

    Lexer lexer;
    lexer.SetBuffer(buffer);
    lexer.Parse();
    std::vector<Token> tokens = lexer.GetTokens();

    IO::WriteLine(lexer.ToString());


    AstBuilder builder;
    builder.SetTokens(std::move(tokens));
    builder.Parse();

    IO::WriteLine(builder.ToString());

    return 0;
}
