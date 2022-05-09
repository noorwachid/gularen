#include "TokenType.hpp"

namespace Gularen {
	std::string toString(TokenType type) {
		switch (type) {
			case TokenType::unknown:
				return "unknown";

			case TokenType::openDocument:
				return "openDocument";
			case TokenType::closeDocument:
				return "closeDocument";

			case TokenType::hash:
				return "hash";
			case TokenType::at:
				return "at";
			case TokenType::symbol:
				return "symbol";
			case TokenType::string:
				return "string";
			case TokenType::openFormatting:
				return "openFormatting";
			case TokenType::closeFormatting:
				return "closeFormatting";

			case TokenType::imageKeyword:
				return "imageKeyword";
			case TokenType::fileKeyword:
				return "fileKeyword";
			case TokenType::tableKeyword:
				return "tableKeyword";
			case TokenType::codeKeyword:
				return "codeKeyword";
			case TokenType::admonitionKeyword:
				return "admonitionKeyword";
			case TokenType::tocKeyword:
				return "tocKeyword";
			case TokenType::referenceKeyword:
				return "referenceKeyword";

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

			case TokenType::period:
				return "period";
			case TokenType::comma:
				return "comma";

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
			case TokenType::caret:
				return "caret";
			case TokenType::ampersand:
				return "ampersand";

			case TokenType::openSingleQuote:
				return "openSingleQuote";
			case TokenType::closeSingleQuote:
				return "closeSingleQuote";
			case TokenType::openDoubleQuote:
				return "openDoubleQuote";
			case TokenType::closeDoubleQuote:
				return "closeDoubleQuote";

			case TokenType::openCurlyBracket:
				return "openCurlyBracket";
			case TokenType::closeCurlyBracket:
				return "closeCurlyBracket";
			case TokenType::openSquareBracket:
				return "openSquareBracket";
			case TokenType::closeSquareBracket:
				return "closeSquareBracket";

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

			default:
				return "[unhandled]";
		}
	}
}
