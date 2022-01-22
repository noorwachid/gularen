#pragma once

#include <string>
#include <Gularen/Node.hpp>
#include <Gularen/IRenderer.hpp>

namespace GularenBridge
{
	namespace Json
	{
		using Gularen::NodeType;
		using Gularen::Node;
		using Gularen::ValueNode;
		using Gularen::SizeNode;
		using Gularen::BooleanNode;
		using Gularen::ContainerNode;
		using Gularen::CodeNode;
		using Gularen::TableNode;
		using Gularen::IRenderer;

		class Renderer : public IRenderer
		{
		public:
			Renderer();

			void SetTree(Node* tree) override;

			void Parse() override;

			std::string GetBuffer() override;

		private:
			void Traverse(Node* node);

			void PreTraverse(Node* node);

			void PostTraverse(Node* node);

			std::string EscapeBuffer(const std::string& buffer);

		private:
			Node* mTree;
			std::string mBuffer;
		};
	}
}

