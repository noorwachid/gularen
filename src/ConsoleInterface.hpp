#include <Gularen/IO.hpp>
#include <Gularen/AstBuilder.hpp>
#include <GularenBridge/Html/Renderer.hpp>
#include <GularenBridge/Json/Renderer.hpp>

using namespace Gularen;
using namespace GularenBridge;

class ConsoleInterface
{
public:
	void onEntry(int size, char** args)
	{
		if (size > 0)
			programExecName = args[0];

		for (int i = 1; i < size; ++i) {
			std::string arg = args[i];

			if (arg[0] == '-') {
				for (size_t j = 1; j < arg.size(); ++j) {
					switch (arg[j]) {
						case 'o':
							if (i + 2 > size || args[i + 1][0] == '-') {
								IO::writeLine("-o requires you to specify the file path");
								terminate(1);
							}
							output = args[i + 1];
							++i;
							break;

							// Renderer's
						case 'r':
							if (i + 2 > size || args[i + 1][0] == '-') {
								IO::writeLine("-r requires you to specify the rendering engine");
								terminate(1);
							}
							outputRenderer = args[i + 1];
							++i;
							break;
						case 's':
							if (i + 2 > size || args[i + 1][0] == '-') {
								IO::writeLine("-t requires you to specify the rendering style");
								terminate(1);
							}
							outputRendererStyle = args[i + 1];
							++i;
							break;

							// Representations
						case 't':
							shownTokens = true;
							break;
						case 'a':
							shownAst = true;
							break;
						case 'b':
							shownRenderedBuffer = true;
							break;

							// Informations
						case 'v':
							writeVersion();
							break;
						case 'h':
							writeHelp();
							terminate(0);
							break;

						default:
							IO::writeLine("Invalid flag: -" + std::string(1, arg[j]));
							break;
					}
				}

			} else
				input = arg;
		}

		parse();
	}

	void parse()
	{
		std::string inputBuffer;

		if (!input.empty())
			inputBuffer = IO::readFile(input);
		else
			inputBuffer = IO::read();

		AstBuilder builder;
		builder.setBuffer(inputBuffer);
		builder.parse();

		if (shownTokens)
			IO::write(builder.getTokensAsString());

		if (shownAst)
			IO::write(builder.getTreeAsString());

		if (shownRenderedBuffer || !output.empty()) {
			std::string outputBuffer;

			if (outputRenderer.substr(0, 4) == "html") {
				Html::Renderer r;
				r.setTree(builder.getTree());

				if (!outputRendererStyle.empty())
					r.setStyle(outputRendererStyle);

				r.parse();

				if (outputRenderer == "html")
					outputBuffer = r.getBuffer();
				if (outputRenderer == "html-content")
					outputBuffer = r.getContentBuffer();
			} else if (outputRenderer == "json") {
				Json::Renderer r;
				r.setTree(builder.getTree());
				r.parse();
				outputBuffer = r.getBuffer();
			} else {
				// TODO: change to Gularen::Renderer as formater
				outputBuffer = inputBuffer;
			}


			if (shownRenderedBuffer)
				IO::write(outputBuffer);

			if (!output.empty())
				IO::writeFile(output, outputBuffer);
		}
	}

	void terminate(int code = 0)
	{
		std::exit(code);
	}

	void writeVersion()
	{
		IO::writeLine("0.1.1");
	}

	void writeHelp()
	{
		IO::writeLine("USAGES:");
		IO::writeLine("    " + programExecName + " [options] file.gr");
		IO::writeLine("    stdin | " + programExecName + " [options]");
		IO::writeLine("");
		IO::writeLine("OPTIONS:");
		IO::writeLine("    -v version");
		IO::writeLine("    -h help");
		IO::writeLine("    -o specify output file path");
		IO::writeLine("    -r specify renderer engine");
		IO::writeLine("    -s specify renderer engine's template");
		IO::writeLine("    -t write tokens to stdout");
		IO::writeLine("    -a write abstract syntax rootNode to stdout");
		IO::writeLine("    -b write output string to stdout");
		IO::writeLine("");
		IO::writeLine("ENGINES:");
		IO::writeLine("    html");
		IO::writeLine("    html-content");
		IO::writeLine("    json");
	}

private:
	std::string input;
	std::string output;
	std::string outputRenderer;
	std::string outputRendererStyle;
	std::string programExecName;

	bool shownAst = false;
	bool shownTokens = false;
	bool shownRenderedBuffer = false;
};
