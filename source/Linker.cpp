#include "Linker.hpp"
#include "BibliographyMaker.hpp"
#include "Parser.hpp"
#include "Stripper.hpp"

String _stripSubtitleOnly(HierarchyNode* hierarchyNode) {
	for (int i = 0; i < hierarchyNode->children.size(); i++) {
		Node* childNode = hierarchyNode->children[i];
		if (childNode->kind == NodeKind_subtitle) {
			return strip(childNode);
		}
	}
	return "";
}

ResourceNode* Linker::_mentionSection(ResourceNode* res, int sectionIndex, int subsectionIndex) {
	ResourceNode* result = new ResourceNode({});
	result->range = res->range;
	result->kind = res->kind;

	ContentNode* label = new ContentNode({});
	label->range = res->range;
	label->kind = NodeKind_text;
	result->children.append(label);

	if (_sectionIndex > -1 && _sectionIndex < _sections.size()) {
		// nearest family search: sibling subsubsection -> parent subsection
		Section const& section = _sections[_sectionIndex];
		if (_subsectionIndex > -1 && _subsectionIndex < section.subsections.size()) {
			Subsection subsection = section.subsections[_subsectionIndex];
			Array<Subsubsection> subsubsections = subsection.subsubsections;
			for (int i = 0; i < subsubsections.size(); i++) {
				Subsubsection subsubsection = subsubsections[i];
				if (subsubsection.title == res->source) {
					res->source = section.title;
					res->source.append(" > ");
					res->source.append(subsection.title);
					res->source.append(" > ");
					res->source.append(subsubsection.title);
					label->content = subsubsection.title;
					goto linking;
				}
			}
		}
		for (int i = 0; i < section.subsections.size(); i++) {
			Subsection subsection = section.subsections[i];
			if (subsection.title == res->source) {
				res->source = section.title;
				res->source.append(" > ");
				res->source.append(subsection.title);
				label->content = subsection.title;
				goto linking;
			}
		}
	}

	for (int i = 0; i < _sections.size(); i++) {
		Section const& section = _sections[i];
		if (section.title == res->source) {
			res->source = section.title;
			label->content = section.title;
			goto linking;
		}
		for (int j = 0; j < section.subsections.size(); j++) {
			Subsection const& subsection = section.subsections[j];
			if (subsection.title == res->source) {
				res->source = section.title;
				res->source.append("-");
				res->source.append(subsection.title);
				label->content = section.title;
				goto linking;
			}
			for (int k = 0; k < subsection.subsubsections.size(); k++) {
				Subsubsection const& subsubsection = subsection.subsubsections[k];
				if (subsubsection.title == res->source) {
					res->source = section.title;
					res->source.append("-");
					res->source.append(subsection.title);
					res->source.append("-");
					res->source.append(subsubsection.title);
					label->content = section.title;
					goto linking;
				}
			}
		}
	}

	linking:
	
	return result;
}

ResourceNode* Linker::_mentionBibliography(ResourceNode* res) {
	ResourceNode* result = new ResourceNode({});
	result->range = res->range;
	result->kind = res->kind;
	FuncNode* func = _bibliographies[res->source];
	result->source = func->arguments.has("id") ? func->arguments["id"] : "";
	result->children = BibliographyMaker::mention(func, res);
	return result;
}

ResourceNode* Linker::mention(ResourceNode* res, int sectionIndex, int subsectionIndex) {
	if (_bibliographies.has(res->source)) {
		return _mentionBibliography(res);
	}
	return _mentionSection(res, sectionIndex, subsectionIndex);
}

Array<Section> Linker::makeOutline() {
	return _sections;
}

Array<Node*> Linker::makeBibliography(FuncNode* func) {
	return BibliographyMaker::make(func);
}

void Linker::collect(Node* node) {
	_collect(node);
}

void Linker::_collectArray(Array<Node*> nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		_collect(nodes[i]);
	}
}

void Linker::_collect(Node* node) {
	switch (node->kind) {
		case NodeKind_document:
		case NodeKind_quote:
		case NodeKind_list:
		case NodeKind_item:
		case NodeKind_numberedlist:
		case NodeKind_numbereditem:
		case NodeKind_checklist:
		case NodeKind_checkitem:
			_collectArray(static_cast<HierarchyNode*>(node)->children);
			break;
		case NodeKind_section: {
			_sectionIndex++;
			_subsectionIndex = -1;
			HierarchyNode* h = static_cast<HierarchyNode*>(node);
			if (h->children.size() != 0 && h->children[0]->kind == NodeKind_title) {
				HierarchyNode* t = static_cast<HierarchyNode*>(h->children[0]);
				Section section;
				section.title = stripTitleOnly(t);
				section.subtitle = _stripSubtitleOnly(t);
				_sections.append(section);
			}
			_collectArray(h->children);
			break;
		}
		case NodeKind_subsection: {
			_subsectionIndex++;
			HierarchyNode* h = static_cast<HierarchyNode*>(node);

			if (h->children.size() != 0 && h->children[0]->kind == NodeKind_title) {
				HierarchyNode* t = static_cast<HierarchyNode*>(h->children[0]);
				if (_sectionIndex > -1) {
					Subsection subsection;
					subsection.title = stripTitleOnly(t);
					subsection.subtitle = _stripSubtitleOnly(t);
					Section& section = const_cast<Section&>(_sections[_sectionIndex]);
					section.subsections.append(subsection);
				}
			}
			_collectArray(h->children);
			break;
		}
		case NodeKind_subsubsection: {
			HierarchyNode* h = static_cast<HierarchyNode*>(node);
			if (h->children.size() != 0 && h->children[0]->kind == NodeKind_title) {
				HierarchyNode* t = static_cast<HierarchyNode*>(h->children[0]);
				if (_subsectionIndex > -1) {
					Subsubsection subsubsection;
					subsubsection.title = stripTitleOnly(t);
					subsubsection.subtitle = _stripSubtitleOnly(t);
					Section& section = const_cast<Section&>(_sections[_sectionIndex]);
					Subsection& subsection = const_cast<Subsection&>(section.subsections[_subsectionIndex]);
					subsection.subsubsections.append(subsubsection);
				}
			}
			_collectArray(h->children);
			break;
		}
		case NodeKind_func: {
			FuncNode* func = static_cast<FuncNode*>(node);
			if (func->symbol == "bibliography") {
				if (func->arguments.has("id")) {
					_bibliographies.set(func->arguments["id"], func);
				}
				break;
			}
			break;
		}
		default: break;
	}
}

