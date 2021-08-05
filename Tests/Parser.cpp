#include <Gularen/IO.hpp>
#include <Gularen/Lexer.hpp>
#include <Gularen/AstBuilder.hpp>
#include <GularenBridge/Html5/Renderer.hpp>

using namespace Gularen;
using namespace GularenBridge::Html5;

int main()
{
    std::string buffer = IO::ReadFile("../Tests/Files/Code.gr");

    Lexer lexer;
    lexer.SetBuffer(buffer);
    lexer.Parse();

    IO::WriteLine(lexer.ToString());

    AstBuilder builder;
    builder.SetTokens(lexer.GetTokens());
    builder.Parse();

    IO::WriteLine(builder.ToString());
}
