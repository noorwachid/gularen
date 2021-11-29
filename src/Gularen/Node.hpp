#pragma once

#include "NodeType.hpp"
#include "NodeGroup.hpp"
#include <vector>

namespace Gularen {

struct Node
{
    Node();
    Node(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown);

    virtual ~Node();

    void Add(Node* node);

    virtual std::string ToString();

    NodeType type;
    NodeGroup group;
    NodeShape shape;
    std::vector<Node*> children;
};

struct ValueNode: public Node
{
    ValueNode();
    ValueNode(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown, const std::string& value = std::string());

    std::string ToString() override;

    std::string value;
};

struct SizeNode: public Node
{
    SizeNode();
    SizeNode(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown, const size_t size = 0);

    std::string ToString() override;

    size_t size;
};

struct BooleanNode: public Node
{
    BooleanNode();
    BooleanNode(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown, bool state = false);

    std::string ToString() override;

    bool state;
};

enum struct TernaryState {
    False = 1,
    InBetween = 2,
    True = 3,
};

struct TernaryNode: public Node
{
    TernaryNode();
    TernaryNode(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown, TernaryState state = TernaryState::False);

    std::string ToString() override;

    TernaryState state;
};

struct ContainerNode: public Node
{
    ContainerNode();
    ContainerNode(NodeType type, NodeGroup group = NodeGroup::Unknown, NodeShape shape = NodeShape::Unknown, Node* package = nullptr);

    std::string ToString() override;

    Node* package;
};

struct CodeNode: public Node
{
    CodeNode();
    CodeNode(const std::string& value, Node* lang = nullptr);

    std::string ToString() override;

    std::string value;
    Node* lang;
};

struct TableNode: public Node
{
    TableNode();

    std::vector<ssize_t> hHeaders;
    std::vector<ssize_t> vHeaders;
};

}
