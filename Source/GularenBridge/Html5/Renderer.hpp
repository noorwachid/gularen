#pragma once

#include <string>
#include <Gularen/Node.hpp>
#include <Gularen/IRenderer.hpp>

namespace GularenBridge {
namespace Html5 {

using Gularen::NodeType;
using Gularen::Node;
using Gularen::ValueNode;
using Gularen::BooleanNode;
using Gularen::TernaryNode;
using Gularen::TernaryState;
using Gularen::ContainerNode;
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

    Node* tree;
    std::string buffer;
};

}
}

