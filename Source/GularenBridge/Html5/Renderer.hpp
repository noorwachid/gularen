#pragma once

#include <string>
#include <Gularen/Node.hpp>

namespace GularenBridge {
namespace Html5 {

using Gularen::NodeType;
using Gularen::Node;
using Gularen::ValueNode;
using Gularen::BooleanNode;

class Renderer
{
public:
    Renderer();

    void SetTree(Node* tree);
    void Parse();

    std::string GetBuffer();

private:
    void Traverse(Node* node);
    void TraverseBeforeChildren(Node* node);
    void TraverseAfterChildren(Node* node);

    Node* tree;
    std::string buffer;
};

}
}

