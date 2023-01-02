#include "Heading.h"

namespace Gularen {
    HeadingNode::HeadingNode(NodeType type): Node(type) {
    }

    HeadingNode::HeadingNode(NodeType type, const NodeChildren &children): Node(type, children) {
    }

    String HeadingNode::toString() const {
        assert(false && "DO NOT CALL THIS FUNCTION DIRECTLY");
        return "Heading";
    }
    
    TitleNode::TitleNode(): HeadingNode(NodeType::title) {
    }

    TitleNode::TitleNode(const NodeChildren &children): HeadingNode(NodeType::title, children) {
    }

    String TitleNode::toString() const {
        return "title";
    }
    
    SubtitleNode::SubtitleNode(): HeadingNode(NodeType::title) {
    }

    SubtitleNode::SubtitleNode(const NodeChildren &children): HeadingNode(NodeType::title, children) {
    }

    String SubtitleNode::toString() const {
        return "subtitle";
    }

    #define GULAREN_HEADING_DEFINITION(classType, enumType)                                                       \
    classType##Node::classType##Node(): HeadingNode(NodeType::enumType) {}                                       \
    classType##Node::classType##Node(const NodeChildren &children): HeadingNode(NodeType::enumType, children) {} \
    String classType##Node::toString() const {                                                                   \
        return #enumType " id:\"" + id + "\"";                                                                   \
    }

    GULAREN_HEADING_DEFINITION(Document, document)
    GULAREN_HEADING_DEFINITION(Part, part)
    GULAREN_HEADING_DEFINITION(Chapter, chapter)
    GULAREN_HEADING_DEFINITION(Section, section)
    GULAREN_HEADING_DEFINITION(Subsection, subsection)
    GULAREN_HEADING_DEFINITION(Subsubsection, subsubsection)
    GULAREN_HEADING_DEFINITION(Segment, segment)
}