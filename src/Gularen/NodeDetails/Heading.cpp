#include "Heading.h"

namespace Gularen 
{
    HeadingNode::HeadingNode(NodeType type): Node(type)
    {
    }

    HeadingNode::HeadingNode(NodeType type, const NodeChildren &children): Node(type, children)
    {
    }

    String HeadingNode::ToString() const
    {
        assert(false && "DO NOT CALL THIS FUNCTION DIRECTLY");
        return "Heading";
    }
    
    TitleNode::TitleNode(): HeadingNode(NodeType::Title)
    {
    }

    TitleNode::TitleNode(const NodeChildren &children): HeadingNode(NodeType::Title, children)
    {
    }

    String TitleNode::ToString() const
    {
        return "Title";
    }
    
    SubtitleNode::SubtitleNode(): HeadingNode(NodeType::Title)
    {
    }

    SubtitleNode::SubtitleNode(const NodeChildren &children): HeadingNode(NodeType::Title, children)
    {
    }

    String SubtitleNode::ToString() const
    {
        return "Subtitle";
    }

    #define GULAREN_HEADING_DEFINITION(type)                                                        \
    type##Node::type##Node(): HeadingNode(NodeType::type) {}                                       \
    type##Node::type##Node(const NodeChildren &children): HeadingNode(NodeType::type, children) {} \
    String type##Node::ToString() const                                                            \
    {                                                                                              \
        return #type " id:\"" + id + "\"";                                                    \
    }

    GULAREN_HEADING_DEFINITION(Document)
    GULAREN_HEADING_DEFINITION(Part)
    GULAREN_HEADING_DEFINITION(Chapter)
    GULAREN_HEADING_DEFINITION(Section)
    GULAREN_HEADING_DEFINITION(Subsection)
    GULAREN_HEADING_DEFINITION(Subsubsection)
    GULAREN_HEADING_DEFINITION(Segment)
}