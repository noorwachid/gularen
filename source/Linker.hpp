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
	void collect(Node* node);

	ResourceNode* mention(ResourceNode* resource, int sectionIndex = -1, int subsectionIndex = -1);

	Array<Section> makeOutline();

	Array<Node*> makeBibliography(FuncNode* resource);

	void _collect(Node* node);

	void _collectArray(Array<Node*> nodes);

	ResourceNode* _mentionBibliography(ResourceNode* resource);

	ResourceNode* _mentionSection(ResourceNode* resource, int sectionIndex, int subsectionIndex);

	int _sectionIndex = -1;
	int _subsectionIndex = -1;

	Array<Section> _sections;

	Table<String, FuncNode*> _bibliographies;
};
