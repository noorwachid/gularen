#include "Node.hpp"

namespace Gularen {

Node::Node():
    type(NodeType::Unknown)
{}

Node::Node(NodeType type):
    type(type)
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

BooleanNode::BooleanNode(NodeType type, bool state):
    Node(type),
    state(state)
{}

std::string BooleanNode::ToString()
{
    return Gularen::ToString(type) + ": " + (state ? "true" : "false");
}

}
