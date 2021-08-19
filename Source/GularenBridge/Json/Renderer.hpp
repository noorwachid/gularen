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

class Renderer: public IRenderer
{
public:
    Renderer();

    void SetTree(Node* tree) override;
    void Parse() override;

    std::string GetBuffer() override;

private:
    void Traverse(Node* node);
    void TraverseBeforeChildren(Node* node);
    void TraverseAfterChildren(Node* node);
    std::string EscapeText(const std::string& text);

    Node* tree;
    std::string buffer;
};

}
}

