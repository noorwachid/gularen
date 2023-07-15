#include <Gularen/Parser.h>
#include <filesystem>

namespace Gularen::Transpiler::HTML {
	// A contributor to refactor this code is very welcome

	struct Context {
		NodePtr tableNode;
		size_t rowCounter;
		size_t cellCounter;
		size_t indent = 0;
	};

	std::string escape(const std::string& from) {
		std::string to;

		for (char c : from) {
			switch (c) {
				case '<': to += "&lt;"; break;
				case '>': to += "&gt;"; break;
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
					case PunctType::hyphen: return "&dash;";
					case PunctType::enDash: return "&ndash;";
					case PunctType::emDash: return "&mdash;";
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
			case NodeGroup::paragraph: return tag(before, "p", "", true);
			case NodeGroup::fs: 
				switch (node->as<FSNode>().type) {
					case FSType::bold: return tag(before, "b");
					case FSType::italic: return tag(before, "i");
					case FSType::monospace: return tag(before, "samp");
				}
			case NodeGroup::heading: {
				std::string tagName = "";
				const HeadingNode& headingNode = node->as<HeadingNode>();
				switch (headingNode.type) {
					case HeadingType::chapter: tagName = "h1"; break;
					case HeadingType::section: tagName = "h2"; break;
					case HeadingType::subsection: tagName = "h3"; break;
					case HeadingType::subtitle: tagName = "small"; break;
				}
				if (headingNode.type == HeadingType::subtitle) {
					if (before) return " <small>";
					return "</small>";
				}
				if (!headingNode.id.empty()) {
					return tag(before, tagName, "id=\"" + headingNode.id + "\"", true);
				}
				return tag(before, tagName, "", true);
			}
			case NodeGroup::indent: return before ? "\n<div class=\"indent\">\n" : "</div>\n";
			case NodeGroup::break_: return before ? "<br>" : "";

			case NodeGroup::footnoteJump: return before ? "<a href=\"#footnote-" + node->as<FootnoteJumpNode>().value + "\">" + node->as<FootnoteJumpNode>().value + "</a>" : "";
			case NodeGroup::footnoteDescribe: return before ? "<dd id=\"footnote-" + node->as<FootnoteDescribeNode>().value + "\">" : "</dd>\n";

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
				if (codeNode.lang.size() > 2 && codeNode.lang.substr(codeNode.lang.size() - 3) == "-pr") {
					if (codeNode.type == CodeType::block) {
						if (before) {
							return "<div class=\"language-presenter " + codeNode.lang + "\">" + escape(codeNode.source);
						}
						return "</div>";
					}

					if (before) {
						return "<span class=\"language-presenter " + codeNode.lang + "\">" + escape(codeNode.source);
					}
					return "</span>";
				}

				if (codeNode.type == CodeType::block) {
					if (before) {
						if (!codeNode.lang.empty()) return "<pre><code class=\"language-" + codeNode.lang + "\">" + escape(codeNode.source);
						return "<pre><code>" + escape(codeNode.source);
					}
					return "</code></pre>\n";
				}

				if (before) {
					if (!codeNode.lang.empty()) return "<code class=\"language-" + codeNode.lang + "\">" + escape(codeNode.source);
					return "<code>" + escape(codeNode.source);
				}
				return "</code>\n";
			}

			case NodeGroup::list: {
				const ListNode& listNode = node->as<ListNode>();
				switch (listNode.type) {
					case ListType::bullet: return before ? "<ul>\n" : "</ul>\n";
					case ListType::index: return before ? "<ol>\n" : "</ol>\n";
					case ListType::check: return before ? "<ul class=\"checklist\">\n" : "</ul>\n";
				}
			}
			case NodeGroup::listItem: {
				const ListItemNode& listItemNode = node->as<ListItemNode>();
				switch (listItemNode.state) {
					case ListItemState::none: return tag(before, "li", "", true);
					case ListItemState::todo: {
						if (before)
							return "<li><input type=\"checkbox\"> ";
						return "</li>\n";
					}
					case ListItemState::done: {
						if (before)
							return "<li><input type=\"checkbox\" checked> ";
						return "</li>\n";
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

		if (node->group == NodeGroup::indent) {
			++context.indent;
		}

		out += decorate(true, context, node, depth);

		for (const NodePtr& childNode : node->children) {
			visit(out, context, childNode, depth + 1);
		}

		out += decorate(false, context, node, depth);

		if (node->group == NodeGroup::indent) {
			--context.indent;
		}

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

