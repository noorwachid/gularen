#include "Gularen/Lexer.h"
#include "Gularen/Token.h"
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

		_tokens.clear();
		_index = 0;
		_end.line = 1;
		_end.column = 1;
		_begin.line = 1;
		_begin.column = 1;

		TokenizeBlock();

		while (CheckBoundary(0))
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

	const Tokens& Lexer::GetTokens() const
	{
		return _tokens;
	}

	bool Lexer::CheckBoundary(size_t offset)
	{
		return _index + offset < _content.size();
	}

	bool Lexer::IsByte(size_t offset, char c)
	{
		return _content[_index + offset] == c;
	}

	bool Lexer::IsSymbolByte(size_t offset)
	{
		char c = _content[_index + offset];
		return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-';
	}

	char Lexer::GetByte(size_t offset)
	{
		return _content[_index + offset];
	}

	void Lexer::Advance(size_t offset)
	{
		_index += 1 + offset;
		_end.column += 1 + offset;
	}

	void Lexer::Retreat(size_t offset)
	{
		_index -= offset;
		_end.column -= offset;
	}

	size_t Lexer::CountRepetendBytes(char c)
	{
		size_t count = 0;
		while (CheckBoundary(0) && IsByte(0, c))
		{
			++count;
			Advance(0);
		}
		return count;
	}

	void Lexer::Add(TokenType type, const std::string& value)
	{
		if (!_tokens.empty() && _tokens.back().type == TokenType::Text)
			_begin.column += 1;

		Token token;
		token.type = type;
		token.value = value;
		token.range.begin = _begin;
		token.range.end = _end;
		_tokens.push_back(token);
		_begin = _end;
		_begin.column += 1;
	}

	void Lexer::AddText(const std::string value)
	{
		if (!_tokens.empty() && _tokens.back().type == TokenType::Text)
		{
			_tokens.back().value += value;
			_tokens.back().range.end.column += value.size();
			_begin = _end;
		}
		else
		{
			if (!_tokens.empty())
			{
				Token token;
				token.type = TokenType::Text;
				token.value = value;
				token.range.begin = _begin;
				token.range.end = _end;
				_tokens.push_back(token);
				_begin = _end;
			}
			else
			{
				Token token;
				token.type = TokenType::Text;
				token.value = value;
				token.range.begin = _begin;
				token.range.end = _end;
				_tokens.push_back(token);
				_begin = _end;
			}
		}
	}

	void Lexer::TokenizeInline()
	{
		switch (GetByte(0))
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
				size_t originalColumn = _end.column;
				size_t counter = CountRepetendBytes('\n');

				if (counter == 1)
				{
					Token token;
					token.type = TokenType::Newline;
					token.value = "\\n";
					_end.column = originalColumn;
					token.range.begin = _end;
					token.range.end = _end;
					_end.line += 1;
					_end.column = 1;
					_begin = _end;
					_tokens.push_back(token);
				}
				else
				{
					_end.line += counter - 1;
					_end.column = 1;
					Token token;
					token.type = TokenType::NewlinePlus;
					token.value = "\\n";
					token.range.begin = _begin;
					token.range.end = _end;
					_end.line += 1;
					_begin = _end;
					_tokens.push_back(token);
				}

				TokenizeBlock();
				break;
			}

			case '\\':
				Advance(0);
				if (CheckBoundary(0))
				{
					AddText(_content.substr(_index, 1));
					Advance(0);
				}
				break;

			case '~': {
				size_t commentIndex = _index;
				size_t commentSize = 1;
				Advance(0);
				while (CheckBoundary(0) && !IsByte(0, '\n'))
				{
					++commentSize;
					Advance(0);
				}
				Add(TokenType::Comment, _content.substr(commentIndex, commentSize));
				break;
			}

			case '\'':
				TokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::QuoteOpen, TokenType::SingleQuoteOpen, "‘",
					TokenType::SingleQuoteClose, "’"
				);
				Advance(0);
				break;

			case '"':
				TokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::SingleQuoteOpen, TokenType::QuoteOpen, "“",
					TokenType::QuoteClose, "”"
				);
				Advance(0);
				break;

			case '-': {
				if (CheckBoundary(2) && IsByte(1, '-') && IsByte(2, '-'))
				{
					Add(TokenType::EmDash, "–");
					Advance(2);
					break;
				}
				if (CheckBoundary(1) && IsByte(1, '-'))
				{
					Add(TokenType::EnDash, "–");
					Advance(1);
					break;
				}
				Add(TokenType::Hyphen, "-");
				Advance(0);
				break;
			}

			case ':': {
				Advance(0);

				if (CheckBoundary(0))
				{
					size_t emojiIndex = _index;
					size_t emojiSize = 0;

					while (CheckBoundary(0) && (GetByte(0) >= 'a' && GetByte(0) <= 'z' || IsByte(0, '-')))
					{
						if (IsByte(0, ':'))
						{
							break;
						}

						++emojiSize;
						Advance(0);
					}

					if (IsByte(0, ':') && emojiSize > 0)
					{
						Add(TokenType::EmojiMark, ":");
						Add(TokenType::EmojiCode, _content.substr(emojiIndex, emojiSize));
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
				Advance(0);

				if (CheckBoundary(0))
				{
					size_t depth = 0;
					std::string source;

					while (CheckBoundary(0))
					{
						if (IsByte(0, '}') && depth == 0)
						{
							Advance(0);
							break;
						}

						if (IsByte(0, '{'))
						{
							++depth;
						}

						if (IsByte(0, '\\'))
						{
							Advance(0);
						}

						source += GetByte(0);
						Advance(0);
					}

					Add(TokenType::CurlyOpen, "{");
					Add(TokenType::CodeSource, source);
					Add(TokenType::CurlyClose, "}");

					if (IsByte(0, '('))
					{
						Advance(0);
						std::string lang;

						if (CheckBoundary(0) && IsSymbolByte(0))
						{
							while (CheckBoundary(0) && IsSymbolByte(0))
							{
								lang += GetByte(0);
								Advance(0);
							}

							if (IsByte(0, ')'))
							{
								Add(TokenType::ParenOpen, "(");
								Add(TokenType::ResourceLabel, lang);
								Add(TokenType::ParenClose, ")");
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
				Advance(0);

				if (CheckBoundary(0))
				{
					bool idMarkerExists = false;
					size_t idMarkerIndex = 0;
					size_t depth = 0;
					std::string resource;

					while (CheckBoundary(0))
					{
						if (IsByte(0, ']') && depth == 0)
						{
							Advance(0);
							break;
						}

						if (IsByte(0, '['))
						{
							++depth;
						}

						if (IsByte(0, '>'))
						{
							idMarkerExists = true;
							idMarkerIndex = resource.size();
						}

						if (IsByte(0, '\\'))
						{
							Advance(0);
						}

						resource += GetByte(0);
						Advance(0);
					}

					if (idMarkerExists)
					{
						Add(TokenType::SquareOpen, "[");
						Add(TokenType::Resource, resource.substr(0, idMarkerIndex));
						Add(TokenType::ResourceIDMark, ">");
						Add(TokenType::ResourceID, resource.substr(idMarkerIndex + 1));
						Add(TokenType::SquareClose, "]");
					}
					else
					{
						Add(TokenType::SquareOpen, "[");
						Add(TokenType::Resource, resource);
						Add(TokenType::SquareClose, "]");
					}

					if (IsByte(0, '('))
					{
						Advance(0);

						if (CheckBoundary(0))
						{
							size_t depth = 0;
							std::string label;

							while (CheckBoundary(0))
							{
								if (IsByte(0, ')') && depth == 0)
								{
									Advance(0);
									break;
								}

								if (IsByte(0, '('))
								{
									++depth;
								}

								if (IsByte(0, '\\'))
								{
									Advance(0);
								}

								label += GetByte(0);
								Advance(0);
							}

							Add(TokenType::ParenOpen, "(");
							Add(TokenType::ResourceLabel, label);
							Add(TokenType::ParenClose, ")");
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
				if (CheckBoundary(1) && IsByte(1, '['))
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
				if (CheckBoundary(1) && IsByte(1, '['))
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
				std::string original(1, GetByte(0));
				if (CheckBoundary(2) && IsByte(1, '[') && IsSymbolByte(2))
				{
					Advance(1);
					size_t idIndex = _index;
					size_t idSize = 0;
					while (CheckBoundary(0) && IsSymbolByte(0))
					{
						++idSize;
						Advance(0);
					}
					if (idSize > 0 && CheckBoundary(0) && IsByte(0, ']'))
					{
						if (original[0] == '^')
						{
							Add(TokenType::JumpMark, original);
							Add(TokenType::SquareOpen, "[");
							Add(TokenType::JumpID, _content.substr(idIndex, idSize));
							Add(TokenType::SquareClose, "]");
							Advance(0);
							break;
						}
						if (CheckBoundary(1) && IsByte(1, ' '))
						{
							Add(TokenType::DescribeMark, original);
							Add(TokenType::SquareOpen, "[");
							Add(TokenType::JumpID, _content.substr(idIndex, idSize));
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
				if (CheckBoundary(2) && IsByte(1, '<') && IsByte(2, '<'))
				{
					Add(TokenType::Break, "<<<");
					Advance(2);
					break;
				}
				if (CheckBoundary(1) && IsByte(1, '<'))
				{
					Add(TokenType::Break, "<<");
					Advance(1);
					break;
				}
				Advance(0);
				size_t begin = _index;
				size_t size = 0;
				while (CheckBoundary(0) && !IsByte(0, '>'))
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
				if (CheckBoundary(2) && IsByte(1, '*') && IsByte(2, '*'))
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
				if (CheckBoundary(1) && IsByte(1, ' '))
				{
					Add(TokenType::HeadingIDMark, ">");
					Advance(1);

					size_t idIndex = _index;
					size_t idSize = 0;

					while (CheckBoundary(0) && !IsByte(0, '\n') && IsSymbolByte(0))
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
		std::basic_string<TokenType> currentPrefix;
		size_t currentIndent = 0;

		while (IsByte(0, '\t') || IsByte(0, '/'))
		{
			if (IsByte(0, '\t'))
			{
				currentPrefix += TokenType::IndentIncr;
				++currentIndent;
				Advance(0);
				continue;
			}

			if (CheckBoundary(1) && IsByte(0, '/') && IsByte(1, ' '))
			{
				currentPrefix += TokenType::BQIncr;
				Advance(1);
				continue;
			}

			if ((IsByte(0, '/') && IsByte(1, '\0')) || (IsByte(0, '/') && IsByte(1, '\n')) || (IsByte(0, '/') && IsByte(1, '\t')))
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
					_prefix.back() == TokenType::IndentIncr ? "I-" : "B-");
				_prefix.pop_back();
			}
		}

		if (currentPrefix.size() > lowerBound)
		{
			while (lowerBound < currentPrefix.size())
			{
				Add(currentPrefix[lowerBound], currentPrefix[lowerBound] == TokenType::IndentIncr ? "I+" : "B+");
				++lowerBound;
			}
		}

		_prefix = currentPrefix;
		_indent = currentIndent;
	}

	void Lexer::TokenizeBlock()
	{
		TokenizePrefix();

		switch (GetByte(0))
		{
			case '>': {
				size_t counter = CountRepetendBytes('>');
				if (counter > 3)
				{
					AddText(std::string(counter, '>'));
					break;
				}

				if (IsByte(0, ' '))
				{
					Advance(0);
					switch (counter)
					{
						case 3:
							Add(TokenType::ChapterMark, ">>>");
							break;
						case 2:
							Add(TokenType::SectionMark, ">>");
							break;
						case 1:
							Add(TokenType::SubsectionMark, ">");
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
				if (CheckBoundary(3) && IsByte(2, ']') && IsByte(3, ' '))
				{
					if (IsByte(1, ' '))
					{
						Add(TokenType::Checkbox, "[ ]");
						Advance(2);
						TokenizeSpace();
						break;
					}
					if (IsByte(1, 'v'))
					{
						Add(TokenType::Checkbox, "[v]");
						Advance(2);
						TokenizeSpace();
						break;
					}
					if (IsByte(1, 'x'))
					{
						Add(TokenType::Checkbox, "[x]");
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
				Advance(0);
				size_t beginIndex = _index;
				size_t size = 0;
				while (CheckBoundary(0) && !IsByte(0, '>'))
				{
					++size;
					Advance(0);
				}
				Advance(0);
				std::string inside = _content.substr(beginIndex, size);
				if (inside == "note")
				{
					Add(TokenType::AdmonNote, "<note>");
					TokenizeSpace();
					break;
				}
				else if (inside == "hint")
				{
					Add(TokenType::AdmonHint, "<hint>");
					TokenizeSpace();
					break;
				}
				else if (inside == "important")
				{
					Add(TokenType::AdmonImportant, "<important>");
					TokenizeSpace();
					break;
				}
				else if (inside == "warning")
				{
					Add(TokenType::AdmonWarning, "<warning>");
					TokenizeSpace();
					break;
				}
				else if (inside == "seealso")
				{
					Add(TokenType::AdmonSeeAlso, "<seealso>");
					TokenizeSpace();
					break;
				}
				else if (inside == "tip")
				{
					Add(TokenType::AdmonTip, "<tip>");
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
				std::string number;
				while (CheckBoundary(0) && GetByte(0) >= '0' && GetByte(0) <= '9')
				{
					number += GetByte(0);
					Advance(0);
				}

				if (CheckBoundary(1) && IsByte(0, '.') && IsByte(1, ' '))
				{
					Advance(0);
					TokenizeSpace();
					Add(TokenType::Index, number + ".");
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
		size_t openingIndent = _indent;
		size_t openingCounter = CountRepetendBytes('-');

		if (openingCounter == 1)
		{
			if (IsByte(0, ' '))
			{
				Advance(0);
				Add(TokenType::Bullet, "-");
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

		bool hasSpace = IsByte(0, ' ');
		size_t spaceCounter = CountRepetendBytes(' ');

		if (hasSpace)
		{
			size_t langIndex = _index;
			size_t langSize = 0;

			while (CheckBoundary(0) && !IsByte(0, '\n') && IsSymbolByte(0))
			{
				++langSize;
				Advance(0);
			}

			if (langSize == 0 || !IsByte(0, '\n'))
			{
				Retreat(openingCounter + spaceCounter + langSize);
				return;
			}

			Add(TokenType::CodeMark, std::string(openingCounter, '-'));
			Add(TokenType::CodeLang, _content.substr(langIndex, langSize));
		}
		else
		{
			if (!IsByte(0, '\n'))
			{
				Retreat(openingCounter + spaceCounter);
				// see inline -
				return;
			}

			Add(TokenType::CodeMark, std::string(openingCounter, '-'));
		}

		std::string source;
		_end.column = _indent;

		while (CheckBoundary(0))
		{
			if (IsByte(0, '\n'))
			{
				size_t newline = CountRepetendBytes('\n');
				size_t indent = CountRepetendBytes('\t');

				_end.line += newline;
				_end.column = indent;

				if (CheckBoundary(2) && IsByte(0, '-') && IsByte(1, '-') && IsByte(2, '-'))
				{
					size_t closingCounter = CountRepetendBytes('-');
					if ((_index >= _content.size() || CheckBoundary(0) && IsByte(0, '\n')) && closingCounter == openingCounter)
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

			source += GetByte(0);
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

		Add(TokenType::CodeSource, source.substr(trimBegin, trimSize));
		Add(TokenType::CodeMark, std::string(openingCounter, '-'));
	}

	void Lexer::TokenizeSpace()
	{
		while (CheckBoundary(0) && IsByte(0, ' '))
		{
			Advance(0);
		}

		return;
	}

	void Lexer::TokenizeTable()
	{
		Add(TokenType::Pipe, "|");
		Advance(0);

		if (!(IsByte(0, '-') || IsByte(0, ':')))
		{
			return;
		}

		while (CheckBoundary(0) && (IsByte(0, '-') || IsByte(0, ':') || IsByte(0, '|')) && !IsByte(0, '\n'))
		{
			if (CheckBoundary(1) && IsByte(0, ':') && IsByte(1, '-'))
			{
				Advance(0);
				size_t lineCounter = CountRepetendBytes('-');

				if (GetByte(0) == ':')
				{
					Advance(0);
					Add(TokenType::PipeConnector, ":-:");
					continue;
				}

				Add(TokenType::PipeConnector, ":--");
				continue;
			}

			if (IsByte(0, '-'))
			{
				size_t lineCounter = CountRepetendBytes('-');

				if (GetByte(0) == ':')
				{
					Advance(0);
					Add(TokenType::PipeConnector, "--:");
					continue;
				}

				Add(TokenType::PipeConnector, ":--");
				continue;
			}

			if (IsByte(0, '|'))
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
		while (CheckBoundary(0))
		{
			switch (GetByte(0))
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
