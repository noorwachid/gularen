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
	void OnEntry(int size, char** args)
	{
		if (size > 0)
			mProgramExecName = args[0];

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
							mOutput = args[i + 1];
							++i;
							break;

							// Renderer's
						case 'r':
							if (i + 2 > size || args[i + 1][0] == '-')
							{
								IO::WriteLine("-r requires you to specify the rendering engine");
								Terminate(1);
							}
							mOutputRenderer = args[i + 1];
							++i;
							break;
						case 's':
							if (i + 2 > size || args[i + 1][0] == '-')
							{
								IO::WriteLine("-t requires you to specify the rendering style");
								Terminate(1);
							}
							mOutputRendererStyle = args[i + 1];
							++i;
							break;

							// Representations
						case 't':
							mShownTokens = true;
							break;
						case 'a':
							mShownAst = true;
							break;
						case 'b':
							mShownRenderedBuffer = true;
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
				mInput = arg;
		}

		Parse();
	}

	void Parse()
	{
		std::string inputBuffer;

		if (!mInput.empty())
			inputBuffer = IO::ReadFile(mInput);
		else
			inputBuffer = IO::Read();

		AstBuilder builder;
		builder.SetBuffer(inputBuffer);
		builder.Parse();

		if (mShownTokens)
			IO::Write(builder.GetTokensAsString());

		if (mShownAst)
			IO::Write(builder.GetTreeAsString());

		if (mShownRenderedBuffer || !mOutput.empty())
		{
			std::string outputBuffer;

			if (mOutputRenderer.substr(0, 4) == "html")
			{
				Html::Renderer r;
				r.SetTree(builder.GetTree());

				if (!mOutputRendererStyle.empty())
					r.SetStyle(mOutputRendererStyle);

				r.Parse();

				if (mOutputRenderer == "html")
					outputBuffer = r.GetBuffer();
				if (mOutputRenderer == "html-content")
					outputBuffer = r.GetContentBuffer();
			}
			else if (mOutputRenderer == "json")
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


			if (mShownRenderedBuffer)
				IO::Write(outputBuffer);

			if (!mOutput.empty())
				IO::WriteFile(mOutput, outputBuffer);
		}
	}

	void Terminate(int code = 0)
	{
		std::exit(code);
	}

	void WriteVersion()
	{
		IO::WriteLine("0.1.1");
	}

	void WriteHelp()
	{
		IO::WriteLine("USAGES:");
		IO::WriteLine("    " + mProgramExecName + " [options] file.gr");
		IO::WriteLine("    stdin | " + mProgramExecName + " [options]");
		IO::WriteLine("");
		IO::WriteLine("OPTIONS:");
		IO::WriteLine("    -v version");
		IO::WriteLine("    -h help");
		IO::WriteLine("    -o specify mOutput file path");
		IO::WriteLine("    -r specify renderer engine");
		IO::WriteLine("    -s specify renderer engine's template");
		IO::WriteLine("    -t Write mTokens to stdout");
		IO::WriteLine("    -a Write abstract syntax mTree to stdout");
		IO::WriteLine("    -b Write mOutput mBuffer to stdout");
		IO::WriteLine("");
		IO::WriteLine("ENGINES:");
		IO::WriteLine("    html");
		IO::WriteLine("    html-content");
		IO::WriteLine("    json");
	}

private:
	std::string mInput;
	std::string mOutput;
	std::string mOutputRenderer;
	std::string mOutputRendererStyle;
	std::string mProgramExecName;

	bool mShownAst = true;
	bool mShownTokens = true;
	bool mShownRenderedBuffer = true;
};

int main(int argc, char** argv)
{
	ConsoleInterface ci;

	ci.OnEntry(argc, argv);

	return 0;
}
