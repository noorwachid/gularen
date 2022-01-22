#pragma once

#include <string>
#include <Gularen/Node.hpp>
#include <Gularen/IRenderer.hpp>

namespace GularenBridge
{
	namespace Html
	{
		using Gularen::NodeType;
		using Gularen::Node;
		using Gularen::ValueNode;
		using Gularen::BooleanNode;
		using Gularen::TernaryNode;
		using Gularen::TernaryState;
		using Gularen::ContainerNode;
		using Gularen::CodeNode;
		using Gularen::IRenderer;

		class Renderer : public IRenderer
		{
		public:
			Renderer();

			void SetTree(Node* tree) override;

			void Parse() override;

			std::string GetBuffer() override;

			std::string GetContentBuffer();

			void SetStyle(const std::string& style);

		private:
			void Traverse(Node* node);

			void PreTraverse(Node* node);

			void PostTraverse(Node* node);

			std::string EscapeBuffer(const std::string& buffer);

		private:
			Node* mTree;
			std::string mBuffer;
			std::string mTitleBuffer;
			std::string mStyleBuffer;
			bool mInTitle;
		};
	}
}