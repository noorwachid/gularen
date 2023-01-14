#include "HeadingNode.h"

namespace Gularen
{
    HeadingNode::HeadingNode(NodeType type) : Node(type, NodeGroup::Block) {}

    HeadingNode::HeadingNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {}

    String HeadingNode::ToString() const
    {
        assert(false && "DO NOT CALL THIS FUNCTION DIRECTLY");
        return "Heading";
    }

    TitleNode::TitleNode() : HeadingNode(NodeType::Title) {}

    TitleNode::TitleNode(const NodeChildren& children) : HeadingNode(NodeType::Title, children) {}

    String TitleNode::ToString() const { return "Title"; }

    SubtitleNode::SubtitleNode() : HeadingNode(NodeType::Title) {}

    SubtitleNode::SubtitleNode(const NodeChildren& children) : HeadingNode(NodeType::Title, children) {}

    String SubtitleNode::ToString() const { return "Subtitle"; }

#define GULAREN_HEADING_DEFINITION(classType)                                                                \
    classType##Node::classType##Node() : HeadingNode(NodeType::classType)                                               \
    {                                                                                                                  \
    }                                                                                                                  \
    classType##Node::classType##Node(const NodeChildren& children) : HeadingNode(NodeType::classType, children)         \
    {                                                                                                                  \
    }                                                                                                                  \
    String classType##Node::ToString() const                                                                           \
    {                                                                                                                  \
        return #classType " id:\"" + id + "\"";                                                                         \
    }

    GULAREN_HEADING_DEFINITION(Document)
    GULAREN_HEADING_DEFINITION(Part)
    GULAREN_HEADING_DEFINITION(Chapter)
    GULAREN_HEADING_DEFINITION(Section)
    GULAREN_HEADING_DEFINITION(Subsection)
    GULAREN_HEADING_DEFINITION(Subsubsection)
    GULAREN_HEADING_DEFINITION(Segment)
}
