#include "Lexer.h"
#include <iostream>

namespace Gularen {
    // PUBLIC DEFINITIONS
    
    Array<Token> Lexer::parse(const String& buffer) {
        _buffer = buffer;
        _bufferCursor = _buffer.begin();

        _tokens.clear();

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
                parseFS(TokenType::backtick);
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
        return _tokens;
    }

    // PRIVATE DEFINITIONS

    // Main Routine Buffer Parsing

    void Lexer::parseNewline() {
        UintSize size = 0;

        while (isByteCursorInProgress() && getCurrentByte() == '\n') {
            advanceByteCursor();
            ++size;
        }

        addToken(Token(TokenType::newline, size));

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
        auto& previous = _tokens.back();

        if ((previous.type == TokenType::text && previous.content.back() == ' ') || 
            previous.type == TokenType::newline || 
            previous.type == TokenType::boDocument)
            return addToken(Token(opening));

        addToken(Token(ending));
    }
    
    void Lexer::parseText() {
        if (isCurrentByteText())
            addToken(Token(TokenType::text, consumeTextBytes()));
        else
            if (_tokens.back().type != TokenType::text)
                addToken(Token(TokenType::text, {getCurrentByte(), 0x0}));
            else 
                _tokens.back().content += getCurrentByte();
    }
    
    void Lexer::parseSymbol() {
        if (isCurrentByteSymbol())
            addToken(Token(TokenType::symbol, consumeSymbolBytes()));
        else
            parseText();
    }

    void Lexer::parseBlock(bool withAdvancing) {
        _inArrowLine = false;

        switch (getCurrentByte()) {
        case '>':
            parseArrow();
            break;

        case '-':
            parseLine();
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

                _inArrowLine = true;
                return addToken(Token(TokenType::smallArrow));
            }

            retreatByteCursor();
        }

        retreatByteCursor();
    }

    void Lexer::parseArrow() {
        advanceByteCursor();

        if (getCurrentByte() == ' ') {
            // "> "
            _inArrowLine = true;
            return addToken(Token(TokenType::arrowHead));
        }

        if (getCurrentByte() == '-') {
            advanceByteCursor();

            if (getCurrentByte() == '>') {
                advanceByteCursor();

                if (getCurrentByte() == ' ') {
                    // ">-> "
                    _inArrowLine = true;
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
                _inArrowLine = true;
                return addToken(Token(TokenType::arrowTail));
            }

            if (getCurrentByte() == '-') {
                advanceByteCursor();

                if (getCurrentByte() == '>') {
                    advanceByteCursor();

                    if (getCurrentByte() == ' ') {
                        // ">>-> "
                        _inArrowLine = true;
                        return addToken(Token(TokenType::largeArrow));
                    }

                    retreatByteCursor();
                }

                retreatByteCursor();
            }

            if (getCurrentByte() == '>') {
                advanceByteCursor();

                if (getCurrentByte() == ' ') {
                    // ">>> "
                    _inArrowLine = true;
                    return addToken(Token(TokenType::largeArrowTail));
                }

                if (getCurrentByte() == '-') {
                    advanceByteCursor();

                    if (getCurrentByte() == '>') {
                        advanceByteCursor();

                        if (getCurrentByte() == ' ') {
                            // ">>>-> "
                            _inArrowLine = true;
                            return addToken(Token(TokenType::extraLargeArrow));
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
        if (!_inArrowLine)
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

    void Lexer::parseEmojiShortcode() {
    }

    // Sub Routine Buffer Parsing
    
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
        return *_bufferCursor;
    }
    
    bool Lexer::isByteCursorInProgress() {
        return _bufferCursor < _buffer.end();
    }
    
    void Lexer::advanceByteCursor() {
        ++_bufferCursor;
    }
    
    void Lexer::retreatByteCursor() {
        --_bufferCursor;
    }
    
    bool Lexer::isCurrentByteText() {
        char byte = getCurrentByte();

        return 
            (byte >= 'a' && byte <= 'z') || 
            (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9') ||

            byte == ' ' || 
            byte == ',' ||
            byte == '.'
        ;
    }

    bool Lexer::isCurrentByteSymbol() {
        char byte = getCurrentByte();
        
        return 
            (byte >= 'a' && byte <= 'z') || 
            (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9') ||

            byte == '-'
        ;
    }

    // Token Helper

    void Lexer::addToken(Token&& token) {
        _tokens.push_back(token);
    }
}
