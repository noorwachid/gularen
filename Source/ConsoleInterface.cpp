#include <Gularen/IO.hpp>
#include <Gularen/Lexer.hpp>
#include <Gularen/AstBuilder.hpp>
#include <GularenBridge/Html5/Renderer.hpp>
#include <GularenBridge/Json/Renderer.hpp>

using namespace Gularen;
using namespace GularenBridge;

class ConsoleInterface
{
public:
    std::string inputBuffer;
    std::string input;
    std::string output;
    std::string outputRenderer;

    bool bTree;
    bool bTokens;
    bool bRenderedBuffer;

    void Run(int size, char** args)
    {
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

                        case 'i':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-i requires you to specify the file path");
                                Terminate(1);
                            }
                            input = args[i + 1];
                            ++i;
                            break;

                        case 'e':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-o requires you to specify the string buffer");
                                Terminate(1);
                            }
                            inputBuffer = args[i + 1];
                            ++i;
                            break;

                        case 't':
                            bTree = true;
                            break;

                        case 'k':
                            bTokens = true;
                            break;

                        case 'r':
                            if (i + 2 > size || args[i + 1][0] == '-')
                            {
                                IO::WriteLine("-r requires you to specify the rendering engine");
                                Terminate(1);
                            }
                            outputRenderer = args[i + 1];
                            ++i;
                            break;

                        case 'g':
                            bRenderedBuffer = true;
                            break;

                        case 'v':
                            WriteVersion();
                            break;

                        case 'h':
                            WriteHelp();
                            break;

                        default:
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
        AstBuilder builder;
        builder.SetBuffer(!inputBuffer.empty() ? inputBuffer : IO::ReadFile(input));
        builder.Parse();

        IRenderer* r;

        if (bTokens)
            IO::Write(builder.GetTokensAsString());

        if (bTree)
            IO::Write(builder.GetTreeAsString());

        if (!outputRenderer.empty())
        {
            if (outputRenderer == "html5")
                r = new Html5::Renderer();
            if (outputRenderer == "json")
                r = new Json::Renderer();
            else
                // TODO: change to Gularen::Renderer as formater
                r = new Html5::Renderer();

            r->SetTree(builder.GetTree());
            r->Parse();

            if (bRenderedBuffer)
                IO::Write(r->GetBuffer());

            if (!output.empty())
                IO::WriteFile(output, r->GetBuffer());
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
        IO::WriteLine("    -v version");
        IO::WriteLine("    -h help");
        IO::WriteLine("    -e parse string buffer");
        IO::WriteLine("    -i specify input file path");
        IO::WriteLine("    -o specify output file path");
        IO::WriteLine("    -r specify renderer engine");
        IO::WriteLine("    -k write tokens to console buffer");
        IO::WriteLine("    -t write tree to console buffer");
        IO::WriteLine("    -g write generated buffer to console buffer");
    }
private:
};


int main(int argc, char** argv)
{
    ConsoleInterface ci;

    if (argc > 1)
        ci.Run(argc, argv);
    else
        ci.WriteHelp();

    return 0;
}
