#include "Gularen/Parser.h"
#include "Gularen/Transpiler/HTML.h"
#include "Gularen/Helper/Emoji.h"
#include <fstream>
#include <iostream>
#include <sstream>

void Build(std::ostream& out, const Gularen::NodePtr& node, size_t depth = 0)
{
	if (!node)
		return;

	out << std::string(depth * 2, ' ') << node->ToString() << '\n';

	for (const Gularen::NodePtr& childNode : node->children)
	{
		Build(out, childNode, depth + 1);
	}
}

std::string ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	std::string content;

	file.seekg(0, std::ios::end);
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	return content;
}

int main(int argc, char** argv)
{
	std::string version = "2.0.1";

	if (argc <= 1)
	{
		std::cout << "please specify the command\n";
		return 0;
	}

	std::string command(argv[1]);

	if (command == "tokenize")
	{
		if (argc < 2)
		{
			std::cout << "please specify the input file\n";
			return 0;
		}

		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Lexer lexer;
		lexer.Set(ReadFile(input));
		lexer.Tokenize();

		for (const Gularen::Token& token : lexer.GetTokens())
		{
			std::cout << token.ToString() << '\n';
		}

		return 0;
	}
	else if (command == "parse")
	{
		if (argc < 2)
		{
			std::cout << "please specify the input file\n";
			return 0;
		}

		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Parser parser;
		parser.Set(ReadFile(input));
		parser.Parse();
		std::ostringstream out;

		Build(out, parser.GetRoot());

		std::cout << out.str();

		return 0;
	}
	else if (command == "html")
	{
		if (argc < 2)
		{
			std::cout << "please specify the input file\n";
			return 0;
		}

		std::string input;
		std::string template_;

		for (int i = 2; i < argc; ++i)
		{
			std::string argument = argv[i];
			if (argument == "--template" && i + 1 < argc)
			{
				template_ = argv[i + 1];
				continue;
			}

			input = argument;
		}

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		if (!template_.empty())
		{
			if (std::filesystem::is_directory(template_))
			{
				std::cout << "please specify the template file, make sure its not a directory\n";
				return 0;
			}

			std::string templateMarker = "<!-- content -->";
			std::string templateContent = ReadFile(template_);

			size_t startPos = templateContent.find(templateMarker);

			if (startPos == std::string::npos)
			{
				std::cout << "pleace add special comment: '" << templateMarker << "' to the template file\n";
				return 0;
			}

			templateContent.replace(
				startPos, templateMarker.length(), Gularen::Transpiler::HTML::Transpile(ReadFile(input))
			);

			std::cout << templateContent;
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::Transpile(ReadFile(input));
		return 0;
	}
	else if (command == "version")
	{
		std::cout << version << "\n";
	}
	else
	{
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
