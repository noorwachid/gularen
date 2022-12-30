#include "Core.h"

namespace Gularen 
{
    Node::Node(NodeType type): type(type) {}

    Node::Node(NodeType type, const NodeChildren& children): type(type), children(children) {}

    String Node::ToString() const
    { 
        return ""; 
    }

    bool operator==(const Node& a, const Node& b)
    {
        if (a.type != b.type) 
            return false;

        // TODO: Implement GetHash instead
        if (a.ToString() != b.ToString())
            return false;

        if (a.children.size() != b.children.size())
            return false;

        for (std::size_t i = 0; i < a.children.size(); ++i)
            if (!(*a.children[i] == *b.children[i]))
                return false;

        return true;
    }
}