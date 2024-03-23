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
	admon,

	accountTag,
	hashTag,
};

struct Annotation {
	std::string_view key;
	std::string_view value;
};

struct Node {
	Range range;
	NodeKind kind;
	std::vector<Node*> children;
	std::vector<Annotation> annotations;

	Node(Range range, NodeKind kind): range(range), kind(kind)  {
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

	Document(Range range, std::string_view path): Node(range, NodeKind::document), path(path) {
	}

	virtual void print() override {
		std::cout << "document " << path << "\n";
	}
};


struct Comment : Node {
	std::string_view content;

	Comment(Range range, std::string_view content): Node(range, NodeKind::comment), content(content) {
	}

	virtual void print() override {
		std::cout << "comment " << content << "\n";
	}
};

struct Text : Node {
	std::string_view content;

	Text(Range range, std::string_view content): Node(range, NodeKind::text), content(content) {
	}

	virtual void print() override {
		std::cout << "text " << content << "\n";
	}
};

struct Space : Node {
	Space(Range range): Node(range, NodeKind::space) {
	}

	virtual void print() override {
		std::cout << "space\n";
	}
};

struct LineBreak : Node {
	LineBreak(Range range): Node(range, NodeKind::lineBreak) {
	}

	virtual void print() override {
		std::cout << "lineBreak\n";
	}
};

struct PageBreak : Node {
	PageBreak(Range range): Node(range, NodeKind::pageBreak) {
	}

	virtual void print() override {
		std::cout << "pageBreak\n";
	}
};

struct Dinkus : Node {
	Dinkus(Range range): Node(range, NodeKind::dinkus) {
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

	Style(Range range, Type type): Node(range, NodeKind::style), type(type) {
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
	Highlight(Range range): Node(range, NodeKind::highlight) {
	}

	virtual void print() override {
		std::cout << "highlight\n";
	}
};

struct Paragraph : Node {
	Paragraph(Range range): Node(range, NodeKind::paragraph) {
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

	Heading(Range range): Node(range, NodeKind::heading) {
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
	Subtitle(Range range): Node(range, NodeKind::subtitle) {
	}

	virtual void print() override {
		std::cout << "subtitle\n";
	}
};

struct Quote : Node {
	Quote(Range range): Node(range, NodeKind::quote) {
	}

	virtual void print() override {
		std::cout << "indent\n";
	}
};

struct List : Node {
	List(Range range, NodeKind kind): Node(range, kind) {
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
	Item(Range range): Node(range, NodeKind::item) {
	}

	virtual void print() override {
		std::cout << "item\n";
	}
};

struct CheckItem : Node {
	bool checked;

	CheckItem(Range range): Node(range, NodeKind::checkItem) {
	}

	virtual void print() override {
		std::cout << "checkItem";
		if (checked) {
			std::cout << " checked";
		}
		std::cout << "\n";
	}
};

struct DefinitionItem : Node {
	DefinitionItem(Range range): Node(range, NodeKind::definitionItem) {
	}

	virtual void print() override {
		std::cout << "definitionItem\n";
	}
};

struct DefinitionTerm : Node {
	DefinitionTerm(Range range): Node(range, NodeKind::definitionTerm) {
	}

	virtual void print() override {
		std::cout << "definitionTerm\n";
	}
};

struct DefinitionDesc : Node {
	DefinitionDesc(Range range): Node(range, NodeKind::definitionDesc) {
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

	Table(Range range): Node(range, NodeKind::table) {
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

	Row(Range range): Node(range, NodeKind::row) {
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
	Cell(Range range): Node(range, NodeKind::cell) {
	}

	virtual void print() override {
		std::cout << "cell\n";
	}
};

struct Code : Node {
	std::string_view label;
	std::string_view content;

	Code(Range range): Node(range, NodeKind::code) {
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
	CodeBlock(Range range): Code(range) {
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

	Link(Range range): Node(range, NodeKind::link) {
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

	View(Range range): Node(range, NodeKind::view) {
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

	Footnote(Range range, std::string_view description): Node(range, NodeKind::footnote), desc(description) {
	}

	virtual void print() override {
		std::cout << "footnote " << desc << "\n";
	}
};

struct InText : Node {
	std::string_view id;

	InText(Range range): Node(range, NodeKind::inText) {
	}

	virtual void print() override {
		std::cout << "inText " << id << "\n";
	}
};

struct ReferenceInfo : Node {
	std::string_view key;

	ReferenceInfo(Range range, std::string_view key): Node(range, NodeKind::referenceInfo), key(key) {
	}

	virtual void print() override {
		std::cout << "referenceInfo " << key << "\n";
	}
};

struct Reference : Node {
	std::string_view id;

	Reference(Range range): Node(range, NodeKind::reference) {
	}

	virtual void print() override {
		std::cout << "reference " << id << "\n";
	}
};

struct Emoji : Node {
	std::string_view code;

	Emoji(Range range, std::string_view code): Node(range, NodeKind::emoji), code(code) {
	}

	virtual void print() override {
		std::cout << "emoji " << code << "\n";
	}
};

struct DateTime : Node {
	std::string_view date;
	std::string_view time;

	std::string_view content;

	DateTime(Range range, std::string_view content): Node(range, NodeKind::dateTime), content(content) {
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

	Punct(Range range, Type type): Node(range, NodeKind::punct), type(type) {
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

	Admon(Range range, std::string_view label): Node(range, NodeKind::admon), label(label) {
	}

	virtual void print() override {
		std::cout << "admon " << label << "\n";
	}
};

struct AccountTag : Node {
	std::string_view resource;

	AccountTag(Range range, std::string_view resource): Node(range, NodeKind::accountTag), resource(resource) {
	}

	virtual void print() override {
		std::cout << "accountTag " << resource << "\n";
	}
};

struct HashTag : Node {
	std::string_view resource;

	HashTag(Range range, std::string_view resource): Node(range, NodeKind::hashTag), resource(resource) {
	}

	virtual void print() override {
		std::cout << "hashTag " << resource << "\n";
	}
};

}
