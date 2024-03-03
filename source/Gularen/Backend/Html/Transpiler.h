#pragma once

#include "Gularen/Frontend/Parser.h"
#include "Gularen/Library/String.h"
#include "Gularen/Library/StringSlice.h"

namespace Gularen::Html {

class Transpiler {
public:
	StringSlice transpile(StringSlice content) {
		Parser parser;
		Slice<Node*> nodes = parser.parse(content);

		_content = String();
		_tableAlignments = Slice<Table::Alignment>(nullptr, 0);
		_tableColumnIndex = 0;

		for (unsigned int i = 0; i < nodes.size(); i += 1) {
			_transpile(nodes.get(i), 0);
		}

		return StringSlice(_content.pointer(), _content.size());
	}

private:
	void _transpile(const Node* node, unsigned int depth) {
	}

private:
	String _content;

	Slice<Table::Alignment> _tableAlignments;

	unsigned int _tableColumnIndex;
};

}
