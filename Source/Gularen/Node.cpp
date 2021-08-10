#include "Node.hpp"

namespace Gularen {

Node::Node():
    type(NodeType::Unknown),
    group(NodeGroup::None)
{}

Node::Node(NodeType type, NodeGroup group):
    type(type),
    group(group)
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

// --- ValueNode ---

ValueNode::ValueNode():
    Node(NodeType::Unknown)
{}

ValueNode::ValueNode(NodeType type, NodeGroup group):
    Node(type, group)
{
}

Gularen::ValueNode::ValueNode(Gularen::NodeType type, const std::string& value):
    Node(type),
    value(value)
{}

std::string ValueNode::ToString()
{
    return Gularen::ToString(type) + ": \"" + value + "\"";;
}

// --- SizeNode ---
SizeNode::SizeNode():
    Node(NodeType::Unknown)
{}

SizeNode::SizeNode(NodeType type, const size_t size):
    Node(type),
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

BooleanNode::BooleanNode(NodeType type, NodeGroup group, bool state):
    Node(type, group),
    state(state)
{}

std::string BooleanNode::ToString()
{
    return Gularen::ToString(type) + ": " + (state ? "true" : "false");
}

ContainerNode::ContainerNode():
    Node(NodeType::Unknown),
    package(nullptr)
{}

ContainerNode::ContainerNode(NodeType type, NodeGroup group, Node *node):
    Node(type, group),
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

}
