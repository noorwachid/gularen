#pragma once

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
	std::string_view key;
	std::string_view value;
};

struct Node {
	Position position;
	NodeKind kind;
	std::vector<Node*> children;
	std::vector<Annotation> annotations;

	Node(Position position, NodeKind kind): position(position), kind(kind)  {
	}

	virtual ~Node() {
		for (unsigned int i = 0; i < children.size(); i += 1) {
			delete children[i];
			children[i] = nullptr;
		}
	}

	virtual void print() {
	}
};

struct Document : Node {
	std::string path;
	std::string content;

	Document(): Node({}, NodeKind::document) {
	}

	Document(Position position, std::string_view path): Node(position, NodeKind::document), path(path) {
	}

	virtual void print() override {
		std::cout << "document " << path << "\n";
	}
};


struct Comment : Node {
	std::string_view content;

	Comment(Position position, std::string_view content): Node(position, NodeKind::comment), content(content) {
	}

	virtual void print() override {
		std::cout << "comment " << content << "\n";
	}
};

struct Text : Node {
	std::string_view content;

	Text(Position position, std::string_view content): Node(position, NodeKind::text), content(content) {
	}

	virtual void print() override {
		std::cout << "text " << content << "\n";
	}
};

struct Space : Node {
	Space(Position position): Node(position, NodeKind::space) {
	}

	virtual void print() override {
		std::cout << "space\n";
	}
};

struct LineBreak : Node {
	LineBreak(Position position): Node(position, NodeKind::lineBreak) {
	}

	virtual void print() override {
		std::cout << "lineBreak\n";
	}
};

struct PageBreak : Node {
	PageBreak(Position position): Node(position, NodeKind::pageBreak) {
	}

	virtual void print() override {
		std::cout << "pageBreak\n";
	}
};

struct Dinkus : Node {
	Dinkus(Position position): Node(position, NodeKind::dinkus) {
	}

	virtual void print() override {
		std::cout << "dinkus\n";
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
		std::cout << "style ";

		switch (type) {
			case Type::bold: std::cout << "bold\n"; break;
			case Type::italic: std::cout << "italic\n"; break;
		}
	}
};

struct Highlight : Node {
	Highlight(Position position): Node(position, NodeKind::highlight) {
	}

	virtual void print() override {
		std::cout << "highlight\n";
	}
};

struct Paragraph : Node {
	Paragraph(Position position): Node(position, NodeKind::paragraph) {
	}

	virtual void print() override {
		std::cout << "paragraph\n";
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
			case Type::section: std::cout << "section\n"; break;
			case Type::subsection: std::cout << "subsection\n"; break;
			case Type::subsubsection: std::cout << "subsubsection\n"; break;
		}
	}
};

struct Subtitle : Node {
	Subtitle(Position position): Node(position, NodeKind::subtitle) {
	}

	virtual void print() override {
		std::cout << "subtitle\n";
	}
};

struct Indent : Node {
	Indent(Position position): Node(position, NodeKind::indent) {
	}

	virtual void print() override {
		std::cout << "indent\n";
	}
};

struct List : Node {
	List(Position position, NodeKind kind): Node(position, kind) {
	}

	virtual void print() override {
		switch (kind) {
			case NodeKind::list: std::cout << "list\n"; break;
			case NodeKind::numberedList: std::cout << "numberList\n"; break;
			case NodeKind::checkList: std::cout << "checkList\n"; break;
			case NodeKind::definitionList: std::cout << "definitionList\n"; break;
			default: break;
		}
	}
};

struct Item : Node {
	Item(Position position): Node(position, NodeKind::item) {
	}

	virtual void print() override {
		std::cout << "item\n";
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
		std::cout << "checkItem ";
		switch (state) {
			case State::unchecked: std::cout << "todo\n"; break;
			case State::checked: std::cout << "done\n"; break;
		}
	}
};

struct DefinitionItem : Node {
	DefinitionItem(Position position): Node(position, NodeKind::definitionItem) {
	}

	virtual void print() override {
		std::cout << "definitionItem\n";
	}
};

struct DefinitionTerm : Node {
	DefinitionTerm(Position position): Node(position, NodeKind::definitionTerm) {
	}

	virtual void print() override {
		std::cout << "definitionTerm\n";
	}
};

struct DefinitionDesc : Node {
	DefinitionDesc(Position position): Node(position, NodeKind::definitionDesc) {
	}

	virtual void print() override {
		std::cout << "definitionDesc\n";
	}
};

struct Table : Node {
	enum class Alignment {
		left,
		center,
		right,
	};

	std::vector<Alignment> alignments;

	Table(Position position): Node(position, NodeKind::table) {
	}

	virtual void print() override {
		std::cout << "table ";

		for (unsigned int i = 0; i < alignments.size(); i += 1) {
			if (i != 0) {
				std::cout << ", ";
			}
			switch (alignments[i]) {
				case Alignment::left: std::cout << "left"; break;
				case Alignment::center: std::cout << "center"; break;
				case Alignment::right: std::cout << "right"; break;
			}
		}

		std::cout << "\n";
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
		std::cout << "row ";

		switch (type) {
			case Type::header: std::cout << "header\n"; break;
			case Type::content: std::cout << "content\n"; break;
			case Type::footer: std::cout << "footer\n"; break;
		}
	}
};

struct Cell : Node {
	Cell(Position position): Node(position, NodeKind::cell) {
	}

	virtual void print() override {
		std::cout << "cell\n";
	}
};

struct Code : Node {
	std::string_view label;
	std::string_view content;

	Code(Position position): Node(position, NodeKind::code) {
	}

	virtual void print() override {
		std::cout << "code ";

		if (label.size()) {
			std::cout << label << " ";
		}

		printContent();
		std::cout << "\n";
	}

	void printContent() {
		for (unsigned int i = 0; i < content.size(); i += 1) {
			if (content[i] < ' ') {
				switch (content[i]) {
					case '\t':
						std::cout << "\\t";
					break;

					case '\n':
						std::cout << "\\n";
					break;

					default:
						std::cout << "\\x" << static_cast<int>(content[i]);
					break;
				}
				continue;
			}
			std::cout << content[i];
		}
	}
};

struct CodeBlock : Code {
	CodeBlock(Position position): Code(position) {
		kind = NodeKind::codeBlock;
	}

	virtual void print() override {
		std::cout << "codeBlock ";

		if (label.size()) {
			std::cout << label << " ";
		}

		printContent();
		std::cout << "\n";
	}
};

struct Link : Node {
	std::string_view resource;
	std::string_view heading;
	std::string_view label;

	Link(Position position): Node(position, NodeKind::link) {
	}

	void setResource(std::string_view resource) {
		unsigned int i = 0;

		while (i < resource.size() && resource[i] != '>') {
			i += 1;
		}

		this->resource = resource.substr(0, i);

		if (i < resource.size()) {
			this->heading = resource.substr(i + 1, resource.size() - i - 1);
		}
	}

	virtual void print() override {
		std::cout << "link";

		if (resource.size()) {
			std::cout << " " << resource;
		}


		if (heading.size()) {
			std::cout << " >" << heading;
		}

		if (label.size()) {
			std::cout << " " << label;
		}

		std::cout << "\n";
	}
};

struct View : Node {
	std::string_view resource;
	std::string_view label;

	View(Position position): Node(position, NodeKind::view) {
	}

	virtual void print() override {
		std::cout << "view " << resource;

		if (label.size()) {
			std::cout << " " << label;
		}

		std::cout << "\n";
	}
};

struct Footnote : Node {
	std::string_view desc;

	Footnote(Position position, std::string_view description): Node(position, NodeKind::footnote), desc(description) {
	}

	virtual void print() override {
		std::cout << "footnote " << desc << "\n";
	}
};

struct InText : Node {
	std::string_view id;

	InText(Position position): Node(position, NodeKind::inText) {
	}

	virtual void print() override {
		std::cout << "inText " << id << "\n";
	}
};

struct ReferenceInfo : Node {
	std::string_view key;

	ReferenceInfo(Position position, std::string_view key): Node(position, NodeKind::referenceInfo), key(key) {
	}

	virtual void print() override {
		std::cout << "referenceInfo " << key << "\n";
	}
};

struct Reference : Node {
	std::string_view id;

	Reference(Position position): Node(position, NodeKind::reference) {
	}

	virtual void print() override {
		std::cout << "reference " << id << "\n";
	}
};

struct Emoji : Node {
	std::string_view code;

	Emoji(Position position, std::string_view code): Node(position, NodeKind::emoji), code(code) {
	}

	virtual void print() override {
		std::cout << "emoji " << code << "\n";
	}
};

struct Blockquote : Node {
	Blockquote(Position position): Node(position, NodeKind::blockquote) {
	}

	virtual void print() override {
		std::cout << "blockquote\n";
	}
};

struct DateTime : Node {
	std::string_view date;
	std::string_view time;

	std::string_view content;

	DateTime(Position position, std::string_view content): Node(position, NodeKind::dateTime), content(content) {
		bool hasDate = false;
		bool hasTime = false;

		unsigned int timeBegin = 0;

		for (unsigned int iter = 0; iter < content.size(); iter += 1) {
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

	virtual void print() override {
		std::cout << "dateTime";
		if (date.size() != 0) {
			std::cout << " " << date;
		}
		if (time.size() != 0) {
			std::cout << " " << time;
		}
		std::cout << "\n";
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
		std::cout << "punct ";
		switch (type) {
			case Type::hypen: std::cout << "hyphen\n"; break;
			case Type::enDash: std::cout << "enDash\n"; break;
			case Type::emDash: std::cout << "emDash\n"; break;

			case Type::quoteOpen: std::cout << "quoteOpen\n"; break;
			case Type::quoteClose: std::cout << "quoteClose\n"; break;
			case Type::squoteOpen: std::cout << "squoteOpen\n"; break;
			case Type::squoteClose: std::cout << "squoteClose\n"; break;
		}
	}
};

struct Admon : Node {
	std::string_view label;

	Admon(Position position, std::string_view label): Node(position, NodeKind::admon), label(label) {
	}

	virtual void print() override {
		std::cout << "admon " << label << "\n";
	}
};

struct AccountTag : Node {
	std::string_view resource;

	AccountTag(Position position, std::string_view resource): Node(position, NodeKind::accountTag), resource(resource) {
	}

	virtual void print() override {
		std::cout << "accountTag " << resource << "\n";
	}
};

struct HashTag : Node {
	std::string_view resource;

	HashTag(Position position, std::string_view resource): Node(position, NodeKind::hashTag), resource(resource) {
	}

	virtual void print() override {
		std::cout << "hashTag " << resource << "\n";
	}
};

}
