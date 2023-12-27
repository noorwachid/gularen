#pragma once

#include "Gularen/Node.h"
#include "Gularen/Internal/Lexer.h"
#include <functional>

namespace Gularen {
	using Visitor = std::function<void(const NodePtr& node)>;

	class Parser {
	public:
		void set(const std::string& content, const std::string& path = "");

		void load(const std::string& path);

		void parse();

		const NodePtr& getRoot() const;

		void visit(const Visitor& visitor);

	private:
		bool checkBoundary(size_t offset) const;

		bool isToken(size_t offset, TokenType type) const;

		const Token& getToken(size_t offset) const;

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

		void recursiveVisit(const Visitor& visitor, const NodePtr& node);

	private:
		NodePtr _root;
		Lexer _lexer;
		std::stack<NodePtr> _scopes;
		bool _pathVirtual = false;
		std::string _path;
		std::vector<std::string> _previousPaths;
		size_t _index;
		TokenType _lastEnding;

		bool _lastListDeadBecauseNewlinePlus;
	};
}
