#include "Node.hpp"

namespace Gularen {

Node::Node():
    type(NodeType::Unknown),
    group(NodeGroup::Unknown),
    shape(NodeShape::Unknown)
{}

Node::Node(NodeType type, NodeGroup group, NodeShape shape):
    type(type),
    group(group),
    shape(shape)
{}

Node::~Node()
{
}

void Node::Add(Node* node)
{
    children.push_back(node);
}

std::string Node::ToString()
{
    return Gularen::ToString(type) + ":";
}

ValueNode::ValueNode():
    Node()
{}

ValueNode::ValueNode(NodeType type, NodeGroup group, NodeShape shape, const std::string &value):
    Node(type, group, shape),
    value(value)
{
}

std::string ValueNode::ToString()
{
    return Gularen::ToString(type) + ": \"" + value + "\"";;
}

SizeNode::SizeNode():
    Node()
{}

SizeNode::SizeNode(NodeType type, NodeGroup group, NodeShape shape, const size_t size):
    Node(type, group, shape),
    size(size)
{}

std::string SizeNode::ToString()
{
    return Gularen::ToString(type) + ": " + std::to_string(size);
}

BooleanNode::BooleanNode():
    Node(NodeType::Unknown),
    state(false)
{}

BooleanNode::BooleanNode(NodeType type, NodeGroup group, NodeShape shape, bool state):
    Node(type, group, shape),
    state(state)
{}

std::string BooleanNode::ToString()
{
    return Gularen::ToString(type) + ": " + (state ? "True" : "False");
}

TernaryNode::TernaryNode():
    Node(),
    state(TernaryState::False)
{
}

TernaryNode::TernaryNode(NodeType type, NodeGroup group, NodeShape shape, TernaryState state):
    Node(type, group, shape),
    state(state)
{
}

std::string TernaryNode::ToString()
{
    std::string buffer = Gularen::ToString(type) + ": ";

    switch (state) {
        case TernaryState::False:
            buffer += "False";
            break;
        case TernaryState::True:
            buffer += "True";
            break;
        default:
            buffer += "InBetween";
            break;
    }

    return buffer;
}

ContainerNode::ContainerNode():
    Node(),
    package(nullptr)
{}

ContainerNode::ContainerNode(NodeType type, NodeGroup group, NodeShape shape, Node *node):
    Node(type, group, shape),
    package(node)
{}

std::string ContainerNode::ToString()
{
    std::string buffer = Gularen::ToString(type) + ": ";
    if (package)
    {
        if (package->type == NodeType::Symbol)
            buffer += "%" + static_cast<ValueNode*>(package)->value;

        if (package->type == NodeType::QuotedText)
            buffer += "\"" + static_cast<ValueNode*>(package)->value + "\"";
    }
    return buffer;
}

TableNode::TableNode():
    Node(NodeType::Table, NodeGroup::Table, NodeShape::Block)
{
}

CodeNode::CodeNode():
    Node(NodeType::Code, NodeGroup::Code, NodeShape::Block)
{
}

CodeNode::CodeNode(const std::string &value, Node *lang):
    Node(NodeType::Code, NodeGroup::Code, NodeShape::Block),
    value(value),
    lang(lang)
{
}

std::string CodeNode::ToString()
{
    std::string buffer = Gularen::ToString(type) + ": ";
    if (lang)
    {
        if (lang->type == NodeType::Symbol)
            buffer += "%" + static_cast<ValueNode*>(lang)->value + " ";

        if (lang->type == NodeType::QuotedText)
            buffer += "\"" + static_cast<ValueNode*>(lang)->value + "\" ";
    }
    return buffer + "\"" + value + "\"";
}

}
