#pragma once

#include "Gularen/Lexer.h"
#include "Gularen/Node.h"
#include <functional>

namespace Gularen
{
	using Visitor = std::function<void(const NodePtr& node)>;

	class Parser
	{
	public:
		void Set(const std::string& content, const std::string& path = "");

		void Load(const std::string& path);

		void Parse();

		const NodePtr& GetRoot() const;

		void Visit(const Visitor& visitor);

	private:
		bool CheckBoundary(size_t offset) const;

		bool IsToken(size_t offset, TokenType type) const;

		const Token& GetToken(size_t offset) const;

		void Advance(size_t offset);

		void Add(const NodePtr& node, const Range& range);

		void AddText(const std::string& value, const Range& range);

		void AddScope(const NodePtr& node, const Range& range);

		void RemoveScope(const Range& range);

		const NodePtr& GetScope();

		void ParseInline();

		void ParseBlock();

		void ParseBlockEnding();

		void ParseIndent();

		NodePtr RecursiveLoad(const std::string& directory, const std::string& nextPath);

		void RecursiveVisit(const Visitor& visitor, const NodePtr& node);

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
