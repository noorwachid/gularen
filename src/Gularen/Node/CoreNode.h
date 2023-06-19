#pragma once

#include "../Integer.h"
#include "Node.h"

namespace Gularen {
	struct RootNode : Node {
		String file = "[memory]";

		RootNode();

		RootNode(const NodeChildren& children);

		virtual String toString() const override;
	};

	struct IndentationNode : Node {
		IndentationNode();

		IndentationNode(const NodeChildren& children);

		virtual String toString() const override;
	};

	struct ParagraphNode : Node {
		ParagraphNode();

		ParagraphNode(const NodeChildren& children);

		virtual String toString() const override;
	};

	struct TextNode : Node {
		String content;

		TextNode(const String& content);

		virtual String toString() const override;
	};

	struct NewlineNode : Node {
		UintSize repetition;

		NewlineNode();

		NewlineNode(UintSize repetition);

		virtual String toString() const override;
	};
}
