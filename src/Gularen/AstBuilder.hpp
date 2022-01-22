#pragma once

#include "Node.hpp"
#include "Lexer.hpp"
#include <stack>

namespace Gularen
{
	class AstBuilder
	{
	public:
		AstBuilder();

		~AstBuilder();

		void setBuffer(const std::string& buffer);

		void setTokens(const std::vector<Token>& tokens);

		void parse();

		void reset();

		void destroyTree();

		std::string getBuffer();

		std::vector<Token> getTokens();

		Node* getTree();

		std::string getTokensAsString();

		std::string getTreeAsString();

	private:
		void traverseAndGenerateBuffer(Node* node, size_t depth);

		void traverseAndDestroyNode(Node* node);

	private:
		void parseNewline(size_t newlineSize = 1);

		void parseIndentation();

		void parseBreak();

		void parseLink(NodeType type);

		void parseBlock(TokenType type);

	private:
		Node* getHead();

		void pushHead(Node* node);

		void compareAndPopHead(NodeType type);

		void compareAndPopHead(NodeType type, size_t newlineSize);

		void popHead();

		void pairFormatHead(NodeType type);

		// -- Token operations
		bool isValid();

		Token& getCurrentToken();

		Token& getNextToken(size_t offset = 1);

		void skip(size_t offset = 1);

		// Buffer definitions
		std::string buffer;

		// Node definitions
		Node* rootNode = nullptr;
		size_t depth = 0;

		std::stack<Node*> headNodes;

		// Token definitions
		size_t tokenIndex = 0;
		size_t tokenSize = 0;
		Token emptyToken;

		size_t headerCounter = 0;

		Lexer lexer;
	};
}
