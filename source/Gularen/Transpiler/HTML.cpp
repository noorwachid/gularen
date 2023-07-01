#include <Gularen/Parser.h>
#include <filesystem>

namespace Gularen::Transpiler::HTML {
	// Contributor to refactor this code are very welcome

	struct Context {
		NodePtr tableNode;
		size_t rowCounter;
		size_t cellCounter;
	};

	std::string escape(const std::string& from) {
		std::string to;

		for (char c : from) {
			switch (c) {
				case '<': to += "&lt;";
				case '>': to += "&gt;";
				default: to += c;
			}
		}

		return to;
	}

	std::string tag(bool before, const std::string& name, const std::string& props = "", bool newline = false) {
		return before ? "<" + name + (!props.empty() ? " " + props : "") + ">" : "</" + name + ">" + (!before && newline ? "\n" : "");
	}

	std::string decorate(bool before, Context& context, const NodePtr& node, size_t depth) {
		switch (node->group) {
			case NodeGroup::text: return before ? escape(node->as<TextNode>().value) : "";
			case NodeGroup::punct: {
				if (!before) return "";
				switch (node->as<PunctNode>().type) {
					case PunctType::ldQuo: return "&ldquo;";
					case PunctType::rdQuo: return "&rdquo;";
					case PunctType::lsQuo: return "&lsquo;";
					case PunctType::rsQuo: return "&rsquo;";
					case PunctType::minus: return "&minus;";
					case PunctType::hyphen: return "&dash;";
					case PunctType::enDash: return "&ndash;";
				}
			}
			case NodeGroup::resource: {
				if (!before) return "";

				const ResourceNode& resourceNode = node->as<ResourceNode>();
				switch (resourceNode.type) {
					case ResourceType::linker:
					case ResourceType::linkerLocal:
						if (resourceNode.label.empty()) {
							return "<a href=\"" + resourceNode.value + "\">" + resourceNode.value + "</a>";
						}
						return "<a href=\"" + resourceNode.value + "\">" + escape(resourceNode.label) + "</a>";
					case ResourceType::presenter:
					case ResourceType::presenterLocal: {
						std::string ext = std::filesystem::path(resourceNode.value).extension();
						if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif" || ext == ".webp") {
							if (resourceNode.label.empty()) {
								return "<img src=\"" + resourceNode.value + "\">";
							}
							return "<figure><img src=\"" + resourceNode.value + "\"><figcaption>" + escape(resourceNode.label) + "</figcaption></figure>";
						}

						if (resourceNode.label.empty()) {
							return "<a href=\"" + resourceNode.value + "\">" + resourceNode.value + "</a>";
						}
						return "<a href=\"" + resourceNode.value + "\">" + escape(resourceNode.label) + "</a>";
					}
				}
			}
			case NodeGroup::paragraph: return tag(before, "p");
			case NodeGroup::fs: 
				switch (node->as<FSNode>().type) {
					case FSType::bold: return tag(before, "b");
					case FSType::italic: return tag(before, "i");
					case FSType::monospace: return tag(before, "code");
				}
			case NodeGroup::heading: {
				std::string tagName = "";
				const HeadingNode& headingNode = node->as<HeadingNode>().type;
				switch (headingNode.type) {
					case HeadingType::chapter: tagName = "h1"; break;
					case HeadingType::section: tagName = "h2"; break;
					case HeadingType::subsection: tagName = "h3"; break;
					case HeadingType::subtitle: tagName = "span"; break;
				}
				if (!headingNode.id.empty()) {
					return tag(before, tagName, "id=\"" + headingNode.id + "\"", true);
				}
				return tag(before, tagName, "", true);
			}
			case NodeGroup::indent: return tag(before, "div", "class=\"indent\"");
			case NodeGroup::break_: return before ? "<br>" : "";

			case NodeGroup::table: return before ? "<table>\n" : "</table>\n";
			case NodeGroup::tableRow: return before ? "<tr>\n" : "</tr>\n";
			case NodeGroup::tableCell: {
				if (context.tableNode) {
					const TableNode& tableNode = context.tableNode->as<TableNode>();
					if (context.rowCounter < tableNode.header || (tableNode.header != tableNode.footer && context.rowCounter >= tableNode.footer)) {
						return tag(before, "th", "", true);
					}
					if (context.cellCounter < tableNode.alignments.size()) {
						if (before) {
							switch (tableNode.alignments[context.cellCounter]) {
								case Alignment::left: return "<td class=\"align-left\">";
								case Alignment::center: return "<td class=\"align-center\">";
								case Alignment::right: return "<td class=\"align-right\">";
							}
						}
						return "</td>\n";
					}
					return tag(before, "td", "", true);
				}
				return tag(before, "td", "", true);
			}

			case NodeGroup::code: {
				const CodeNode& codeNode = node->as<CodeNode>();
				if (codeNode.lang == "mermaid" || codeNode.lang == "katex") {
					if (before) {
						// WARN: validate the katex and mermaid symbols for html tags
						return "<div class=\"language-presenter " + codeNode.lang + "\">" + codeNode.source;
					}
					return "</div>";
				}

				if (before) {
					return "<pre><code class=\"language-" + codeNode.lang + "\">" + escape(codeNode.source);
				}
				return "</code></pre>\n";
			}

			case NodeGroup::list: {
				const ListNode& listNode = node->as<ListNode>();
				switch (listNode.type) {
					case ListType::bullet: return tag(before, "ul", "", true);
					case ListType::index: return tag(before, "ol", "", true);
					case ListType::check: return tag(before, "ul", "class=\"checklist\"", true);
				}
			}
			case NodeGroup::listItem: {
				const ListItemNode& listItemNode = node->as<ListItemNode>();
				switch (listItemNode.state) {
					case ListItemState::none: return tag(before, "li");
					case ListItemState::todo: {
						if (before)
							return "<li><input type=\"checkbox\"> ";
						return "</li>";
					}
					case ListItemState::done: {
						if (before)
							return "<li><input type=\"checkbox\" checked> ";
						return "</li>";
					}
					case ListItemState::canceled: {
						if (before)
							return "<li><input type=\"checkbox\" disabled> <del>";
						return "</del></li>\n";
					}
				}
			}

			default: return "";
		}
	}

	void visit(std::string& out, Context& context, const NodePtr& node, size_t depth) {
		if (node->group == NodeGroup::table) {
			context.tableNode = node;
			context.rowCounter = 0;
		}

		if (node->group == NodeGroup::tableRow) {
			context.cellCounter = 0;
		}

		out += decorate(true, context, node, depth);

		for (const NodePtr& childNode : node->children) {
			visit(out, context, childNode, depth + 1);
		}

		out += decorate(false, context, node, depth);

		if (node->group == NodeGroup::tableRow) {
			++context.rowCounter;
		}

		if (node->group == NodeGroup::tableCell) {
			++context.cellCounter;
		}

		if (node->group == NodeGroup::table) {
			context.tableNode = nullptr;
		}
	}
	
	std::string transpile(const std::string& content) {
		Parser parser;
		parser.set(content);
		parser.parse();

		NodePtr node = parser.get();
		Context context;

		std::string out;

		if (node) {
			visit(out, context, node, 0);
		}

		return out;
	}
}

