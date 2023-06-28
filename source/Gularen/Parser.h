#pragma once

#include <Gularen/Lexer.h>
#include <Gularen/Node.h>

namespace Gularen {
	class Parser {
	public:
		const NodePtr& parse(const std::string& content);
		
		const NodePtr& get() const;

	private:
		bool check(size_t offset) const;

		bool is(size_t offset, TokenType type) const;

		const Token& get(size_t offset) const;

		void advance(size_t offset);

		void add(const NodePtr& node);

		void addScope(const NodePtr& node);

		void removeScope();

		const NodePtr& getScope();

		void parse();

		void parseBlock();

	private:
		NodePtr root;
		Lexer lexer;
		std::stack<NodePtr> scopes;
		size_t index;
		size_t lastNewline;
	};
}
