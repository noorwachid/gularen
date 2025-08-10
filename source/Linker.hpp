#pragma once

#include "Collection/String.hpp"
#include "Collection/Array.hpp"
#include "Parser.hpp"

struct Subsubsection {
	String title;
	String subtitle;
};

struct Subsection {
	String title;
	String subtitle;
	Array<Subsubsection> subsubsections;
};

struct Section {
	String title;
	String subtitle;
	Array<Subsection> subsections;
};

struct Linker {
	Linker(Node*);

	Array<Node*> makeMention(ResourceNode* resource);

	Array<Section> makeOutline();

	Array<Node*> makeBibliography(FuncNode* resource);

	Array<Section> _sections;

	void addFootnote(HierarchyNode* footnote);

	Array<HierarchyNode*> getFootnotes();

	void removeFootnotes();

	int _sectionIndex = -1;
	int _subsectionIndex = -1;

	Table<String, FuncNode*> _bibliographies;

	String toText(Node*);
};
