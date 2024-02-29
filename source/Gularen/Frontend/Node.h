#pragma once

#include "Gularen/Frontend/Lexer.h"

namespace Gularen {

enum class NodeKind {
	comment,

	text,

	style,

	paragraph,

	space,

	heading,
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

}
