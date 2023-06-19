#include "Lexer.h"
#include <iostream>

namespace Gularen {
	// PUBLIC DEFINITIONS

	Array<Token> Lexer::tokenize(const String& newBuffer) {
		lineCounter = 1;

		buffer = newBuffer;
		bufferCursor = buffer.begin();

		tokens.clear();

		// Guarantied to have one token in the collection
		addToken(Token(TokenType::boDocument));
		parseBlock(true);

		while (isByteCursorInProgress()) {
			switch (getCurrentByte()) {
				case '\n':
					parseNewline();
					advanceByteCursor();

					parseBlock();
					break;

				case '#':
					parseComment();
					break;

				case '>':
					parseArrowID();
					break;

				case '<':
					parseReverseArrowPart();
					break;

				case '-':
					parseDash();
					break;

				case ':':
					parseEmojiShortcode();
					break;

				case '*':
					parseFS(TokenType::asterisk);
					break;

				case '_':
					parseFS(TokenType::underscore);
					break;

				case '`':
					parseFS(TokenType::Backtick);
					break;

				case '\'':
					parseQuote(TokenType::lsQuote, TokenType::rsQuote);
					break;

				case '"':
					parseQuote(TokenType::ldQuote, TokenType::rdQuote);
					break;

				default:
					parseText();
					break;
			}

			advanceByteCursor();
		}

		// Guarantied to have two token in the collection
		addToken(Token(TokenType::eoDocument));

		// TODO: Optimize this copy
		return tokens;
	}

	// PRIVATE DEFINITIONS

	// Main Routine Buffer Parsing

	void Lexer::parseNewline() {
		UintSize size = 0;

		while (isByteCursorInProgress() && getCurrentByte() == '\n') {
			advanceByteCursor();
			++size;
		}

		if (tokens.back().type != TokenType::newline)
			addToken(Token(TokenType::newline, size));
		else
			tokens.back().size += size;

		lineCounter += size;

		retreatByteCursor();
	}

	void Lexer::parseComment() {
		advanceByteCursor();

		while (isByteCursorInProgress() && getCurrentByte() != '\n')
			advanceByteCursor();

		if (getCurrentByte() == '\n' || getCurrentByte() == '\0')
			retreatByteCursor();
	}

	void Lexer::parseFS(TokenType type) {
		addToken(Token(type));
	}

	void Lexer::parseQuote(TokenType opening, TokenType ending) {
		auto& previous = tokens.back();

		if ((previous.type == TokenType::text && previous.content.back() == ' ') ||
			previous.type == TokenType::newline || previous.type == TokenType::boDocument)
			return addToken(Token(opening));

		addToken(Token(ending));
	}

	void Lexer::parseText() {
		if (isCurrentByteText())
			addToken(Token(TokenType::text, consumeTextBytes()));
		else if (tokens.back().type != TokenType::text)
			addToken(Token(TokenType::text, {getCurrentByte(), 0x0}));
		else
			tokens.back().content += getCurrentByte();
	}

	void Lexer::parseSymbol() {
		if (isCurrentByteSymbol())
			addToken(Token(TokenType::symbol, consumeSymbolBytes()));
		else
			parseText();
	}

	void Lexer::parseBlock(bool withAdvancing) {
		inArrowLine = false;

		if (getCurrentByte() == ' ') {
			parseIndentation();

			if (tokens.back().type == TokenType::indentation && getCurrentByte() == '\n') {
				tokens.pop_back();
				parseNewline();
			}
		}

		switch (getCurrentByte()) {
			case '>':
				parseArrow();
				break;

			case '-':
				parseLine();
				break;

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
				parseList();
				break;

			default:
				retreatByteCursor();
				break;
		}

		if (withAdvancing)
			advanceByteCursor();
	}

	void Lexer::parseLine() {
		advanceByteCursor();

		if (getCurrentByte() == '>') {
			advanceByteCursor();

			if (getCurrentByte() == ' ') {
				// "-> "

				inArrowLine = true;
				return addToken(Token(TokenType::sArrow));
			}

			retreatByteCursor();
		}

		retreatByteCursor();
	}

	void Lexer::parseIndentation() {
		UintSize size = 0;

		while (isByteCursorInProgress() && getCurrentByte() == ' ') {
			advanceByteCursor();

			++size;
		}

		UintSize indentSpaceSize = 4;
		UintSize indentLevel = size / indentSpaceSize;
		UintSize indentReminder = size % indentSpaceSize;

		if (indentLevel)
			addToken(Token(TokenType::indentation, indentLevel));

		if (indentReminder) {
			for (UintSize leftOverIndex = 0; leftOverIndex < indentReminder; ++leftOverIndex)
				retreatByteCursor();
		}
	}

	void Lexer::parseArrow() {
		advanceByteCursor();

		if (getCurrentByte() == ' ') {
			// "> "
			inArrowLine = true;
			return addToken(Token(TokenType::arrowHead));
		}

		if (getCurrentByte() == '-') {
			advanceByteCursor();

			if (getCurrentByte() == '>') {
				advanceByteCursor();

				if (getCurrentByte() == ' ') {
					// ">-> "
					inArrowLine = true;
					return addToken(Token(TokenType::arrow));
				}

				retreatByteCursor();
			}

			retreatByteCursor();
		}

		if (getCurrentByte() == '>') {
			advanceByteCursor();

			if (getCurrentByte() == ' ') {
				// ">> "
				inArrowLine = true;
				return addToken(Token(TokenType::arrowTail));
			}

			if (getCurrentByte() == '-') {
				advanceByteCursor();

				if (getCurrentByte() == '>') {
					advanceByteCursor();

					if (getCurrentByte() == ' ') {
						// ">>-> "
						inArrowLine = true;
						return addToken(Token(TokenType::lArrow));
					}

					retreatByteCursor();
				}

				retreatByteCursor();
			}

			if (getCurrentByte() == '>') {
				advanceByteCursor();

				if (getCurrentByte() == ' ') {
					// ">>> "
					inArrowLine = true;
					return addToken(Token(TokenType::lArrowTail));
				}

				if (getCurrentByte() == '-') {
					advanceByteCursor();

					if (getCurrentByte() == '>') {
						advanceByteCursor();

						if (getCurrentByte() == ' ') {
							// ">>>-> "
							inArrowLine = true;
							return addToken(Token(TokenType::xlArrow));
						}

						retreatByteCursor();
					}

					retreatByteCursor();
				}

				retreatByteCursor();
			}

			retreatByteCursor();
		}

		retreatByteCursor();
	}

	void Lexer::parseArrowID() {
		if (!inArrowLine)
			return parseText();

		addToken(Token(TokenType::arrowHead));

		advanceByteCursor();

		if (getCurrentByte() == ' ') {
			advanceByteCursor();

			if (isCurrentByteSymbol()) {
				return parseSymbol();
			}

			retreatByteCursor();
		}

		retreatByteCursor();
	}

	void Lexer::parseReverseArrowPart() {
		String buffer;

		while (isByteCursorInProgress() && getCurrentByte() == '<') {
			buffer += '<';
			advanceByteCursor();
		}

		retreatByteCursor();

		switch (buffer.size()) {
			case 1:
				addToken(TokenType::rArrowHead);
				break;

			case 2:
				addToken(TokenType::rArrowTail);
				break;

			case 3:
				addToken(TokenType::rlArrowTail);
				break;

			default:
				addTokenText(buffer);
				break;
		}
	}

	void Lexer::parseDash() {
		advanceByteCursor();

		if (getCurrentByte() == '-') {
			advanceByteCursor();

			if (getCurrentByte() == '-') {
				return addToken(Token(TokenType::emDash));
			}

			retreatByteCursor();

			return addToken(Token(TokenType::enDash));
		}

		retreatByteCursor();

		return addToken(Token(TokenType::hyphen));
	}

	void Lexer::parseList() {
		String number = consumeNumericBytes();

		if (!number.empty()) {
			advanceByteCursor();

			if (getCurrentByte() == '.') {
				addToken(Token(TokenType::numericBullet, number));
				advanceByteCursor();
				return;
			}

			retreatByteCursor();
		}
	}

	void Lexer::parseEmojiShortcode() {
	}

	// Sub Routine Buffer Parsing

	String Lexer::consumeNumericBytes() {
		String text;

		while (isByteCursorInProgress() && isCurrentByteNumeric()) {
			text += getCurrentByte();

			advanceByteCursor();
		}

		retreatByteCursor();

		return text;
	}

	String Lexer::consumeTextBytes() {
		String text;

		while (isByteCursorInProgress() && isCurrentByteText()) {
			text += getCurrentByte();

			advanceByteCursor();
		}

		retreatByteCursor();

		return text;
	}

	String Lexer::consumeSymbolBytes() {
		String symbol;

		while (isByteCursorInProgress() && isCurrentByteSymbol()) {
			symbol += getCurrentByte();

			advanceByteCursor();
		}

		retreatByteCursor();

		return symbol;
	}

	// Buffer Iterator Helper

	char Lexer::getCurrentByte() {
		return *bufferCursor;
	}

	bool Lexer::isByteCursorInProgress() {
		return bufferCursor < buffer.end();
	}

	void Lexer::advanceByteCursor() {
		++bufferCursor;
	}

	void Lexer::retreatByteCursor() {
		--bufferCursor;
	}

	bool Lexer::isCurrentByteNumeric() {
		char byte = getCurrentByte();

		return (byte >= '0' && byte <= '9');
	}

	bool Lexer::isCurrentByteText() {
		char byte = getCurrentByte();

		return (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') ||

			   byte == ' ' || byte == ',' || byte == '.';
	}

	bool Lexer::isCurrentByteSymbol() {
		char byte = getCurrentByte();

		return (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') ||

			   byte == '-';
	}

	// Token Helper

	void Lexer::addToken(Token&& token) {
		token.line = lineCounter;
		tokens.push_back(token);
	}

	void Lexer::addTokenText(const String& text) {
		if (tokens.back().type == TokenType::text)
			tokens.back().content += text;
		else
			addToken(Token(TokenType::text, text));
	}
}
