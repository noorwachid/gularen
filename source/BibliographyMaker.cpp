#include "BibliographyMaker.hpp"
#include "Parser.hpp"

namespace BibliographyMaker {

	Array<String> _splitNames(String const& name) {
		Array<String> parts;
		int i = 0;
		int start = 0;
		while (i < name.size()) {
			if (name[i] == ',') {
				int end = i;
				i++;
				while (i < name.size() && name[i] == ' ') {
					i++;
				}
				parts.append(name.slice(start, end - start));
				start = i;
				continue;
			}
			i++;
		}
		parts.append(name.slice(start, i - start));
		return parts;
	}

	Array<String> _parseName(String const& name) {
		Array<String> parts;
		int i = 0;
		int start = 0;
		while (i < name.size()) {
			if (name[i] == ' ' && (name[start] >= 'A' && name[start] <= 'Z')) {
				int end = i;
				while (i < name.size() && name[i] == ' ') {
					i++;
				}
				parts.append(name.slice(start, end - start));
				start = i;
				continue;
			}
			i++;
		}
		parts.append(name.slice(start, i - start));
		return parts;
	}

	Node* mentionAuthors(FuncNode* func, Array<String> const& names) {
		ContentNode* text = new ContentNode();
		text->kind = NodeKind_text;
		text->range = func->range;
		if (names.size() == 1) {
			Array<String> parts = _parseName(names[0]);
			text->content.append(parts[parts.size() - 1]);
		} else if (names.size() == 2) {
			Array<String> parts0 = _parseName(names[0]);
			text->content.append(parts0[parts0.size() - 1]);
			text->content.append(" & ");
			Array<String> parts1 = _parseName(names[1]);
			text->content.append(parts1[parts1.size() - 1]);
		} else {
			Array<String> parts = _parseName(names[0]);
			text->content.append(parts[parts.size() - 1]);
			text->content.append(" et al.");
		}
		return text;
	}

	Array<Node*> mention(FuncNode* func, ResourceNode* resource) {
		Array<Node*> items;
		ContentNode* opening = new ContentNode();
		opening->kind = NodeKind_text;
		opening->range = func->range;
		opening->content = "(";
		items.append(opening);
		if (func->arguments.has("author")) {
			Array<String> names;
			names.append(func->arguments["author"]);
			items.append(mentionAuthors(func, names));
		}
		if (func->arguments.has("authors")) {
			Array<String> names = _splitNames(func->arguments["authors"]);
			items.append(mentionAuthors(func, names));
		}
		if (func->arguments.has("year")) {
			ContentNode* text = new ContentNode();
			text->kind = NodeKind_text;
			text->range = func->range;
			text->content.append(", ");
			text->content.append(func->arguments["year"]);
			items.append(text);
		}
		ContentNode* closing = new ContentNode();
		closing->kind = NodeKind_text;
		closing->range = func->range;
		closing->content = ")";
		items.append(closing);
		return items;
	}

	Node* makeAuthors(FuncNode* func, Array<String> const& names) {
		ContentNode* text = new ContentNode();
		text->kind = NodeKind_text;
		text->range = func->range;
		for (int i = 0; i < names.size(); i++) {
			if (i != 0) {
				text->content.append(", ");
				if (i == names.size() - 1) {
					text->content.append("& ");
				}
			}
			Array<String> parts = _parseName(names[i]);
			if (parts.size() > 1) {
				text->content.append(parts[parts.size() - 1]);
				text->content.append(", ");
				for (int j = 0; j < parts.size() - 1; j++) {
					text->content.append(1, parts[j].items());
					text->content.append(".");
				}
			} else {
				text->content.append(parts[parts.size() - 1]);
				text->content.append(".");
			}
		}
		return text;
	}

	Array<Node*> make(FuncNode* func) {
		Array<Node*> items;
		if (func->arguments.has("author")) {
			Array<String> names;
			names.append(func->arguments["author"]);
			items.append(makeAuthors(func, names));
		}
		if (func->arguments.has("authors")) {
			Array<String> names = _splitNames(func->arguments["authors"]);
			items.append(makeAuthors(func, names));
		}
		if (func->arguments.has("year")) {
			ContentNode* text = new ContentNode();
			text->kind = NodeKind_text;
			text->range = func->range;
			text->content.append(" (");
			text->content.append(func->arguments["year"]);
			text->content.append(").");
			items.append(text);
		}
		if (func->arguments.has("title")) {
			ContentNode* spaceText = new ContentNode();
			spaceText->kind = NodeKind_text;
			spaceText->range = func->range;
			spaceText->content.append(" ");

			ContentNode* titleText = new ContentNode();
			titleText->kind = NodeKind_text;
			titleText->range = func->range;
			titleText->content.append(func->arguments["title"]);
			titleText->content.append(".");

			HierarchyNode* emphasis = new HierarchyNode();
			emphasis->kind = NodeKind_emphasis;
			emphasis->range = func->range;
			emphasis->children.append(titleText);

			items.append(spaceText);
			items.append(emphasis);
		}
		if (func->arguments.has("publisher")) {
			ContentNode* text = new ContentNode();
			text->kind = NodeKind_text;
			text->range = func->range;
			text->content.append(" ");
			text->content.append(func->arguments["publisher"]);
			text->content.append(".");
			items.append(text);
		}
		return items;
	}
}
