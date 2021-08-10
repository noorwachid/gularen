#pragma once

#include "NodeType.hpp"
#include "NodeGroup.hpp"
#include <vector>

namespace Gularen {

struct Node
{
    Node();
    Node(NodeType type, NodeGroup group = NodeGroup::None);

    virtual ~Node();

    void Add(Node* node);

    virtual std::string ToString();

    NodeType type;
    NodeGroup group;
    std::vector<Node*> children;
};

struct ValueNode: public Node
{
    ValueNode();
    ValueNode(NodeType type, NodeGroup group = NodeGroup::None);
    ValueNode(NodeType type, const std::string& value);

    std::string ToString() override;

    std::string value;
};

struct SizeNode: public Node
{
    SizeNode();
    SizeNode(NodeType type, const size_t size);

    std::string ToString() override;

    size_t size;
};

struct BooleanNode: public Node
{
    BooleanNode();
    BooleanNode(NodeType type, NodeGroup group = NodeGroup::None, bool state = false);

    std::string ToString() override;

    bool state;
};

struct ContainerNode: public Node
{
    ContainerNode();
    ContainerNode(NodeType type, NodeGroup group = NodeGroup::None, Node* package = nullptr);

    std::string ToString() override;

    Node* package;
};

}

