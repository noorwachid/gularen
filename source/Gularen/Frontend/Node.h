#pragma once

#include "Gularen/Frontend/Lexer.h"

namespace Gularen {

enum class NodeKind {
	comment,

	text,

	style,

	paragraph,
	space,
	lineBreak,
	pageBreak,
	dinkus,

	heading,

	indent,

	list,
	numberedList,
	todoList,

	item,
	todoItem,

	table,
	row,
	cell,
};

struct Node {
	Position position;
	NodeKind kind;
	Array<Node*> children;

	Node(Position position, NodeKind kind): position(position), kind(kind)  {
	}

	virtual void print() {
	}
};

struct Comment : Node {
	StringSlice value;

	Comment(Position position, StringSlice value): Node(position, NodeKind::comment), value(value) {
	}

	virtual void print() override {
		printf("comment %.*s\n", value.size(), value.pointer());
	}
};

struct Text : Node {
	StringSlice value;

	Text(Position position, StringSlice value): Node(position, NodeKind::text), value(value) {
	}

	virtual void print() override {
		printf("text %.*s\n", value.size(), value.pointer());
	}
};

struct Space : Node {
	StringSlice value;

	Space(Position position): Node(position, NodeKind::space) {
	}

	virtual void print() override {
		printf("space\n");
	}
};

struct LineBreak : Node {
	StringSlice value;

	LineBreak(Position position): Node(position, NodeKind::lineBreak) {
	}

	virtual void print() override {
		printf("lineBreak\n");
	}
};

struct PageBreak : Node {
	StringSlice value;

	PageBreak(Position position): Node(position, NodeKind::pageBreak) {
	}

	virtual void print() override {
		printf("pageBreak\n");
	}
};

struct Dinkus : Node {
	StringSlice value;

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
		monospaced,
	};

	Type type;

	Style(Position position, Type type): Node(position, NodeKind::style), type(type) {
	}


	virtual void print() override {
		printf("style ");

		switch (type) {
			case Type::bold: printf("bold\n"); break;
			case Type::italic: printf("italic\n"); break;
			case Type::monospaced: printf("monospaced\n"); break;
		}
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
		chapter,
		section,
		subsection,

		title,
		subtitle,
	};

	Type type;

	Heading(Position position): Node(position, NodeKind::heading) {
	}

	virtual void print() override {
		switch (type) {
			case Type::chapter: printf("chapter\n"); break;
			case Type::section: printf("section\n"); break;
			case Type::subsection: printf("subsection\n"); break;

			case Type::title: printf("title\n"); break;
			case Type::subtitle: printf("subtitle\n"); break;
		}
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
			case NodeKind::todoList: printf("todoList\n"); break;
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

struct TodoItem : Node {
	enum class State {
		todo,
		done,
		cancelled,
	};

	State state;

	TodoItem(Position position): Node(position, NodeKind::todoItem) {
	}

	virtual void print() override {
		printf("todoItem ");
		switch (state) {
			case State::todo: printf("todo\n"); break;
			case State::done: printf("done\n"); break;
			case State::cancelled: printf("cancelled\n"); break;
		}
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

}
