#include "gularen/helper/emoji.h"
#include "gularen/parser.h"
#include "gularen/transpiler/html.h"
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

std::string GetContent(const std::string& filePath)
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
	if (argc <= 1)
	{
		std::cout << "please specify the command\n";
		return 0;
	}

	std::string command(argv[1]);

	if (command == "tokenize" && argc > 2)
	{
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Lexer lexer;
		lexer.Set(GetContent(input));
		lexer.Tokenize();

		for (const Gularen::Token& token : lexer.Get())
		{
			std::cout << token.ToString() << '\n';
		}

		return 0;
	}
	else if (command == "parse" && argc > 2)
	{
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		Gularen::Parser parser;
		parser.Set(GetContent(input));
		parser.Parse();
		std::ostringstream out;

		Build(out, parser.Get());

		std::cout << out.str();

		return 0;
	}
	else if (command == "shortcode-to-emoji" && argc > 2)
	{
		std::cout << Gularen::Helper::ShortcodeToEmoji(argv[2]);
		return 0;
	}
	else if (command == "get-shortcodes")
	{
		for (const std::string& shortcode : Gularen::Helper::GetShortcodes())
		{
			std::cout << shortcode << '\n';
		}
		return 0;
	}
	else if (command == "to-html")
	{
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::Transpile(GetContent(input));
		return 0;
	}
	else if (command == "to-html-ls")
	{
		std::string input(argv[2]);

		if (std::filesystem::is_directory(input))
		{
			std::cout << "please specify the input file, make sure its not a directory\n";
			return 0;
		}

		std::cout << Gularen::Transpiler::HTML::TranspileLS(GetContent(input));
		return 0;
	}
	else
	{
		std::cout << "unknown command\n";
		return 1;
	}

	return 0;
}
