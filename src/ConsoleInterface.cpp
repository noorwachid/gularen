#include <Gularen/IO.hpp>
#include <Gularen/Lexer.hpp>
#include <Gularen/AstBuilder.hpp>
#include <GularenBridge/Html/Renderer.hpp>
#include <GularenBridge/Json/Renderer.hpp>

using namespace Gularen;
using namespace GularenBridge;

class ConsoleInterface
{
public:
    std::string input;
    std::string output;
    std::string outputRenderer;
    std::string outputRendererStyle;
    std::string programExecName;

    bool bAst;
    bool bTokens;
    bool bRenderedBuffer;

    void Run(int size, char** args)
    {
        if (size > 0)
            programExecName = args[0];

        for (int i = 1; i < size; ++i)
        {
            std::string arg = args[i];

            if (arg[0] == '-')
            {
                for (size_t j = 1; j < arg.size(); ++j)
                {
                    switch (arg[j])
                    {
                        case 'o':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-o requires you to specify the file path");
                                Terminate(1);
                            }
                            output = args[i + 1];
                            ++i;
                            break;

                        // Renderer's
                        case 'r':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-r requires you to specify the rendering engine");
                                Terminate(1);
                            }
                            outputRenderer = args[i + 1];
                            ++i;
                            break;
                        case 's':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-t requires you to specify the rendering style");
                                Terminate(1);
                            }
                            outputRendererStyle = args[i + 1];
                            ++i;
                            break;

                        // Representations
                        case 't':
                            bTokens = true;
                            break;
                        case 'a':
                            bAst = true;
                            break;
                        case 'b':
                            bRenderedBuffer = true;
                            break;

                        // Informations
                        case 'v':
                            WriteVersion();
                            break;
                        case 'h':
                            WriteHelp();
                            Terminate(0);
                            break;

                        default:
                            IO::WriteLine("Invalid flag: -" + std::string(1, arg[j]));
                            break;
                    }
                }

            }
            else
                input = arg;
        }

        Parse();
    }

    void Parse()
    {
        std::string inputBuffer;

        if (!input.empty())
            inputBuffer = IO::ReadFile(input);
        else
            inputBuffer = IO::Read();

        AstBuilder builder;
        builder.SetBuffer(inputBuffer);
        builder.Parse();

        if (bTokens)
            IO::Write(builder.GetTokensAsString());

        if (bAst)
            IO::Write(builder.GetTreeAsString());

        if (bRenderedBuffer || !output.empty())
        {
            std::string outputBuffer;

            if (outputRenderer.substr(0, 4) == "html")
            {
                Html::Renderer r;
                r.SetTree(builder.GetTree());

                if (!outputRendererStyle.empty())
                    r.SetStyle(outputRendererStyle);

                r.Parse();

                if (outputRenderer == "html")
                    outputBuffer = r.GetBuffer();
                if (outputRenderer == "html-content")
                    outputBuffer = r.GetContentBuffer();
            }
            else if (outputRenderer == "json")
            {
                Json::Renderer r;
                r.SetTree(builder.GetTree());
                r.Parse();
                outputBuffer = r.GetBuffer();
            }
            else
            {
                // TODO: change to Gularen::Renderer as formater
                outputBuffer = inputBuffer;
            }


            if (bRenderedBuffer)
                IO::Write(outputBuffer);

            if (!output.empty())
                IO::WriteFile(output, outputBuffer);
        }
    }

    void Terminate(int code = 0)
    {
        std::exit(code);
    }

    void WriteVersion()
    {
        IO::WriteLine("0.0.1");
    }

    void WriteHelp()
    {
        IO::WriteLine("USAGES:");
        IO::WriteLine("    " + programExecName + " [options] file.gr");
        IO::WriteLine("    stdin | " + programExecName + " [options]");
        IO::WriteLine("");
        IO::WriteLine("OPTIONS:");
        IO::WriteLine("    -v version");
        IO::WriteLine("    -h help");
        IO::WriteLine("    -o specify output file path");
        IO::WriteLine("    -r specify renderer engine");
        IO::WriteLine("    -s specify renderer engine's template");
        IO::WriteLine("    -t write tokens to stdout");
        IO::WriteLine("    -a write abstract syntax tree to stdout");
        IO::WriteLine("    -b write output buffer to stdout");
        IO::WriteLine("");
        IO::WriteLine("ENGINES:");
        IO::WriteLine("    html");
        IO::WriteLine("    html-content");
        IO::WriteLine("    json");
    }
private:
};


int main(int argc, char** argv)
{
    ConsoleInterface ci;

    ci.Run(argc, argv);

    return 0;
}
