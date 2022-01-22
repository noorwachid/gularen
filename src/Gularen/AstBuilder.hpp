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

		void SetBuffer(const std::string& buffer);

		void SetTokens(const std::vector<Token>& tokens);

		void Parse();

		void Reset();

		void DestroyTree();

		std::string GetBuffer();

		std::vector<Token> GetTokens();

		Node* GetTree();

		std::string GetTokensAsString();

		std::string GetTreeAsString();

	private:
		void TraverseAndGenerateBuffer(Node* node, size_t depth);

		void TraverseAndDestroyNode(Node* node);

	private:
		void ParseNewline(size_t newlineSize = 1);

		void ParseIndentation();

		void ParseBreak();

		void ParseLink(NodeType type);

		void ParseBlock(TokenType type);

	private:
		Node* GetHead();

		void PushHead(Node* node);

		void CompareAndPopHead(NodeType type);

		void CompareAndPopHead(NodeType type, size_t newlineSize);

		void PopHead();

		void PairFHead(NodeType type);

		// -- Token operations
		bool IsValid();

		Token& GetCurrentToken();

		Token& GetNextToken(size_t offset = 1);

		void Skip(size_t offset = 1);

		// Buffer definitions
		std::string mBuffer;

		// Node definitions
		Node* mRoot = nullptr;
		size_t mDepth = 0;

		std::stack<Node*> mHeads;

		// Token definitions
		size_t mTokenIndex = 0;
		size_t mTokenSize = 0;
		Token mEmptyToken;

		size_t mHeaderCounter = 0;

		Lexer mLexer;
	};
}
