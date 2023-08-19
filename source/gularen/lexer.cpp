#include "gularen/lexer.h"
#include "gularen/token.h"
#include <fstream>
#include <iostream>

namespace Gularen
{
	bool IsDigit(char letter)
	{
		return letter >= '0' && letter <= '9';
	}

	bool IsDate(std::string_view text)
	{
		if (text.size() == 10 && IsDigit(text[0]) && IsDigit(text[1]) && IsDigit(text[2]) && IsDigit(text[3]) &&
			text[4] == '-' && IsDigit(text[5]) && IsDigit(text[6]) && text[7] == '-' && IsDigit(text[8]) &&
			IsDigit(text[9]))
		{
			return true;
		}
		return false;
	}

	bool IsTime(std::string_view text)
	{
		if (text.size() == 5 && IsDigit(text[0]) && IsDigit(text[1]) && text[2] == ':' && IsDigit(text[3]) &&
			IsDigit(text[4]))
		{
			return true;
		}

		if (text.size() == 8 && IsDigit(text[0]) && IsDigit(text[1]) && text[2] == ':' && IsDigit(text[3]) &&
			IsDigit(text[4]) && text[5] == ':' && IsDigit(text[6]) && IsDigit(text[7]))
		{
			return true;
		}

		return false;
	}

	bool IsDateTime(std::string_view text)
	{
		if (text.size() == 16 && IsDate(text.substr(0, 10)) && IsTime(text.substr(11, 5)))
		{
			return true;
		}
		if (text.size() == 19 && IsDate(text.substr(0, 10)) && IsTime(text.substr(11, 8)))
		{
			return true;
		}
		return false;
	}

	void Lexer::Set(const std::string& content)
	{
		this->_content = content;
	}

	void Lexer::Tokenize()
	{
		if (_content.empty())
			return;

		this->_tokens.clear();
		this->_index = 0;
		this->_position.line = 0;
		this->_position.column = 0;

		TokenizeBlock();

		while (Check(0))
		{
			TokenizeInline();
		}

		if (!_tokens.empty() &&
			(_tokens.back().type == TokenType::Newline || _tokens.back().type == TokenType::NewlinePlus))
		{
			_tokens.back().type = TokenType::End;
			_tokens.back().value = "\\0";
			_tokens.back().range.end = _tokens.back().range.begin;
		}
		else
		{
			Add(TokenType::End, "\\0");
		}
	}

	const Tokens& Lexer::Get() const
	{
		return _tokens;
	}

	bool Lexer::Check(size_t offset)
	{
		return _index + offset < _content.size();
	}

	bool Lexer::Is(size_t offset, char c)
	{
		return _content[_index + offset] == c;
	}

	bool Lexer::IsSymbol(size_t offset)
	{
		char c = _content[_index + offset];
		return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-';
	}

	char Lexer::Get(size_t offset)
	{
		return _content[_index + offset];
	}

	void Lexer::Advance(size_t offset)
	{
		_index += 1 + offset;
		_position.column += 1 + offset;
	}

	void Lexer::Retreat(size_t offset)
	{
		_index -= offset;
		_position.column -= offset;
	}

	size_t Lexer::Count(char c)
	{
		size_t count = 0;
		while (Check(0) && Is(0, c))
		{
			++count;
			Advance(0);
		}
		return count;
	}

	void Lexer::Add(TokenType type, const std::string& value, Position begin, Position end)
	{
		Token token;
		token.type = type;
		token.value = value;
		token.range.begin = begin;
		token.range.end = end;
		_tokens.push_back(token);
	}

	void Lexer::Add(TokenType type, const std::string& value, Position begin)
	{
		Token token;
		token.type = type;
		token.value = value;
		token.range.begin = begin;
		token.range.end = _position;
		--token.range.end.column;
		_tokens.push_back(token);
	}

	void Lexer::Add(TokenType type, const std::string& value)
	{
		Token token;
		token.type = type;
		token.value = value;
		token.range.begin = _position;
		token.range.end = _position;
		_tokens.push_back(token);
	}

	void Lexer::AddText(const std::string value)
	{
		if (!_tokens.empty() && _tokens.back().type == TokenType::Text)
		{
			_tokens.back().value += value;
			_tokens.back().range.end.column += value.size();
		}
		else
		{
			if (!_tokens.empty())
			{
				Position beginPosition = _position;
				beginPosition.column -= value.size() - 1;
				Token token;
				token.type = TokenType::Text;
				token.value = value;
				token.range.begin = beginPosition;
				token.range.end = _position;
				_tokens.push_back(token);
			}
			else
			{
				Position beginPosition = _position;
				beginPosition.column -= beginPosition.column > value.size() ? value.size() : 0;
				Token token;
				token.type = TokenType::Text;
				token.value = value;
				token.range.begin = beginPosition;
				token.range.end = _position;
				token.range.end.column -= 1;
				_tokens.push_back(token);
			}
		}
	}

	void Lexer::TokenizeInline()
	{
		switch (Get(0))
		{
			case ' ':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				TokenizeText();
				break;

			case '\n': {
				Position beginPosition = _position;
				size_t counter = Count('\n');

				if (counter == 1)
				{
					Add(TokenType::Newline, "\\n", beginPosition);
					_position.line += counter;
					_position.column = 0;
				}
				else
				{
					_position.line += counter - 1;
					_position.column = 0;
					Token token;
					token.type = TokenType::NewlinePlus;
					token.value = "\\n";
					token.range.begin = beginPosition;
					token.range.end = _position;
					_tokens.push_back(token);
					_position.line += 1;
				}

				TokenizeBlock();
				break;
			}

			case '\\':
				Advance(0);
				if (Check(0))
				{
					AddText(_content.substr(_index, 1));
					Advance(0);
				}
				break;

			case '~': {
				Position beginPosition = _position;
				size_t commentIndex = _index;
				size_t commentSize = 1;
				Advance(0);
				while (Check(0) && !Is(0, '\n'))
				{
					++commentSize;
					Advance(0);
				}
				Add(TokenType::Comment, _content.substr(commentIndex, commentSize), beginPosition);
				break;
			}

			case '\'':
				TokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::LDQuo, TokenType::LSQuo, "‘", TokenType::RSQuo,
					"’"
				);
				Advance(0);
				break;

			case '"':
				TokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::LSQuo, TokenType::LDQuo, "“", TokenType::RDQuo,
					"”"
				);
				Advance(0);
				break;

			case '-': {
				if (Check(2) && Is(1, '-') && Is(2, '-'))
				{
					Add(TokenType::EmDash, "–", Position(_position.line, _position.column),
						Position(_position.line, _position.column + 2));
					Advance(2);
					break;
				}
				if (Check(1) && Is(1, '-'))
				{
					Add(TokenType::EnDash, "–", Position(_position.line, _position.column),
						Position(_position.line, _position.column + 1));
					Advance(1);
					break;
				}
				Add(TokenType::Hyphen, "-");
				Advance(0);
				break;
			}

			case ':': {
				Position emojiMarkerPosition = _position;
				Advance(0);

				if (Check(0))
				{
					size_t emojiIndex = _index;
					size_t emojiSize = 0;

					while (Check(0) && (Get(0) >= 'a' && Get(0) <= 'z' || Is(0, '-')))
					{
						if (Is(0, ':'))
						{
							break;
						}

						++emojiSize;
						Advance(0);
					}

					if (Is(0, ':') && emojiSize > 0)
					{
						Add(TokenType::EmojiMark, ":", emojiMarkerPosition, emojiMarkerPosition);
						Add(TokenType::EmojiCode, _content.substr(emojiIndex, emojiSize),
							Position(emojiMarkerPosition.line, emojiMarkerPosition.column + 1));
						Add(TokenType::EmojiMark, ":");
						Advance(0);
						break;
					}

					AddText(":" + _content.substr(emojiIndex, emojiSize));
					break;
				}

				AddText(":");
				break;
			}

			case '{': {
				Position beginPosition = _position;

				Advance(0);

				if (Check(0))
				{
					size_t depth = 0;
					std::string source;

					while (Check(0))
					{
						if (Is(0, '}') && depth == 0)
						{
							Advance(0);
							break;
						}

						if (Is(0, '{'))
						{
							++depth;
						}

						if (Is(0, '\\'))
						{
							Advance(0);
						}

						source += Get(0);
						Advance(0);
					}

					Add(TokenType::CurlyOpen, "{", beginPosition, beginPosition);
					++beginPosition.column;
					Add(TokenType::CodeSource, source, beginPosition,
						Position(beginPosition.line, beginPosition.column + source.size() - 1));
					beginPosition.column += source.size();
					Add(TokenType::CurlyClose, "}", beginPosition, beginPosition);

					if (Is(0, '('))
					{
						Advance(0);
						std::string lang;

						if (Check(0) && IsSymbol(0))
						{
							while (Check(0) && IsSymbol(0))
							{
								lang += Get(0);
								Advance(0);
							}

							if (Is(0, ')'))
							{
								++beginPosition.column;
								Add(TokenType::ParenOpen, "(", beginPosition, beginPosition);
								++beginPosition.column;
								Add(TokenType::ResourceLabel, lang, beginPosition,
									Position(beginPosition.line, beginPosition.column + lang.size() - 1));
								beginPosition.column += lang.size();
								Add(TokenType::ParenClose, ")", beginPosition, beginPosition);
								Advance(0);
								break;
							}

							AddText("(" + lang);
							break;
						}
						else
						{
							AddText("(");
						}
					}
					break;
				}

				AddText("{");
			}

			case '[': {
				Position beginPosition = _position;

				Advance(0);

				if (Check(0))
				{
					bool idMarkerExists = false;
					size_t idMarkerIndex = 0;
					size_t depth = 0;
					std::string resource;

					while (Check(0))
					{
						if (Is(0, ']') && depth == 0)
						{
							Advance(0);
							break;
						}

						if (Is(0, '['))
						{
							++depth;
						}

						if (Is(0, '>'))
						{
							idMarkerExists = true;
							idMarkerIndex = resource.size();
						}

						if (Is(0, '\\'))
						{
							Advance(0);
						}

						resource += Get(0);
						Advance(0);
					}

					if (idMarkerExists)
					{
						Add(TokenType::SquareOpen, "[", beginPosition, beginPosition);
						++beginPosition.column;
						Add(TokenType::Resource, resource.substr(0, idMarkerIndex), beginPosition,
							Position(beginPosition.line, beginPosition.column + idMarkerIndex - 1));
						beginPosition.column += idMarkerIndex;
						Add(TokenType::ResourceIDMark, ">", beginPosition, beginPosition);
						++beginPosition.column;
						Add(TokenType::ResourceID, resource.substr(idMarkerIndex + 1), beginPosition,
							Position(beginPosition.line, beginPosition.column + resource.size() - idMarkerIndex - 2));
						beginPosition.column += resource.size() - idMarkerIndex - 1;
						Add(TokenType::SquareClose, "]", beginPosition, beginPosition);
					}
					else
					{
						Add(TokenType::SquareOpen, "[", beginPosition, beginPosition);
						++beginPosition.column;
						Add(TokenType::Resource, resource, beginPosition,
							Position(beginPosition.line, beginPosition.column + resource.size() - 1));
						beginPosition.column += resource.size();
						Add(TokenType::SquareClose, "]", beginPosition, beginPosition);
					}

					if (Is(0, '('))
					{
						Advance(0);

						if (Check(0))
						{
							size_t depth = 0;
							std::string label;

							while (Check(0))
							{
								if (Is(0, ')') && depth == 0)
								{
									Advance(0);
									break;
								}

								if (Is(0, '('))
								{
									++depth;
								}

								if (Is(0, '\\'))
								{
									Advance(0);
								}

								label += Get(0);
								Advance(0);
							}

							++beginPosition.column;
							Add(TokenType::ParenOpen, "(", beginPosition, beginPosition);
							++beginPosition.column;
							Add(TokenType::ResourceLabel, label, beginPosition,
								Position(beginPosition.line, beginPosition.column + label.size() - 1));
							beginPosition.column += label.size();
							Add(TokenType::ParenClose, ")", beginPosition, beginPosition);
							break;
						}
						else
						{
							AddText("(");
						}
					}
					break;
				}

				AddText("[");
				break;
			}

			case '!':
				if (Check(1) && Is(1, '['))
				{
					Add(TokenType::PresentMark, "!");
					Advance(0);
					// see inline [
					break;
				}
				Advance(0);
				AddText("!");
				break;

			case '?':
				if (Check(1) && Is(1, '['))
				{
					Add(TokenType::IncludeMark, "?");
					Advance(0);
					// see inline [
					break;
				}
				Advance(0);
				AddText("?");
				break;

			case '^':
			case '=': {
				Position beginPosition = _position;
				std::string original(1, Get(0));
				if (Check(2) && Is(1, '[') && IsSymbol(2))
				{
					Advance(1);
					size_t idIndex = _index;
					size_t idSize = 0;
					while (Check(0) && IsSymbol(0))
					{
						++idSize;
						Advance(0);
					}
					if (idSize > 0 && Check(0) && Is(0, ']'))
					{
						if (original[0] == '^')
						{
							Add(TokenType::JumpMark, original, beginPosition, beginPosition);
							++beginPosition.column;
							Add(TokenType::SquareOpen, "[", beginPosition, beginPosition);
							++beginPosition.column;
							Add(TokenType::JumpID, _content.substr(idIndex, idSize), beginPosition,
								Position(beginPosition.line, beginPosition.column + idSize - 1));
							beginPosition.column += idSize;
							++beginPosition.column;
							Add(TokenType::SquareClose, "]");
							Advance(0);
							break;
						}
						if (Check(1) && Is(1, ' '))
						{
							Add(TokenType::DescribeMark, original, beginPosition, beginPosition);
							++beginPosition.column;
							Add(TokenType::SquareOpen, "[", beginPosition, beginPosition);
							++beginPosition.column;
							Add(TokenType::JumpID, _content.substr(idIndex, idSize), beginPosition,
								Position(beginPosition.line, beginPosition.column + idSize - 1));
							beginPosition.column += idSize;
							++beginPosition.column;
							Add(TokenType::SquareClose, "]");
							Advance(1);
							break;
						}
					}
					AddText(original);
					Retreat(idSize + 1);
					break;
				}
				Advance(0);
				AddText(original);
				break;
			}

			case '|':
				// trim right
				if (!_tokens.empty() && _tokens.back().type == TokenType::Text)
				{
					size_t blankCount = 0;
					for (size_t i = _tokens.back().value.size(); i >= 0 && _tokens.back().value[i - 1] == ' '; --i)
					{
						++blankCount;
					}
					_tokens.back().value = _tokens.back().value.substr(0, _tokens.back().value.size() - blankCount);
				}
				Add(TokenType::Pipe, "|");
				Advance(0);
				TokenizeSpace();
				break;

			case '<': {
				if (Check(2) && Is(1, '<') && Is(2, '<'))
				{
					Add(TokenType::Break, "<<<", _position, Position(_position.line, _position.column + 1));
					Advance(2);
					break;
				}
				if (Check(1) && Is(1, '<'))
				{
					Add(TokenType::Break, "<<", _position, Position(_position.line, _position.column + 1));
					Advance(1);
					break;
				}
				Advance(0);
				size_t begin = _index;
				size_t size = 0;
				while (Check(0) && !Is(0, '>'))
				{
					++size;
					Advance(0);
				}
				Advance(0);
				std::string_view text = std::string_view(_content.data() + begin, size);

				if (IsDate(text))
				{
					Add(TokenType::Date, _content.substr(begin, size));
					break;
				}
				if (IsTime(text))
				{
					Add(TokenType::Time, _content.substr(begin, size));
					break;
				}
				if (IsDateTime(text))
				{
					Add(TokenType::DateTime, _content.substr(begin, size));
					break;
				}

				AddText("<");
				AddText(_content.substr(begin, size));
				AddText(">");
				break;
			}

			case '*':
				if (Check(2) && Is(1, '*') && Is(2, '*'))
				{
					Add(TokenType::Dinkus, "***");
					Advance(2);
					break;
				}
				Add(TokenType::FSBold, "*");
				Advance(0);
				break;

			case '_':
				Add(TokenType::FSItalic, "_");
				Advance(0);
				break;

			case '`':
				Add(TokenType::FSMonospace, "`");
				Advance(0);
				break;

			case '>': {
				Position beginPosition = _position;

				if (Check(1) && Is(1, ' '))
				{
					Advance(1);
					Add(TokenType::HeadingIDMark, ">", beginPosition, beginPosition);

					size_t idIndex = _index;
					size_t idSize = 0;

					while (Check(0) && !Is(0, '\n') && IsSymbol(0))
					{
						++idSize;
						Advance(0);
					}

					if (idSize > 0)
					{
						Add(TokenType::HeadingID, _content.substr(idIndex, idSize));
					}
					break;
				}

				AddText(">");
				Advance(0);
				break;
			}

			default:
				AddText(_content.substr(_index, 1));
				Advance(0);
				break;
		}
	}

	void Lexer::TokenizePrefix()
	{
		Position beginPosition = _position;

		std::basic_string<TokenType> currentPrefix;
		size_t currentIndent = 0;

		while (Is(0, '\t') || Is(0, '/'))
		{
			if (Is(0, '\t'))
			{
				currentPrefix += TokenType::IndentIncr;
				++currentIndent;
				Advance(0);
				continue;
			}

			if (Check(1) && Is(0, '/') && Is(1, ' '))
			{
				currentPrefix += TokenType::BQIncr;
				Advance(1);
				continue;
			}

			if ((Is(0, '/') && Is(1, '\0')) || (Is(0, '/') && Is(1, '\n')) || (Is(0, '/') && Is(1, '\t')))
			{
				currentPrefix += TokenType::BQIncr;
				Advance(0);
				continue;
			}

			break;
		}

		size_t lowerBound = std::min(_prefix.size(), currentPrefix.size());

		for (size_t i = 0; i < lowerBound; ++i)
		{
			if (currentPrefix[i] != _prefix[i])
			{
				lowerBound = i;
				break;
			}
		}

		if (_prefix.size() > lowerBound)
		{
			while (_prefix.size() > lowerBound)
			{
				Add(_prefix.back() == TokenType::IndentIncr ? TokenType::IndentDecr : TokenType::BQDecr,
					_prefix.back() == TokenType::IndentIncr ? "I-" : "B-", beginPosition, beginPosition);
				_prefix.pop_back();
			}
		}

		if (currentPrefix.size() > lowerBound)
		{
			while (lowerBound < currentPrefix.size())
			{
				Add(currentPrefix[lowerBound], currentPrefix[lowerBound] == TokenType::IndentIncr ? "I+" : "B+",
					beginPosition, beginPosition);
				++lowerBound;
			}
		}

		_prefix = currentPrefix;
		_indent = currentIndent;

		/* size_t currentIndent = is(0, '\t') ? count('\t') : 0; */

		/* if (currentIndent > indent) { */
		/* 	while (currentIndent > indent) { */
		/* 		beginPosition.column = indent; */
		/* 		Add(TokenType::indentIncr, "I+", beginPosition, beginPosition); */
		/* 		++indent; */
		/* 	} */
		/* } */

		/* if (currentIndent < indent) { */
		/* 	while (currentIndent < indent) { */
		/* 		--indent; */
		/* 		// indentDecr has no symbol */
		/* 		Add(TokenType::indentDecr, "I-", tokens.back().begin, tokens.back().begin); */
		/* 	} */
		/* } */
	}

	void Lexer::TokenizeBlock()
	{
		TokenizePrefix();

		Position beginPosition = _position;

		switch (Get(0))
		{
			case '>': {
				Position beginPosition = _position;

				size_t counter = Count('>');
				if (counter > 3)
				{
					AddText(std::string(counter, '>'));
					break;
				}

				if (Is(0, ' '))
				{
					Advance(0);
					switch (counter)
					{
						case 3:
							Add(TokenType::ChapterMark, ">>>", beginPosition,
								Position(beginPosition.line, beginPosition.column + 2));
							break;
						case 2:
							Add(TokenType::SectionMark, ">>", beginPosition,
								Position(beginPosition.line, beginPosition.column + 1));
							break;
						case 1:
							Add(TokenType::SubsectionMark, ">", beginPosition, beginPosition);
							break;
					}
					break;
				}
				AddText(std::string(counter, '>'));
				break;
			}

			case '-': {
				TokenizeCode();
				break;
			}

			case '[': {
				Position beginPosition = _position;
				if (Check(3) && Is(2, ']') && Is(3, ' '))
				{
					if (Is(1, ' '))
					{
						Add(TokenType::Checkbox, "[ ]", beginPosition,
							Position(beginPosition.line, beginPosition.column + 2));
						Advance(2);
						TokenizeSpace();
						break;
					}
					if (Is(1, 'v'))
					{
						Add(TokenType::Checkbox, "[v]", beginPosition,
							Position(beginPosition.line, beginPosition.column + 2));
						Advance(2);
						TokenizeSpace();
						break;
					}
					if (Is(1, 'x'))
					{
						Add(TokenType::Checkbox, "[x]", beginPosition,
							Position(beginPosition.line, beginPosition.column + 2));
						Advance(2);
						TokenizeSpace();
						break;
					}
				}

				// see inline [
				break;
			}

			case '{': {
				// see inline [
				break;
			}

			case '|':
				TokenizeTable();
				break;

			case '<': {
				size_t previousIndex = _index;
				Position beginPosition = _position;
				Advance(0);
				size_t beginIndex = _index;
				size_t size = 0;
				while (Check(0) && !Is(0, '>'))
				{
					++size;
					Advance(0);
				}
				Advance(0);
				std::string inside = _content.substr(beginIndex, size);
				if (inside == "note")
				{
					Add(TokenType::AdmonNote, "<note>", beginPosition);
					TokenizeSpace();
					break;
				}
				else if (inside == "hint")
				{
					Add(TokenType::AdmonHint, "<hint>", beginPosition);
					TokenizeSpace();
					break;
				}
				else if (inside == "important")
				{
					Add(TokenType::AdmonImportant, "<important>", beginPosition);
					TokenizeSpace();
					break;
				}
				else if (inside == "warning")
				{
					Add(TokenType::AdmonWarning, "<warning>", beginPosition);
					TokenizeSpace();
					break;
				}
				else if (inside == "seealso")
				{
					Add(TokenType::AdmonSeeAlso, "<seealso>", beginPosition);
					TokenizeSpace();
					break;
				}
				else if (inside == "tip")
				{
					Add(TokenType::AdmonTip, "<tip>", beginPosition);
					TokenizeSpace();
					break;
				}

				_index = previousIndex;
				// see inline <
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
			case '9': {
				Position beginPosition = _position;
				std::string number;
				while (Check(0) && Get(0) >= '0' && Get(0) <= '9')
				{
					number += Get(0);
					Advance(0);
				}

				if (Check(1) && Is(0, '.') && Is(1, ' '))
				{
					Advance(0);
					TokenizeSpace();
					Add(TokenType::Index, number + ".", beginPosition,
						Position(beginPosition.line, beginPosition.column + number.size()));
					break;
				}

				AddText(number);
				break;
			}

			default:
				break;
		}
	}

	void Lexer::TokenizeCode()
	{
		Position beginPosition = _position;

		size_t openingIndent = _indent;
		size_t openingCounter = Count('-');

		if (openingCounter == 1)
		{
			if (Is(0, ' '))
			{
				Advance(0);
				Add(TokenType::Bullet, "-", beginPosition, beginPosition);
				return;
			}

			Retreat(1);
			// see inline -
			return;
		}

		if (openingCounter == 2)
		{
			Retreat(2);
			// see inline -
			return;
		}

		bool hasSpace = Is(0, ' ');
		size_t spaceCounter = Count(' ');

		if (hasSpace)
		{
			size_t langIndex = _index;
			size_t langSize = 0;

			Position beginLangPosition = _position;

			while (Check(0) && !Is(0, '\n') && IsSymbol(0))
			{
				++langSize;
				Advance(0);
			}

			if (langSize == 0 || !Is(0, '\n'))
			{
				Retreat(openingCounter + spaceCounter + langSize);
				return;
			}

			Add(TokenType::CodeMark, std::string(openingCounter, '-'), beginPosition,
				Position(beginLangPosition.line, beginLangPosition.column - 1));
			Add(TokenType::CodeLang, _content.substr(langIndex, langSize), beginLangPosition);
		}
		else
		{
			if (!Is(0, '\n'))
			{
				Retreat(openingCounter + spaceCounter);
				// see inline -
				return;
			}

			Add(TokenType::CodeMark, std::string(openingCounter, '-'), beginPosition);
		}

		beginPosition = _position;
		beginPosition.line += 1;
		beginPosition.column = _indent;

		Position endPosition = beginPosition;

		std::string source;
		_position.column = _indent;

		while (Check(0))
		{
			if (Is(0, '\n'))
			{
				endPosition = _position;

				size_t newline = Count('\n');
				size_t indent = Count('\t');

				_position.line += newline;
				_position.column = indent;

				if (Check(2) && Is(0, '-') && Is(1, '-') && Is(2, '-'))
				{
					size_t closingCounter = Count('-');
					if ((_index >= _content.size() || Check(0) && Is(0, '\n')) && closingCounter == openingCounter)
					{
						break;
					}

					if (newline > 0)
					{
						source += std::string(newline, '\n');
					}

					if (indent > openingIndent)
					{
						source += std::string(indent - openingIndent, '\t');
					}

					source += std::string(closingCounter, '-');
					continue;
				}
				else
				{
					if (newline > 0)
					{
						source += std::string(newline, '\n');
					}

					if (indent > openingIndent)
					{
						source += std::string(indent - openingIndent, '\t');
					}
					continue;
				}
			}

			source += Get(0);
			Advance(0);
		}

		size_t trimBegin = 0;
		size_t trimEnd = 0;

		for (size_t i = 0; i < source.size() && source[i] == '\n'; ++i)
		{
			++trimBegin;
		}

		for (size_t i = source.size(); source.size() && i > 0 && source[i] == '\n'; --i)
		{
			++trimEnd;
		}

		size_t trimSize = trimEnd == source.size() ? source.size() : source.size() - trimBegin - trimEnd;

		Add(TokenType::CodeSource, source.substr(trimBegin, trimSize), beginPosition, endPosition);
		Add(TokenType::CodeMark, std::string(openingCounter, '-'), Position(_position.line, _indent));
	}

	void Lexer::TokenizeSpace()
	{
		while (Check(0) && Is(0, ' '))
		{
			Advance(0);
		}

		return;
	}

	void Lexer::TokenizeTable()
	{
		Add(TokenType::Pipe, "|");
		Advance(0);

		if (!(Is(0, '-') || Is(0, ':')))
		{
			return;
		}

		while (Check(0) && (Is(0, '-') || Is(0, ':') || Is(0, '|')) && !Is(0, '\n'))
		{
			Position beginPosition = _position;

			if (Check(1) && Is(0, ':') && Is(1, '-'))
			{
				Advance(0);
				size_t lineCounter = Count('-');

				if (Get(0) == ':')
				{
					Advance(0);
					Add(TokenType::PipeConnector, ":-:", beginPosition);
					continue;
				}

				Add(TokenType::PipeConnector, ":--", beginPosition);
				continue;
			}

			if (Is(0, '-'))
			{
				size_t lineCounter = Count('-');

				if (Get(0) == ':')
				{
					Advance(0);
					Add(TokenType::PipeConnector, "--:", beginPosition);
					continue;
				}

				Add(TokenType::PipeConnector, ":--", beginPosition);
				continue;
			}

			if (Is(0, '|'))
			{
				Add(TokenType::Pipe, "|");
				Advance(0);
				continue;
			}

			AddText(_content.substr(_index));
			return;
		}
	}

	void Lexer::TokenizeText()
	{
		while (Check(0))
		{
			switch (Get(0))
			{
				case ' ':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'j':
				case 'k':
				case 'l':
				case 'm':
				case 'n':
				case 'o':
				case 'p':
				case 'q':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'v':
				case 'w':
				case 'x':
				case 'y':
				case 'z':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					AddText(_content.substr(_index, 1));
					Advance(0);
					break;

				default:
					return;
			}
		}
	}

	void Lexer::TokenizeQuoMark(
		bool should, TokenType leftType, const std::string& leftValue, TokenType rightType,
		const std::string& rightValue
	)
	{
		if (should || _index == 0 || _content[_index - 1] == ' ' || _content[_index - 1] == '\t' ||
			_content[_index - 1] == '\n' || _content[_index - 1] == '\0')

		{
			Add(leftType, leftValue);
			return;
		}

		Add(rightType, rightValue);
	}
}
