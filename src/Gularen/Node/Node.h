#pragma once

#include "../DS/Array.h"
#include "../DS/RC.h"
#include "../DS/String.h"

namespace Gularen {
	enum struct NodeType {
		root,

		title,
		subtitle,

		document,
		part,
		chapter,
		section,
		subsection,
		subsubsection,
		segment,

		indentation,

		paragraph,

		text,
		newline,

		lineBreak,
		pageBreak,

		boldFS,
		italicFS,
		monospaceFS,

		lsQuote,
		rsQuote,
		ldQuote,
		rdQuote,

		list,
		item,

		numericList,
		numericItem,
	};

	enum struct NodeGroup {
		Inline,
		Block,
		VirtualBlock,
	};

	struct Node;

	using NodeChildren = Array<RC<Node>>;

	struct Node {
		NodeType type;
		NodeGroup group;

		NodeChildren children;

		Node(NodeType type, NodeGroup group);

		Node(NodeType type, NodeGroup group, const NodeChildren& children);

		virtual String toString() const;
	};

	bool operator==(const Node& a, const Node& b);
}
