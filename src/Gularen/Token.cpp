#include "Token.h"

namespace Gularen {
	Token::Token(TokenType type) : type(type), size(0) {
	}

	Token::Token(TokenType type, const String& content) : type(type), content(content), size(content.size()) {
	}

	Token::Token(TokenType type, std::size_t size) : type(type), size(size) {
	}

	String Token::toString() const {
		if (type == TokenType::text || type == TokenType::symbol) {
			return Gularen::toString(type) + " content = \"" + content + "\"";
		}

		if (type == TokenType::indentation || type == TokenType::newline) {
			return Gularen::toString(type) + " size = " + std::to_string(size);
		}

		return Gularen::toString(type);
	}

	bool Token::operator==(const Token& other) {
		return type == other.type && size == other.size && content == other.content;
	}

	String toString(TokenType tokenType) {
		switch (tokenType) {
			case TokenType::text:
				return "text";
			case TokenType::symbol:
				return "symbol";

			case TokenType::indentation:
				return "indentation";
			case TokenType::newline:
				return "newline";

			case TokenType::asterisk:
				return "asterisk";
			case TokenType::underscore:
				return "underscore";
			case TokenType::Backtick:
				return "backtick";

			case TokenType::lsQuote:
				return "lSQuote";
			case TokenType::rsQuote:
				return "rSQuote";
			case TokenType::ldQuote:
				return "lDQuote";
			case TokenType::rdQuote:
				return "rDQuote";

			case TokenType::boDocument:
				return "bODocument";
			case TokenType::eoDocument:
				return "eODocument";

			case TokenType::sArrow:
				return "sArrow";
			case TokenType::arrow:
				return "arrow";
			case TokenType::lArrow:
				return "lArrow";
			case TokenType::xlArrow:
				return "xLArrow";

			case TokenType::arrowHead:
				return "arrowHead";
			case TokenType::arrowTail:
				return "arrowTail";
			case TokenType::lArrowTail:
				return "lArrowTail";

			case TokenType::rArrowHead:
				return "rArrowHead";
			case TokenType::rArrowTail:
				return "rArrowTail";
			case TokenType::rlArrowTail:
				return "rLArrowTail";

			case TokenType::numericBullet:
				return "numericBullet";
			case TokenType::bullet:
				return "bullet";

			default:
				return "[unknown]";
		}
	}
}
