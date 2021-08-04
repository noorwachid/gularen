#include "Gularen/IO.hpp"
#include "Gularen/Lexer.hpp"
#include "Gularen/AstBuilder.hpp"
#include "GularenBridge/Html5/Renderer.hpp"

using namespace Gularen;
using namespace GularenBridge::Html5;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        Lexer l;
        l.SetBuffer(IO::ToString(argv[1]));
        l.Parse();

        AstBuilder b;
        b.SetTokens(l.GetTokens());
        b.Parse();

        Renderer r;
        r.SetTree(b.GetTree());
        r.Parse();

        IO::Write(r.GetBuffer());

        return 0;
    }

    std::string buffer = IO::ToString("../Tests/Files/Partition.gr");

    Lexer lexer;
    lexer.SetBuffer(buffer);
    lexer.Parse();
    std::vector<Token> tokens = lexer.GetTokens();

    IO::WriteLine(lexer.ToString());

    AstBuilder builder;
    builder.SetTokens(std::move(tokens));
    builder.Parse();

    IO::WriteLine(builder.ToString());

    Renderer renderer;
    renderer.SetTree(builder.GetTree());
    renderer.Parse();
    IO::WriteLine(renderer.GetBuffer());

    return 0;
}
