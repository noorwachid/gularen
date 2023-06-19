#include "HeadingNode.h"

namespace Gularen {
	HeadingNode::HeadingNode(NodeType type) : Node(type, NodeGroup::Block) {
	}

	HeadingNode::HeadingNode(NodeType type, const NodeChildren& children) : Node(type, NodeGroup::Block, children) {
	}

	String HeadingNode::toString() const {
		assert(false && "DO NOT CALL THIS FUNCTION DIRECTLY");
		return "heading";
	}

	TitleNode::TitleNode() : HeadingNode(NodeType::title) {
	}

	TitleNode::TitleNode(const NodeChildren& children) : HeadingNode(NodeType::title, children) {
	}

	String TitleNode::toString() const {
		return "title";
	}

	SubtitleNode::SubtitleNode() : HeadingNode(NodeType::title) {
	}

	SubtitleNode::SubtitleNode(const NodeChildren& children) : HeadingNode(NodeType::title, children) {
	}

	String SubtitleNode::toString() const {
		return "subtitle";
	}

#define GULAREN_HEADING_DEFINITION(classType, enumType) \
	classType::classType() : HeadingNode(NodeType::enumType) { \
	} \
	classType::classType(const NodeChildren& children) : HeadingNode(NodeType::enumType, children) { \
	} \
	String classType::toString() const { \
		return #classType " id:\"" + id + "\""; \
	}

	GULAREN_HEADING_DEFINITION(DocumentNode, document)
	GULAREN_HEADING_DEFINITION(PartNode, part)
	GULAREN_HEADING_DEFINITION(ChapterNode, chapter)
	GULAREN_HEADING_DEFINITION(SectionNode, section)
	GULAREN_HEADING_DEFINITION(SubsectionNode, subsection)
	GULAREN_HEADING_DEFINITION(SubsubsectionNode, subsubsection)
	GULAREN_HEADING_DEFINITION(SegmentNode, segment)
}
