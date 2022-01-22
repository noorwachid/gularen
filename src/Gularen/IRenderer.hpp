#pragma once

#include "Node.hpp"

namespace Gularen
{
	class IRenderer
	{
	public:
		virtual void SetTree(Node* tree) = 0;

		virtual void Parse() = 0;

		virtual std::string GetBuffer() = 0;
	};
}
