#include "Core.h"

namespace Gularen {
    Node::Node(NodeType type, NodeGroup group): type(type), group(group) {}

    Node::Node(NodeType type, NodeGroup group, const NodeChildren& children): type(type), group(group), children(children) {}

    String Node::toString() const { 
        return "[unimplemented]"; 
    }

    bool operator==(const Node& a, const Node& b) {
        if (a.type != b.type) 
            return false;

        // NOTE: I think There is no need to compare group

        // TODO: Implement GetHash instead
        if (a.toString() != b.toString())
            return false;

        if (a.children.size() != b.children.size())
            return false;

        for (std::size_t i = 0; i < a.children.size(); ++i)
            if (!(*a.children[i] == *b.children[i]))
                return false;

        return true;
    }
}