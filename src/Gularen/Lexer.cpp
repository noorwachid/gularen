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
		add(Token(TokenType::openDocument));
		parseNewline();

		while (isValid()) {
			// Main switchboard
			switch (getCurrentByte()) {
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
					if (getCurrentByte() == '`') {
						skip();
						add(Token(TokenType::teeth));
						std::string buffer;
						while (isValid()) {
							if (getCurrentByte() == '`' && getNextByte() == '`') {
								add(Token(TokenType::buffer, buffer));
								add(Token(TokenType::teeth));
								skip(2);
								break;
							}
							buffer += getCurrentByte();
							skip();
						}
					} else
						add(Token(TokenType::backtick));
					break;

				case '\r':
					skip();
					break;

				case '\n': {
					size_t size = 0;
					while (isValid() && getCurrentByte() == '\n') {
						++size;
						skip();
					}

					if (!tokens.empty() && tokens.back().type == TokenType::newline)
						tokens.back().size += size;
					else {
						Token token(TokenType::newline);
						token.size = size;
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
					if (inHeaderLine) {
						skip();
						skipSpaces();
						if (getCurrentByte() == '\'') {
							skip();
							std::string symbol;
							while (isValid() && isValidSymbol()) {
								symbol += getCurrentByte();
								skip();
							}
							if (getCurrentByte() == '\'') {
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
					add(Token(TokenType::openCurlyBracket));
					skip();
					parseInlineFunction();
					break;

				case '}':
					add(Token(TokenType::closeCurlyBracket));
					skip();
					break;

				case '\\':
					skip();
					add(Token(TokenType::buffer, std::string(1, getCurrentByte())));
					skip();
					break;

				case '\'':
					parseQuoteBuffer(TokenType::openDoubleQuote, TokenType::openSingleQuote, TokenType::closeSingleQuote);
					break;

				case '"':
					if (!inInterpolatedBuffer)
						parseQuoteBuffer(TokenType::openSingleQuote, TokenType::openDoubleQuote, TokenType::closeDoubleQuote);
					else
						parseInterpolatedString();
					break;

				case '|':
					skip();
					if (!tokens.empty() && tokens.back().type == TokenType::buffer) {
						// remove blankspaces of previous token
						for (size_t i = tokens.back().value.size() - 1; i >= 0; --i) {
							if (tokens.back().value[i] != ' ') {
								tokens.back().value = tokens.back().value.substr(0, i + 1);
								break;
							}
						}
					}
					add(Token(TokenType::pipe));
					skipSpaces();
					break;

				case '#': {
					skip();
					std::string symbol;
					while (isValid() && isValidSymbol()) {
						symbol += getCurrentByte();
						skip();
					}
					Token token;
					token.type = TokenType::hash;
					token.value = symbol;
					add(std::move(token));
					break;
				}
				case '@': {
					skip();
					std::string symbol;
					while (isValid() && isValidSymbol()) {
						symbol += getCurrentByte();
						skip();
					}
					Token token;
					token.type = TokenType::at;
					token.value = symbol;
					add(std::move(token));
					break;
				}

				default:
					if (isValidBuffer())
						parseBuffer();
					else {
						if (!tokens.empty() && tokens.back().type == TokenType::buffer)
							tokens.back().value += getCurrentByte();
						else
							add(Token(TokenType::buffer, std::string(1, getCurrentByte())));
						skip();
					}
					break;
			}
		}

		add(Token(TokenType::closeDocument));
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
		inInterpolatedBuffer = false;
		inCodeBlock = false;

		bufferIndex = 0;
		tokens.clear();

		previousByte = 0;
		previousType = TokenType::unknown;

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

	void Lexer::parseBuffer()
	{
		std::string buffer;

		while (isValid() && isValidBuffer()) {
			buffer += getCurrentByte();
			skip();
		}

		if (!tokens.empty() && tokens.back().type == TokenType::buffer)
			tokens.back().value += buffer;
		else
			add(Token(TokenType::buffer, buffer));
	}

	void Lexer::parseQuoteBuffer(TokenType previousType, TokenType openType, TokenType closeType)
	{
		char previousByte = getPreviousByte();

		add(Token((
			previousByte == ' ' ||
			previousByte == '\t' ||
			previousByte == '\n' ||
			previousByte == '\0' ||
			this->previousType == previousType)
				? openType
				: closeType));
		skip();
	}

	void Lexer::parseString()
	{
		Token token;
		token.type = TokenType::buffer;

		skip();
		while (isValid() && getCurrentByte() != '\'') {
			if (getCurrentByte() == '\\')
				skip();

			token.value += getCurrentByte();
			skip();
		}
		skip();
		add(std::move(token));
	}

	void Lexer::parseInterpolatedString()
	{
		if (!inInterpolatedBuffer) {
			add(Token(TokenType::openInterpolation));
			inInterpolatedBuffer = true;
		} else {
			add(Token(TokenType::closeInterpolation));
			inInterpolatedBuffer = false;
		}
		skip();
	}
	void Lexer::parseNewline()
	{
		// state variables
		inHeaderLine = false;

		if (getCurrentByte() == ' ') {
			Token token(TokenType::space);
			token.size = 1;
			skip();

			while (isValid() && getCurrentByte() == ' ') {
				++token.size;
				skip();
			}

			currentDepth = token.size;
			add(std::move(token));
		} else
			currentDepth = 0;

		switch (getCurrentByte()) {
			case '-': {
				size_t size = 0;

				while (isValid() && getCurrentByte() == '-') {
					skip();
					++size;
				}

				if (size == 1) {
					if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::arrow, 1));
						skip();
						skipSpaces();
					} else {
						add(Token(TokenType::bullet));
						skipSpaces();
					}
				} else if (size == 2 && getCurrentByte() == '>' && getNextByte() == ' ') {
					add(Token(TokenType::arrow, 2));
					skip();
					skipSpaces();
				} else {
					add(Token(TokenType::line, size));
					skipSpaces();

					if (inCodeBlock) {
						std::string buffer;

						while (isValid()) {
							if (getCurrentByte() == '\n') {
								skip();
								// skip identations or blank lines
								size_t shouldSkipCounter = 0;
								while (isValid() && shouldSkipCounter < currentDepth && getCurrentByte() != '\n') {
									skip();
									++shouldSkipCounter;
								}

								if (getCurrentByte() == '-') {
									size_t laterSize = 0;

									while (isValid() && getCurrentByte() == '-') {
										++laterSize;
										skip();
									}

									if (laterSize == size) {
										add(Token(TokenType::buffer, buffer));
										add(Token(TokenType::line, size));
										inCodeBlock = false;
										break;
									} else {
										if (!buffer.empty())
											buffer += "\n";
										buffer += std::string(laterSize, '-');
									}
								} else if (!buffer.empty())
									buffer += "\n";
							} else {
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
			case '0': {
				std::string buffer;

				while (isValid() && isValidNumeric()) {
					buffer += getCurrentByte();
					skip();
				}

				if (getCurrentByte() == '.' && getNextByte() == ' ') {
					add(Token(TokenType::numericBullet));
					skip(2);
				} else {
					if (!tokens.empty() && tokens.back().type == TokenType::buffer)
						tokens.back().value += buffer;
					else
						add(Token(TokenType::buffer, buffer));
				}

				break;
			}

			case '.':
				if (getNextByte(1) == '.' && getNextByte(2) == ' ') {
					add(Token(TokenType::numericBullet));
					skip(3);
				}
				break;

			case '[':
				if (getNextByte(2) == ']') {
					Token token(TokenType::checkBox);

					switch (getNextByte(1)) {
						case ' ':
							token.size = 1;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						case '-':
							token.size = 2;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						case '+':
							token.size = 3;
							add(std::move(token));
							skip(3);
							skipSpaces();
							break;

						default:
							break;
					}
				}
				break;

			case '>': {
				inHeaderLine = true;

				skip();
				if (getCurrentByte() == ' ') {
					add(Token(TokenType::tail, 1));
					skipSpaces();
					break;
				} else if (getCurrentByte() == '>') {
					skip();
					if (getCurrentByte() == ' ') {
						add(Token(TokenType::tail, 2));
						skipSpaces();
					} else if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::tail, 3));
						skip();
						skipSpaces();
					} else if (getCurrentByte() == '-' && getNextByte(1) == '>' && getNextByte(2) == ' ') {
						add(Token(TokenType::arrow, 3));
						skip(2);
						skipSpaces();
					} else if (getCurrentByte() == '-' && getNextByte(1) == '-' && getNextByte(2) == '>' &&
						getNextByte(3) == ' ') {
						add(Token(TokenType::arrow, 4));
						skip(3);
						skipSpaces();
					}
				} else if (getCurrentByte() == '-') {
					skip();
					if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::arrow, 2));
						skip();
						skipSpaces();
					}
				}
			}

			case '<':
				if (getNextByte() == '<') {
					skip(2);
					if (getCurrentByte() == '<') {
						add(Token(TokenType::reverseTail, 3));
						skip();
					} else
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

		while (isValid() && isValidSymbol()) {
			symbol += getCurrentByte();
			skip();
		}

		if (symbol == "image") {
			add(Token(TokenType::imageKeyword));
			skipSpaces();
		} else if (symbol == "table") {
			add(Token(TokenType::tableKeyword));
			skipSpaces();
		} else if (symbol == "code") {
			add(Token(TokenType::codeKeyword));
			skipSpaces();
			inCodeBlock = true;
		} else if (symbol == "admon") {
			add(Token(TokenType::admonitionKeyword));
			skipSpaces();
		} else if (symbol == "file") {
			add(Token(TokenType::fileKeyword));
			skipSpaces();
		} else if (symbol == "toc") {
			add(Token(TokenType::tocKeyword));
			skipSpaces();
		} else
			add(Token(TokenType::symbol, symbol));

		skipSpaces();

		if (getCurrentByte() == '\'')
			parseString();
		else if (getCurrentByte() == '"')
			parseInterpolatedString();
		else if (isValidSymbol()) {
			std::string symbol;
			while (isValid() && isValidSymbol()) {
				symbol += getCurrentByte();
				skip();
			}
			add(Token(TokenType::symbol, symbol));
		}

		skipSpaces();

		if (getCurrentByte() == '{') {
			add(Token(TokenType::openCurlyBracket));
			skip();
		}
	}

	void Lexer::parseInlineFunction()
	{
		switch (getCurrentByte()) {
			case ':':
				inInterpolatedBuffer = true;
				add(Token(TokenType::colon));
				skip();
				break;

			case '!':
				inInterpolatedBuffer = true;
				add(Token(TokenType::exclamationMark));
				skip();
				break;

			case '?':
				inInterpolatedBuffer = true;
				add(Token(TokenType::questionMark));
				skip();
				break;

			case '^':
				inInterpolatedBuffer = true;
				add(Token(TokenType::caret));
				skip();
				break;

			case '&':
				inInterpolatedBuffer = true;
				add(Token(TokenType::ampersand));
				skip();
				break;
		}

		if (getCurrentByte() == '\'')
			parseString();
		else if (isValidSymbol()) {
			std::string symbol;
			while (isValid() && isValidSymbol()) {
				symbol += getCurrentByte();
				skip();
			}
			add(Token(TokenType::symbol, symbol));
		}

		skipSpaces();

		if (getCurrentByte() == '{') {
			add(Token(TokenType::openCurlyBracket));
			skip();
		}
	}

	bool Lexer::isValid()
	{
		return bufferIndex < bufferSize;
	}

	bool Lexer::isValidBuffer()
	{
		char c = getCurrentByte();

		return (c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == ' ' || c == '-' || c == '.' ||
			c == '!' || c == '?' ||
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

	char Lexer::getPreviousByte()
	{
		return previousByte;
	}

	char Lexer::getNextByte(size_t offset)
	{
		return bufferIndex + offset < bufferSize ? buffer[bufferIndex + offset] : 0;
	}

	void Lexer::skip(size_t offset)
	{
		previousByte = getCurrentByte();
		bufferIndex += offset;
	}

	void Lexer::skipSpaces()
	{
		while (isValid() && getCurrentByte() == ' ')
			skip();
	}

	void Lexer::add(Token&& token)
	{
		previousType = token.type;
		tokens.emplace_back(token);
	}
}
