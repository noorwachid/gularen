#include "AST.h"
#include <iostream>

namespace Gularen
{
	AST::AST(const RC<Node>& rootNode): _rootNode(rootNode) {}

	void AST::SetRootNode(const RC<Node>& rootNode)
	{
		_rootNode = rootNode;
	}

	const RC<Node>& AST::GetRootNode() const
	{
		return _rootNode;
	}

	void AST::Print()
	{
		Print(_rootNode, 0);
	}

	void AST::Print(const RC<Node>& parentNode, UintSize depth)
	{
		if (!parentNode)
			return;

        for (std::size_t i = 0; i < depth; ++i)
            std::cout << "    ";

        std::cout << parentNode->ToString() << "\n";

        for (const RC<Node>& childNode: parentNode->children)
            Print(childNode, depth + 1);
	}
}
