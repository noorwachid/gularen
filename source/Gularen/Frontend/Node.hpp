#pragma once

#include "Gularen/Frontend/Helper.hpp"
#include "Gularen/Frontend/Lexer.hpp"
#include <string>

namespace Gularen {

enum class NodeKind {
	document, 

	comment,

	text,

	emphasis,
	highlight,
	change,

	paragraph,
	space,
	lineBreak,
	pageBreak,
	dinkus,

	heading,
	title,
	subtitle,
	content,

	quote,

	list,
	numberedList,
	checkList,
	definitionList,

	item,
	checkItem,
	definitionItem,
	definitionTerm,
	definitionDesc,

	table,
	row,
	cell,

	code,
	codeBlock,

	link,
	view,
	footnote,
	inText,
	reference,
	referenceInfo,

	punct,

	emoji,
	dateTime,
	admonition,

	accountTag,
	hashTag,
};

struct Pair {
	std::string_view key;
	std::string_view value;
};

struct Node {
	Range range;
	NodeKind kind;
	std::vector<Node*> children;
	std::vector<Pair> annotations;

	Node(Range range, NodeKind kind): range(range), kind(kind)  {
	}

	virtual ~Node() {
		for (size_t i = 0; i < children.size(); i += 1) {
			delete children[i];
			children[i] = nullptr;
		}
	}
};

struct Document : Node {
	std::string path;
	std::string content;

	Document(): Node({}, NodeKind::document) {
	}

	Document(Range range, std::string_view path): Node(range, NodeKind::document), path(path) {
	}
};


struct Comment : Node {
	std::string_view content;

	Comment(Range range, std::string_view content): Node(range, NodeKind::comment), content(content) {
	}
};

struct Text : Node {
	std::string_view content;

	Text(Range range, std::string_view content): Node(range, NodeKind::text), content(content) {
	}
};

struct Space : Node {
	Space(Range range): Node(range, NodeKind::space) {
	}
};

struct LineBreak : Node {
	LineBreak(Range range): Node(range, NodeKind::lineBreak) {
	}
};

struct PageBreak : Node {
	PageBreak(Range range): Node(range, NodeKind::pageBreak) {
	}
};

struct Dinkus : Node {
	Dinkus(Range range): Node(range, NodeKind::dinkus) {
	}
};

struct Emphasis : Node {
	enum class Type {
		bold,
		italic,
		underline,
	};

	Type type;

	Emphasis(Range range, Type type): Node(range, NodeKind::emphasis), type(type) {
	}
};

struct Highlight : Node {
	Highlight(Range range): Node(range, NodeKind::highlight) {
	}
};

struct Change : Node {
	enum class Type {
		added,
		removed,
	};

	Type type;

	Change(Range range, Type type): Node(range, NodeKind::change), type(type) {
	}
};

struct Paragraph : Node {
	Paragraph(Range range): Node(range, NodeKind::paragraph) {
	}
};

struct Heading : Node {
	enum class Type {
		chapter,
		section,
		subsection,
	};

	Type type;

	Heading(Range range): Node(range, NodeKind::heading) {
	}
};

struct Title : Node {
	Title(Range range): Node(range, NodeKind::title) {
	}
};

struct Subtitle : Node {
	Subtitle(Range range): Node(range, NodeKind::subtitle) {
	}
};

struct Content : Node {
	Content(Range range): Node(range, NodeKind::content) {
	}
};

struct Quote : Node {
	Quote(Range range): Node(range, NodeKind::quote) {
	}
};

struct List : Node {
	List(Range range, NodeKind kind): Node(range, kind) {
	}
};

struct Item : Node {
	Item(Range range): Node(range, NodeKind::item) {
	}
};

struct CheckItem : Node {
	bool checked;

	CheckItem(Range range): Node(range, NodeKind::checkItem) {
	}
};

struct DefinitionItem : Node {
	DefinitionItem(Range range): Node(range, NodeKind::definitionItem) {
	}
};

struct DefinitionTerm : Node {
	DefinitionTerm(Range range): Node(range, NodeKind::definitionTerm) {
	}
};

struct DefinitionDesc : Node {
	DefinitionDesc(Range range): Node(range, NodeKind::definitionDesc) {
	}
};

struct Table : Node {
	enum class Alignment {
		left,
		center,
		right,
	};

	std::vector<Alignment> alignments;

	Table(Range range): Node(range, NodeKind::table) {
	}
};

struct Row : Node {
	enum class Type {
		header,
		content,
		footer,
	};

	Type type;

	Row(Range range): Node(range, NodeKind::row) {
	}
};

struct Cell : Node {
	Cell(Range range): Node(range, NodeKind::cell) {
	}
};

struct Code : Node {
	std::string_view label;
	std::string_view content;

	Code(Range range): Node(range, NodeKind::code) {
	}
};

struct CodeBlock : Code {
	CodeBlock(Range range): Code(range) {
		kind = NodeKind::codeBlock;
	}
};

struct Link : Node {
	std::string_view resource;
	std::vector<std::string_view> headings;
	std::string_view label;

	Link(Range range): Node(range, NodeKind::link) {
	}

	void setResource(std::string_view resource) {
		bool found = false;
		size_t startIndex = 0;

		while (startIndex < resource.size()) {
			if (resource[startIndex] == '>') {
				found = true;
				break;
			}
			startIndex += 1;
		}

		this->resource = resource.substr(0, startIndex);

		if (found) {
			size_t index = startIndex + 1;
			while (index < resource.size()) {
				if (resource[index] == '>') {
					std::string_view division = resource.substr(startIndex + 1, index - startIndex - 1);
					headings.push_back(division);
					startIndex = index;
				}
				index += 1;
			}

			std::string_view division = resource.substr(startIndex + 1, index - startIndex - 1);
			headings.push_back(division);
		}
	}
};

struct View : Node {
	std::string_view resource;
	std::string_view label;

	View(Range range): Node(range, NodeKind::view) {
	}
};

struct Footnote : Node {
	std::string_view desc;

	Footnote(Range range, std::string_view description): Node(range, NodeKind::footnote), desc(description) {
	}
};

struct InText : Node {
	std::string_view id;

	InText(Range range): Node(range, NodeKind::inText) {
	}
};

struct ReferenceInfo : Node {
	std::string_view key;
	
	ReferenceInfo(Range range, std::string_view key): Node(range, NodeKind::referenceInfo) {
		this->key = Helper::trim(key);
	}
};

struct Reference : Node {
	std::string_view id;

	Reference(Range range): Node(range, NodeKind::reference) {
	}
};

struct Emoji : Node {
	std::string_view code;

	Emoji(Range range, std::string_view code): Node(range, NodeKind::emoji), code(code) {
	}
};

struct DateTime : Node {
	std::string_view date;
	std::string_view time;

	std::string_view content;

	DateTime(Range range, std::string_view content): Node(range, NodeKind::dateTime), content(content) {
		bool hasDate = false;
		bool hasTime = false;

		size_t timeBegin = 0;

		for (size_t iter = 0; iter < content.size(); iter += 1) {
			if (content[iter] == '-') {
				hasDate = true;
			}
			if (content[iter] == ':') {
				hasTime = true;
			}
			if (content[iter] == ' ') {
				timeBegin = iter + 1;
			}
		}

		if (hasDate) {
			date = content.substr(0, timeBegin == 0 ? content.size() : timeBegin - 1);
		}
		if (hasTime) {
			time = content.substr(timeBegin, content.size() - timeBegin);
		}
	}
};

struct Punct : Node {
	enum class Type {
		hypen,
		enDash,
		emDash,

		quoteOpen,
		quoteClose,
		squoteOpen,
		squoteClose,
	};

	Type type;

	Punct(Range range, Type type): Node(range, NodeKind::punct), type(type) {
	}
};

struct Admonition : Node {
	std::string_view label;

	Admonition(Range range): Node(range, NodeKind::admonition) {
	}
};

struct AccountTag : Node {
	std::string_view resource;

	AccountTag(Range range, std::string_view resource): Node(range, NodeKind::accountTag), resource(resource) {
	}
};

struct HashTag : Node {
	std::string_view resource;

	HashTag(Range range, std::string_view resource): Node(range, NodeKind::hashTag), resource(resource) {
	}
};

}
