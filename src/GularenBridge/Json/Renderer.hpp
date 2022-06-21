#pragma once

#include <string>
#include <Gularen/Node.hpp>
#include <Gularen/IRenderer.hpp>

namespace GularenBridge {
	namespace Json {
		class Renderer : public Gularen::IRenderer {
		public:
			Renderer();

			void setTree(Gularen::Node* tree) override;

			void parse() override;

			std::string getBuffer() override;

		private:
			void traverse(Gularen::Node* node);

			void preTraverse(Gularen::Node* node);

			void postTraverse(Gularen::Node* node);

			std::string escapeBuffer(const std::string& buffer);

		private:
			Gularen::Node* rootNode;
			std::string buffer;
		};
	}
}
