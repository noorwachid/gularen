#pragma once

#include <gularen/lexer.h>
#include <gularen/node.h>
#include <functional>

namespace Gularen {
	using Visitor = std::function<void(const NodePtr& node)>;

	class Parser {
	public:
		void set(const std::string& content, const std::string& path = "");

		void load(const std::string& path);

		void parse();
		
		const NodePtr& get() const;

		void visit(const Visitor& visitor);

	private:
		bool check(size_t offset) const;

		bool is(size_t offset, TokenType type) const;

		const Token& get(size_t offset) const;

		void advance(size_t offset);

		void add(const NodePtr& node, const Range& range);

		void addText(const std::string& value, const Range& range);

		void addScope(const NodePtr& node, const Range& range);

		void removeScope(const Range& range);

		const NodePtr& getScope();

		void parseInline();

		void parseBlock();

		void parseBlockEnding();

		void parseIndent();

		NodePtr recursiveLoad(const std::string& directory, const std::string& nextPath);

		void visit(const Visitor& visitor, const NodePtr& node);

	private:
		NodePtr root;
		Lexer lexer;
		std::stack<NodePtr> scopes;
		bool pathVirtual = false;
		std::string path;
		std::vector<std::string> previousPaths;
		size_t index;
		TokenType lastEnding;

		bool lastListDeadBecauseNewlinePlus;
	};
}
