#include "Lexer.hpp"
#include "IO.hpp"

namespace Gularen
{
	Lexer::Lexer()
	{
		reset();
	}

	void Lexer::setBuffer(const std::string& buffer)
	{
		this->buffer = buffer;
		bufferSize = buffer.size();

		reset();
	}

	void Lexer::setTokens(const std::vector<Token>& tokens)
	{
		reset();
		buffer.clear();

		this->tokens = tokens;
	}

	void Lexer::parse()
	{
		reset();
		add(Token(TokenType::documentBegin));
		parseNewline();

		while (isValid())
		{
			// Main switchboard
			switch (getCurrentByte())
			{
				case '*':
					add(Token(TokenType::asterisk));
					skip();
					break;

				case '_':
					add(Token(TokenType::underline));
					skip();
					break;

				case '`':
					skip();
					if (getCurrentByte() == '`')
					{
						skip();
						add(Token(TokenType::teeth));
						std::string buffer;
						while (isValid())
						{
							if (getCurrentByte() == '`' && getNextByte() == '`')
							{
								add(Token(TokenType::text, buffer));
								add(Token(TokenType::teeth));
								skip(2);
								break;
							}
							buffer += getCurrentByte();
							skip();
						}
					}
					else
						add(Token(TokenType::backtick));
					break;

				case '\r':
					skip();
					break;

				case '\n':
				{
					size_t size = 0;
					while (isValid() && getCurrentByte() == '\n')
					{
						++size;
						skip();
					}

					if (!tokens.empty() && tokens.back().Type == TokenType::newline)
						tokens.back().Size += size;
					else
					{
						Token token(TokenType::newline);
						token.Size = size;
						add(std::move(token));
					}

					parseNewline();
					break;
				}

				case '~':
					while (isValid() && getCurrentByte() != '\n')
						skip();
					break;

				case '>':
					if (inHeaderLine)
					{
						skip();
						skipSpaces();
						if (getCurrentByte() == '\'')
						{
							skip();
							std::string symbol;
							while (isValid() && isValidSymbol())
							{
								symbol += getCurrentByte();
								skip();
							}
							if (getCurrentByte() == '\'')
							{
								add(Token(TokenType::anchor, symbol));
								skip();
							}
						}
					}
					skip();
					break;

				case '<':
					add(Token(TokenType::reverseTail, 1));
					skip();
					break;

				case '{':
					add(Token(TokenType::leftCurlyBracket));
					skip();
					parseInlineFunction();
					break;

				case '}':
					add(Token(TokenType::rightCurlyBracket));
					skip();
					break;

				case '\\':
					parseInlineEscapedByte();
					break;

				case '|':
					skip();
					if (!tokens.empty() && tokens.back().Type == TokenType::text)
					{
						// remove blankspaces of previous token
						for (size_t i = tokens.back().Value.size() - 1; i >= 0; --i)
						{
							if (tokens.back().Value[i] != ' ')
							{
								tokens.back().Value = tokens.back().Value.substr(0, i + 1);
								break;
							}
						}
					}
					add(Token(TokenType::pipe));
					skipSpaces();
					break;

				case '#':
				{
					skip();
					std::string symbol;
					while (isValid() && isValidSymbol())
					{
						symbol += getCurrentByte();
						skip();
					}
					Token token;
					token.Type = TokenType::hashSymbol;
					token.Value = symbol;
					add(std::move(token));
					break;
				}
				case '@':
				{
					skip();
					std::string symbol;
					while (isValid() && isValidSymbol())
					{
						symbol += getCurrentByte();
						skip();
					}
					Token token;
					token.Type = TokenType::atSymbol;
					token.Value = symbol;
					add(std::move(token));
					break;
				}

				default:
					if (isValidText())
						parseText();
					else
					{
						if (!tokens.empty() && tokens.back().Type == TokenType::text)
							tokens.back().Value += getCurrentByte();
						else
							add(Token(TokenType::text, std::string(1, getCurrentByte())));
						skip();
					}
					break;
			}
		}

		add(Token(TokenType::documentEnd));
	}

	std::string Lexer::getBuffer()
	{
		return buffer;
	}

	Token& Lexer::getTokenAt(size_t index)
	{
		return tokens[index];
	}

	void Lexer::reset()
	{
		inHeaderLine = false;
		inLink = false;
		inCodeBlock = false;

		bufferIndex = 0;
		tokens.clear();

		currentDepth = 0;
	}

	std::vector<Token>& Lexer::getTokens()
	{
		return tokens;
	}

	std::string Lexer::getTokensAsString()
	{
		std::string buffer;

		for (Token& token: tokens)
			buffer += token.ToString() + "\n";

		return buffer + "\n";
	}

	void Lexer::parseText()
	{
		std::string buffer;

		while (isValid() && isValidText())
		{
			buffer += getCurrentByte();
			skip();
		}

		if (!tokens.empty() && tokens.back().Type == TokenType::text)
			tokens.back().Value += buffer;
		else
			add(Token(TokenType::text, buffer));
	}

	void Lexer::parseQuotedText()
	{
		Token token;
		token.Type = TokenType::quotedText;

		skip();
		while (isValid() && getCurrentByte() != '\'')
		{
			token.Value += getCurrentByte();
			skip();
		}
		skip();
		add(std::move(token));
	}

	void Lexer::parseInlineEscapedByte()
	{
		skip();

		switch (getCurrentByte())
		{
			case '~': // Oneline comments
			case '\\':
			case '*':
			case '_':
			case '`':
			case '{':
			case '}':
			case '<':
				add(Token(TokenType::text, std::string(1, getCurrentByte())));
				skip();
				break;

			default:
			{
				std::string buffer(1, '\\');
				buffer += getCurrentByte();
				add(Token(TokenType::text, buffer));
				skip();
				break;
			}
		}
	}

	void Lexer::parseNewline()
	{
		// state variables
		inHeaderLine = false;

		if (getCurrentByte() == ' ')
		{
			Token token(TokenType::space);
			token.Size = 1;
			skip();

			while (isValid() && getCurrentByte() == ' ')
			{
				++token.Size;
				skip();
			}

			currentDepth = token.Size;
			add(std::move(token));
		}
		else
			currentDepth = 0;

		switch (getCurrentByte())
		{
			case '-':
			{
				size_t size = 0;

				while (isValid() && getCurrentByte() == '-')
				{
					skip();
					++size;
				}

				if (size == 1)
				{
					if (getCurrentByte() == '>' && getNextByte() == ' ')
					{
						add(Token(TokenType::arrow, 1));
						skip();
						skipSpaces();
					}
					else
					{
						add(Token(TokenType::bullet));
						skipSpaces();
					}
				}
				else if (size == 2 && getCurrentByte() == '>' && getNextByte() == ' ')
				{
					add(Token(TokenType::arrow, 2));
					skip();
					skipSpaces();
				}
				else
				{
					add(Token(TokenType::line, size));
					skipSpaces();

					if (inCodeBlock)
					{
						std::string buffer;

						while (isValid())
						{
							if (getCurrentByte() == '\n')
							{
								skip();
								// skip identations or blank lines
								size_t shouldSkipCounter = 0;
								while (isValid() && shouldSkipCounter < currentDepth && getCurrentByte() != '\n')
								{
									skip();
									++shouldSkipCounter;
								}

								if (getCurrentByte() == '-')
								{
									size_t laterSize = 0;

									while (isValid() && getCurrentByte() == '-')
									{
										++laterSize;
										skip();
									}

									if (laterSize == size)
									{
										add(Token(TokenType::rawText, buffer));
										add(Token(TokenType::line, size));
										inCodeBlock = false;
										break;
									}
									else
									{
										if (!buffer.empty())
											buffer += "\n";
										buffer += std::string(laterSize, '-');
									}
								}
								else if (!buffer.empty())
									buffer += "\n";
							}
							else
							{
								buffer += getCurrentByte();
								skip();
							}
						}
					}
				}
				break;
			}

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
			{
				std::string buffer;

				while (isValid() && isValidNumeric())
				{
					buffer += getCurrentByte();
					skip();
				}

				if (getCurrentByte() == '.' && getNextByte() == ' ')
				{
					add(Token(TokenType::numericBullet));
					skip(2);
				}
				else
				{
					if (!tokens.empty() && tokens.back().Type == TokenType::text)
						tokens.back().Value += buffer;
					else
						add(Token(TokenType::text, buffer));
				}

				break;
			}

			case '.':
				if (getNextByte(1) == '.' && getNextByte(2) == ' ')
				{
					add(Token(TokenType::numericBullet));
					skip(3);
				}
				break;

			case '[':
				if (getNextByte(2) == ']')
				{
					Token token(TokenType::checkBox);

					switch (getNextByte(1))
					{
						case ' ':
							token.Size = 1;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						case '-':
							token.Size = 2;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						case '+':
							token.Size = 3;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						default:
							break;
					}
				}
				break;

			case '>':
			{
				inHeaderLine = true;

				skip();
				if (getCurrentByte() == ' ')
				{
					add(Token(TokenType::tail, 1));
					skipSpaces();
					break;
				}
				else if (getCurrentByte() == '>')
				{
					skip();
					if (getCurrentByte() == ' ')
					{
						add(Token(TokenType::tail, 2));
						skipSpaces();
					}
					else if (getCurrentByte() == '>' && getNextByte() == ' ')
					{
						add(Token(TokenType::tail, 3));
						skip();
						skipSpaces();
					}
					else if (getCurrentByte() == '-' && getNextByte(1) == '>' && getNextByte(2) == ' ')
					{
						add(Token(TokenType::arrow, 3));
						skip(2);
						skipSpaces();
					}
					else if (getCurrentByte() == '-' && getNextByte(1) == '-' && getNextByte(2) == '>' &&
						getNextByte(3) == ' ')
					{
						add(Token(TokenType::arrow, 4));
						skip(3);
						skipSpaces();
					}
				}
				else if (getCurrentByte() == '-')
				{
					skip();
					if (getCurrentByte() == '>' && getNextByte() == ' ')
					{
						add(Token(TokenType::arrow, 2));
						skip();
						skipSpaces();
					}
				}
			}

			case '<':
				if (getNextByte() == '<')
				{
					skip(2);
					if (getCurrentByte() == '<')
					{
						add(Token(TokenType::reverseTail, 3));
						skip();
					}
					else
						add(Token(TokenType::reverseTail, 2));
				}
				break;

			case '$':
				add(Token(TokenType::dollar));
				skip();
				skipSpaces();
				parseFunction();
				break;
		}
	}

	void Lexer::parseFunction()
	{
		std::string symbol;

		while (isValid() && isValidSymbol())
		{
			symbol += getCurrentByte();
			skip();
		}

		if (symbol == "image")
		{
			add(Token(TokenType::keywordImage));
			skipSpaces();
		}
		else if (symbol == "table")
		{
			add(Token(TokenType::keywordTable));
			skipSpaces();
		}
		else if (symbol == "code")
		{
			add(Token(TokenType::keywordCode));
			skipSpaces();
			inCodeBlock = true;
		}
		else if (symbol == "admon")
		{
			add(Token(TokenType::KeywordBlock));
			skipSpaces();
		}
		else if (symbol == "file")
		{
			add(Token(TokenType::KeywordFile));
			skipSpaces();
		}
		else if (symbol == "toc")
		{
			add(Token(TokenType::KeywordToc));
			skipSpaces();
		}
		else
			add(Token(TokenType::symbol, symbol));

		skipSpaces();

		if (getCurrentByte() == '\'')
			parseQuotedText();
		else if (isValidSymbol())
		{
			std::string symbol;
			while (isValid() && isValidSymbol())
			{
				symbol += getCurrentByte();
				skip();
			}
			add(Token(TokenType::symbol, symbol));
		}

		skipSpaces();

		if (getCurrentByte() == '{')
		{
			add(Token(TokenType::leftCurlyBracket));
			skip();
		}
	}

	void Lexer::parseInlineFunction()
	{
		switch (getCurrentByte())
		{
			case ':':
				inLink = true;
				add(Token(TokenType::colon));
				skip();
				break;

			case '!':
				inLink = true;
				add(Token(TokenType::exclamationMark));
				skip();
				break;

			case '?':
				inLink = true;
				add(Token(TokenType::questionMark));
				skip();
				break;
		}

		if (getCurrentByte() == '\'')
			parseQuotedText();
		else if (isValidSymbol())
		{
			std::string symbol;
			while (isValid() && isValidSymbol())
			{
				symbol += getCurrentByte();
				skip();
			}
			add(Token(TokenType::symbol, symbol));
		}

		skipSpaces();

		if (getCurrentByte() == '{')
		{
			add(Token(TokenType::leftCurlyBracket));
			skip();
		}
	}

	bool Lexer::isValid()
	{
		return bufferIndex < bufferSize;
	}

	bool Lexer::isValidText()
	{
		char c = getCurrentByte();

		return (c >= 'A' && c <= 'Z') ||
			   (c >= 'a' && c <= 'z') ||
			   (c >= '0' && c <= '9') ||
			   c == ' ' || c == '-' || c == '.' ||
			   c == '!' || c == '?' ||
			   c == '"' || c == '\'' ||
			   c == ';' || c == ':';
	}

	bool Lexer::isValidSymbol()
	{
		char c = getCurrentByte();

		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
	}

	bool Lexer::isValidNumeric()
	{
		char c = getCurrentByte();

		return c >= '0' && c <= '9';
	}

	char Lexer::getCurrentByte()
	{
		return buffer[bufferIndex];
	}

	char Lexer::getNextByte(size_t offset)
	{
		return bufferIndex + offset < bufferSize ? buffer[bufferIndex + offset] : 0;
	}

	void Lexer::skip(size_t offset)
	{
		bufferIndex += offset;
	}

	void Lexer::skipSpaces()
	{
		while (isValid() && getCurrentByte() == ' ')
			skip();
	}

	void Lexer::add(Token&& token)
	{
		tokens.emplace_back(token);
	}
}
