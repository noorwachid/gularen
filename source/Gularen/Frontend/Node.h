#pragma once

#include "Gularen/Library/String.h"
#include "Gularen/Library/StringHelper.h"
#include "Gularen/Frontend/Lexer.h"

namespace Gularen {

enum class NodeKind {
	document, 

	comment,

	text,

	style,
	highlight,

	paragraph,
	space,
	lineBreak,
	pageBreak,
	dinkus,

	heading,
	subtitle,

	indent,

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

	blockquote,

	punct,

	emoji,
	dateTime,
	admon,

	accountTag,
	hashTag,
};

struct Annotation {
	StringSlice key;
	StringSlice value;
};

struct Node {
	Position position;
	NodeKind kind;
	Array<Node*> children;
	Array<Annotation> annotations;

	Node(Position position, NodeKind kind): position(position), kind(kind)  {
	}

	virtual ~Node() {
		for (unsigned int i = 0; i < children.size(); i += 1) {
			children.get(i)->~Node();
			delete children.get(i);
			children.get(i) = nullptr;
		}
	}

	virtual void print() {
	}
};

struct Document : Node {
	String path;
	String content;

	Document(): Node({}, NodeKind::document) {
	}

	Document(Position position, StringSlice path): Node(position, NodeKind::document), path(path.pointer(), path.size()) {
	}

	virtual void print() override {
		printf("document %.*s\n", path.size(), path.pointer());
	}
};


struct Comment : Node {
	StringSlice content;

	Comment(Position position, StringSlice content): Node(position, NodeKind::comment), content(content) {
	}

	virtual void print() override {
		printf("comment %.*s\n", content.size(), content.pointer());
	}
};

struct Text : Node {
	StringSlice content;

	Text(Position position, StringSlice content): Node(position, NodeKind::text), content(content) {
	}

	virtual void print() override {
		printf("text %.*s\n", content.size(), content.pointer());
	}
};

struct Space : Node {
	Space(Position position): Node(position, NodeKind::space) {
	}

	virtual void print() override {
		printf("space\n");
	}
};

struct LineBreak : Node {
	LineBreak(Position position): Node(position, NodeKind::lineBreak) {
	}

	virtual void print() override {
		printf("lineBreak\n");
	}
};

struct PageBreak : Node {
	PageBreak(Position position): Node(position, NodeKind::pageBreak) {
	}

	virtual void print() override {
		printf("pageBreak\n");
	}
};

struct Dinkus : Node {
	Dinkus(Position position): Node(position, NodeKind::dinkus) {
	}

	virtual void print() override {
		printf("dinkus\n");
	}
};

struct Style : Node {
	enum class Type {
		bold,
		italic,
	};

	Type type;

	Style(Position position, Type type): Node(position, NodeKind::style), type(type) {
	}


	virtual void print() override {
		printf("style ");

		switch (type) {
			case Type::bold: printf("bold\n"); break;
			case Type::italic: printf("italic\n"); break;
		}
	}
};

struct Highlight : Node {
	Highlight(Position position): Node(position, NodeKind::highlight) {
	}

	virtual void print() override {
		printf("highlight\n");
	}
};

struct Paragraph : Node {
	Paragraph(Position position): Node(position, NodeKind::paragraph) {
	}

	virtual void print() override {
		printf("paragraph\n");
	}
};

struct Heading : Node {
	enum class Type {
		section,
		subsection,
		subsubsection,
	};

	Type type;

	Heading(Position position): Node(position, NodeKind::heading) {
	}

	virtual void print() override {
		switch (type) {
			case Type::section: printf("section\n"); break;
			case Type::subsection: printf("subsection\n"); break;
			case Type::subsubsection: printf("subsubsection\n"); break;
		}
	}
};

struct Subtitle : Node {
	Subtitle(Position position): Node(position, NodeKind::subtitle) {
	}

	virtual void print() override {
		printf("subtitle\n");
	}
};

struct Indent : Node {
	Indent(Position position): Node(position, NodeKind::indent) {
	}

	virtual void print() override {
		printf("indent\n");
	}
};

struct List : Node {
	List(Position position, NodeKind kind): Node(position, kind) {
	}

	virtual void print() override {
		switch (kind) {
			case NodeKind::list: printf("list\n"); break;
			case NodeKind::numberedList: printf("numberList\n"); break;
			case NodeKind::checkList: printf("checkList\n"); break;
			case NodeKind::definitionList: printf("definitionList\n"); break;
			default: break;
		}
	}
};

struct Item : Node {
	Item(Position position): Node(position, NodeKind::item) {
	}

	virtual void print() override {
		printf("item\n");
	}
};

struct CheckItem : Node {
	enum class State {
		unchecked,
		checked,
	};

	State state;

	CheckItem(Position position): Node(position, NodeKind::checkItem) {
	}

	virtual void print() override {
		printf("checkItem ");
		switch (state) {
			case State::unchecked: printf("todo\n"); break;
			case State::checked: printf("done\n"); break;
		}
	}
};

struct DefinitionItem : Node {
	DefinitionItem(Position position): Node(position, NodeKind::definitionItem) {
	}

	virtual void print() override {
		printf("definitionItem\n");
	}
};

struct DefinitionTerm : Node {
	DefinitionTerm(Position position): Node(position, NodeKind::definitionTerm) {
	}

	virtual void print() override {
		printf("definitionTerm\n");
	}
};

struct DefinitionDesc : Node {
	DefinitionDesc(Position position): Node(position, NodeKind::definitionDesc) {
	}

	virtual void print() override {
		printf("definitionDesc\n");
	}
};

struct Table : Node {
	enum class Alignment {
		left,
		center,
		right,
	};

	Array<Alignment> alignments;

	Table(Position position): Node(position, NodeKind::table) {
	}

	virtual void print() override {
		printf("table ");

		for (unsigned int i = 0; i < alignments.size(); i += 1) {
			if (i != 0) {
				printf(", ");
			}
			switch (alignments.get(i)) {
				case Alignment::left: printf("left"); break;
				case Alignment::center: printf("center"); break;
				case Alignment::right: printf("right"); break;
			}
		}

		printf("\n");
	}
};

struct Row : Node {
	enum class Type {
		header,
		content,
		footer,
	};

	Type type;

	Row(Position position): Node(position, NodeKind::row) {
	}

	virtual void print() override {
		printf("row ");

		switch (type) {
			case Type::header: printf("header\n"); break;
			case Type::content: printf("content\n"); break;
			case Type::footer: printf("footer\n"); break;
		}
	}
};

struct Cell : Node {
	Cell(Position position): Node(position, NodeKind::cell) {
	}

	virtual void print() override {
		printf("cell\n");
	}
};

struct Code : Node {
	StringSlice label;
	StringSlice content;

	Code(Position position): Node(position, NodeKind::code) {
	}

	virtual void print() override {
		printf("code ");

		if (label.size()) {
			printf("%.*s ", label.size(), label.pointer());
		}

		printContent();
		printf("\n");
	}

	void printContent() {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			if (content.get(i) < ' ') {
				switch (content.get(i)) {
					case '\t':
						printf("\\t");
					break;

					case '\n':
						printf("\\n");
					break;

					default:
						printf("\\x%02X", content.get(i) & 0xFF);
					break;
				}
				continue;
			}
			printf("%c", content.get(i));
		}
	}
};

struct CodeBlock : Code {
	CodeBlock(Position position): Code(position) {
		kind = NodeKind::codeBlock;
	}

	virtual void print() override {
		printf("codeBlock ");

		if (label.size()) {
			printf("%.*s ", label.size(), label.pointer());
		}

		printContent();
		printf("\n");
	}
};

struct Link : Node {
	StringSlice resource;
	StringSlice heading;
	StringSlice label;

	Link(Position position): Node(position, NodeKind::link) {
	}

	void setResource(StringSlice resource) {
		unsigned int i = 0;

		while (i < resource.size() && resource.get(i) != '>') {
			i += 1;
		}

		this->resource = resource.cut(0, i);

		if (i < resource.size()) {
			this->heading = resource.cut(i + 1, resource.size() - i - 1);
		}
	}

	virtual void print() override {
		printf("link ");

		if (resource.size()) {
			printf("%.*s ", resource.size(), resource.pointer());
		}


		if (heading.size()) {
			printf(">%.*s ", heading.size(), heading.pointer());
		}

		if (label.size()) {
			printf("%.*s", label.size(), label.pointer());
		}

		printf("\n");
	}
};

struct View : Node {
	StringSlice resource;
	StringSlice label;

	View(Position position): Node(position, NodeKind::view) {
	}

	virtual void print() override {
		printf("view %.*s", resource.size(), resource.pointer());

		if (label.size()) {
			printf(" %.*s", label.size(), label.pointer());
		}

		printf("\n");
	}
};

struct Footnote : Node {
	StringSlice desc;

	Footnote(Position position, StringSlice description): Node(position, NodeKind::footnote), desc(description) {
	}

	virtual void print() override {
		printf("footnote %.*s\n", desc.size(), desc.pointer());
	}
};

struct InText : Node {
	StringSlice id;
	StringSlice label;

	InText(Position position): Node(position, NodeKind::inText) {
	}

	virtual void print() override {
		printf("inText %.*s", id.size(), id.pointer());

		if (label.size() != 0) {
			printf(" %.*s ", label.size(), label.pointer());
		}

		printf("\n");
	}
};

struct ReferenceInfo : Node {
	StringSlice key;

	ReferenceInfo(Position position, StringSlice key): Node(position, NodeKind::referenceInfo), key(key) {
	}

	virtual void print() override {
		printf("referenceInfo %.*s\n", key.size(), key.pointer());
	}
};

struct Reference : Node {
	StringSlice id;

	Reference(Position position): Node(position, NodeKind::reference) {
	}

	virtual void print() override {
		printf("reference %.*s\n", id.size(), id.pointer());
	}
};

struct Emoji : Node {
	StringSlice code;

	Emoji(Position position, StringSlice code): Node(position, NodeKind::emoji), code(code) {
	}

	virtual void print() override {
		printf("emoji %.*s\n", code.size(), code.pointer());
	}
};

struct Blockquote : Node {
	Blockquote(Position position): Node(position, NodeKind::blockquote) {
	}

	virtual void print() override {
		printf("blockquote\n");
	}
};

struct DateTime : Node {
	StringSlice date;
	StringSlice time;

	StringSlice content;

	DateTime(Position position, StringSlice content): Node(position, NodeKind::dateTime), content(content) {
		bool hasDate = false;
		bool hasTime = false;

		unsigned int timeBegin = 0;

		for (unsigned int iter = 0; iter < content.size(); iter += 1) {
			if (content.get(iter) == '-') {
				hasDate = true;
			}
			if (content.get(iter) == ':') {
				hasTime = true;
			}
			if (content.get(iter) == ' ') {
				timeBegin = iter + 1;
			}
		}

		if (hasDate) {
			date = content.cut(0, timeBegin == 0 ? content.size() : timeBegin - 1);
		}
		if (hasTime) {
			time = content.cut(timeBegin, content.size() - timeBegin);
		}
	}

	virtual void print() override {
		printf("dateTime");
		if (date.size() != 0) {
			printf(" %.*s", date.size(), date.pointer());
		}
		if (time.size() != 0) {
			printf(" %.*s", time.size(), time.pointer());
		}
		printf("\n");
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

	Punct(Position position, Type type): Node(position, NodeKind::punct), type(type) {
	}

	virtual void print() override {
		printf("punct ");
		switch (type) {
			case Type::hypen: printf("hyphen\n"); break;
			case Type::enDash: printf("enDash\n"); break;
			case Type::emDash: printf("emDash\n"); break;

			case Type::quoteOpen: printf("quoteOpen\n"); break;
			case Type::quoteClose: printf("quoteClose\n"); break;
			case Type::squoteOpen: printf("squoteOpen\n"); break;
			case Type::squoteClose: printf("squoteClose\n"); break;
		}
	}
};

struct Admon : Node {
	StringSlice label;

	Admon(Position position, StringSlice label): Node(position, NodeKind::admon), label(label) {
	}

	virtual void print() override {
		printf("admon %.*s\n", label.size(), label.pointer());
	}
};

struct AccountTag : Node {
	StringSlice resource;

	AccountTag(Position position, StringSlice resource): Node(position, NodeKind::accountTag), resource(resource) {
	}

	virtual void print() override {
		printf("accountTag %.*s\n", resource.size(), resource.pointer());
	}
};

struct HashTag : Node {
	StringSlice resource;

	HashTag(Position position, StringSlice resource): Node(position, NodeKind::hashTag), resource(resource) {
	}

	virtual void print() override {
		printf("hashTag %.*s\n", resource.size(), resource.pointer());
	}
};

}
