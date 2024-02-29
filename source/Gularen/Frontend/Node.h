#pragma once

#include "Gularen/Frontend/Lexer.h"

namespace Gularen {

enum class NodeKind {
	comment,

	text,

	style,

	paragraph,

	chapter,
	section,
	subsection,
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

struct Chapter : Node {
	Chapter(Position position): Node(position, NodeKind::chapter) {
	}

	virtual void print() override {
		printf("chapter\n");
	}
};

struct Section : Node {
	Section(Position position): Node(position, NodeKind::section) {
	}

	virtual void print() override {
		printf("section\n");
	}
};

struct Subsection : Node {
	Subsection(Position position): Node(position, NodeKind::subsection) {
	}

	virtual void print() override {
		printf("subsection\n");
	}
};

}
