#pragma once

#include <string>
#include <Gularen/Node.hpp>
#include <Gularen/IRenderer.hpp>

namespace GularenBridge {
	namespace Json {
		using Gularen::NodeType;
		using Gularen::Node;
		using Gularen::ValueNode;
		using Gularen::SizeNode;
		using Gularen::BooleanNode;
		using Gularen::ContainerNode;
		using Gularen::CodeNode;
		using Gularen::TableNode;
		using Gularen::IRenderer;

		class Renderer : public IRenderer {
		public:
			Renderer();

			void setTree(Node* tree) override;

			void parse() override;

			std::string getBuffer() override;

		private:
			void traverse(Node* node);

			void preTraverse(Node* node);

			void postTraverse(Node* node);

			std::string escapeBuffer(const std::string& buffer);

		private:
			Node* rootNode;
			std::string buffer;
		};
	}
}
