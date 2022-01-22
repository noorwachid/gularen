#include "TokenType.hpp"

namespace Gularen
{
	std::string toString(TokenType type)
	{
		switch (type)
		{
			case TokenType::unknown:
				return "unknown";

			case TokenType::documentBegin:
				return "documentBegin";
			case TokenType::documentEnd:
				return "documentEnd";

			case TokenType::hashSymbol:
				return "hashSymbol";
			case TokenType::atSymbol:
				return "atSymbol";
			case TokenType::symbol:
				return "symbol";
			case TokenType::text:
				return "text";
			case TokenType::quotedText:
				return "quotedText";
			case TokenType::rawText:
				return "rawText";

			case TokenType::keywordImage:
				return "keywordImage";
			case TokenType::KeywordFile:
				return "KeywordFile";
			case TokenType::keywordTable:
				return "keywordTable";
			case TokenType::keywordCode:
				return "keywordCode";
			case TokenType::KeywordBlock:
				return "KeywordBlock";
			case TokenType::KeywordToc:
				return "KeywordToc";
			case TokenType::KeywordRef:
				return "KeywordRef";

			case TokenType::asterisk:
				return "asterisk";
			case TokenType::underline:
				return "underline";
			case TokenType::backtick:
				return "backtick";

			case TokenType::newline:
				return "newline";
			case TokenType::space:
				return "space";

			case TokenType::equal:
				return "equal";
			case TokenType::colon:
				return "colon";
			case TokenType::pipe:
				return "pipe";
			case TokenType::questionMark:
				return "questionMark";
			case TokenType::exclamationMark:
				return "exclamationMark";

			case TokenType::leftCurlyBracket:
				return "leftCurlyBracket";
			case TokenType::rightCurlyBracket:
				return "rightCurlyBracket";

			case TokenType::dollar:
				return "dollar";

			case TokenType::bullet:
				return "Bullet";
			case TokenType::numericBullet:
				return "numericBullet";
			case TokenType::checkBox:
				return "checkBox";
			case TokenType::line:
				return "Line";

			case TokenType::anchor:
				return "anchor";

			case TokenType::tail:
				return "tail";
			case TokenType::reverseTail:
				return "reverseTail";
			case TokenType::arrow:
				return "arrow";

			case TokenType::teeth:
				return "teeth";

			default:
				return "[unhandled]";
		}
	}
}
