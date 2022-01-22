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

			void setTree(Node* tree) override;

			void parse() override;

			std::string getBuffer() override;

			std::string getContentBuffer();

			void setStyle(const std::string& style);

		private:
			void traverse(Node* node);

			void preTraverse(Node* node);

			void postTraverse(Node* node);

			std::string escapeBuffer(const std::string& buffer);

		private:
			Node* rootNode;
			std::string buffer;
			std::string titleBuffer;
			std::string styleBuffer;
			bool inTitle;
		};
	}
}