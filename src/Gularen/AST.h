#pragma once 

#include "Integer.h"
#include "Node.h"

namespace Gularen
{
	class AST 
	{
	public:
		AST() = default;

		AST(const RC<Node>& rootNode);

		void SetRootNode(const RC<Node>& rootNode);

		const RC<Node>& GetRootNode() const;

        void Print();

	private:
        void Print(const RC<Node>& parentNode, UintSize depth);

		RC<Node> _rootNode;
	};
}
