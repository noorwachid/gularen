#include "CitationGen.hpp"
#include "Parser.hpp"

Array<Node*> cite(ResourceNode* resource, FuncNode* func) {
	Array<Node*> items;
	if (func == nullptr) {
		return items;
	}
	if (func->arguments.has("year") && func->arguments.has("author")) {
		ContentNode* contentNode = new ContentNode();
		contentNode->kind = NodeKind_text;
		contentNode->range = resource->range;
		contentNode->content.append("(");
		contentNode->content.append(func->arguments["author"]);
		contentNode->content.append(", ");
		contentNode->content.append(func->arguments["year"]);
		contentNode->content.append(")");
		items.append(contentNode);
		return items;
	}
	return items;
}

Array<Node*> genReference(FuncNode* func) {
	Array<Node*> items;
	if (func == nullptr) {
		return items;
	}
	if (func->arguments.has("author")) {
		ContentNode* text = new ContentNode();
		text->kind = NodeKind_text;
		text->range = func->range;
		text->content = func->arguments["author"];
		text->content.append(",");
		items.append(text);
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

