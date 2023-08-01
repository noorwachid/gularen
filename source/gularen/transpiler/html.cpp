#include <gularen/parser.h>
#include <gularen/helper/emoji.h>
#include <filesystem>

namespace Gularen::Transpiler::HTML {
	struct Options {
		bool rangeEnabled = false;
	};
	
	class Transpiler {
	public:
		const std::string& transpile(const std::string& input, const Options& options) {
			Parser parser;
			parser.set(input);
			parser.parse();

			this->options = options;
			std::shared_ptr<Node> root = parser.get();
			visit(root);
			return output;
		}

	private:
		std::string slugify(const std::string& from) {
			std::string to;

			for (char letter : from) {
				if (letter >= '0' && letter <= '9') {
					to += letter;
				} else if (letter >= 'a' && letter <= 'z') {
					to += letter;
				} else if (letter >= 'A' && letter <= 'Z') {
					to += letter + ' ';
				} else if (letter == ' ') {
					to += '-';
				}
			}

			return to;
		}

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

		void preVisit(const std::shared_ptr<Node>& node) {
			switch (node->group) {
				case NodeGroup::text:
					return addText(node, node->as<TextNode>().value);

				case NodeGroup::paragraph:
					return addOpenTag(node, "p");

				case NodeGroup::fs:
					switch (node->as<FSNode>().type) {
						case FSType::bold: return addOpenTag(node, "b");
						case FSType::italic: return addOpenTag(node, "i");
						case FSType::monospace: return addOpenTag(node, "samp");
					}
					return;

				case NodeGroup::heading: {
					const HeadingNode& headingNode = node->as<HeadingNode>();
					if (headingNode.type == HeadingType::subtitle) return addOpenTag(node, "small");
					std::string tag;

					switch (headingNode.type) {
						case HeadingType::chapter:
							tag = "h1";
							break;
						case HeadingType::section:
							tag = "h2";
							break;
						case HeadingType::subsection:
							tag = "h3";
							break;

						default: break;
					}

					std::string id = node->as<HeadingNode>().id;

					if (id.empty()) {
						id = collectText(node);
					}

					return addOpenTag(node, tag + " id=\"" + slugify(id) + "\"");
				}

				case NodeGroup::indent:
					return addOpenTag(node, "blockquote");

				case NodeGroup::break_:
					switch (node->as<BreakNode>().type) {
						case BreakType::line: return addOpenTagLF(node, "br");
						case BreakType::thematic: return addOpenTagLF(node, "hr");
						case BreakType::page: { addOpenTagWithClassAttr(node, "div", "pagebreak"); addCloseTagLF(node, "div"); return; }
						default: break;
					}
					return;

				case NodeGroup::list:
					switch (node->as<ListNode>().type) {
						case ListType::bullet: return addOpenTagLF(node, "ul");
						case ListType::index: return addOpenTagLF(node, "ol");
						case ListType::check: return addOpenTagWithClassAttrLF(node, "ul", "checklist");
					}
					return;

				case NodeGroup::listItem:
					switch (node->as<ListItemNode>().state) {
						case ListItemState::none: return addOpenTag(node, "li");
						case ListItemState::todo:
							addOpenTag(node, "li");
							addOpenTag(node, "input type=\"checkbox\"");
							return;
						case ListItemState::done:
							addOpenTag(node, "li");
							addOpenTag(node, "input type=\"checkbox\" checked");
							return;
						case ListItemState::canceled:
							addOpenTag(node, "li");
							addOpenTag(node, "input type=\"checkbox\" disabled");
							addOpenTag(node, "del");
							return;
					}
					return;

				case NodeGroup::table:
					tableNode = static_cast<TableNode*>(node.get());
					tableRowCount = 0;
					return addOpenTagLF(node, "table");

				case NodeGroup::tableRow:
					tableColumnCount = 0;
					return addOpenTagLF(node, "tr");

				case NodeGroup::tableCell: {
					std::string alignment = "cell-left";
					switch (tableNode->alignments[tableColumnCount]) {
						case Alignment::left: alignment = "cell-left"; break;
						case Alignment::center: alignment = "cell-center"; break;
						case Alignment::right: alignment = "cell-right"; break;
					}
					if (tableRowCount < tableNode->header || (tableNode->footer > tableNode->header && tableRowCount >= tableNode->footer)) {
						return addOpenTagWithClassAttr(node, "th", alignment);
					} else {
						return addOpenTagWithClassAttr(node, "td", alignment);
					}
				}

				case NodeGroup::code: {
					const CodeNode& codeNode = node->as<CodeNode>();
					if (codeNode.lang.empty()) {
						switch (codeNode.type) {
							case CodeType::inline_: 
								addOpenTag(node, "code");
								addText(node, codeNode.source);
								return;
							case CodeType::block: 
								addOpenTag(node, "pre");
								addOpenTag(node, "code");
								addText(node, codeNode.source);
								return;
						}
					}
					switch (codeNode.type) {
						case CodeType::inline_:
							addOpenTagWithClassAttr(node, "code", "language-" + codeNode.lang);
							addText(node, codeNode.source);
							return;

						case CodeType::block: 
							addOpenTag(node, "pre");
							addOpenTagWithClassAttr(node, "code", "language-" + codeNode.lang);
							addText(node, codeNode.source);
							return;
					}
					return;
				}

				case NodeGroup::resource: {
					const ResourceNode& resourceNode = node->as<ResourceNode>();
					std::string value = resourceNode.value;
					if (!resourceNode.id.empty()) {
						value += "#" + slugify(resourceNode.id);
					}
					std::string label = resourceNode.label.empty() ? value : resourceNode.label;

					switch (resourceNode.type) {
						case ResourceType::presenter:
						case ResourceType::presenterLocal:
							return;
					}

					addOpenTag(node, "a href=\"" + value + "\"");
					addText(node, label);
					addCloseTag(node, "a");
					return;
				}

				case NodeGroup::footnoteJump: {
					const FootnoteJumpNode& jumpNode = node->as<FootnoteJumpNode>();
					addOpenTag(node, "sup");
					addOpenTag(node, "a href=\"#footnote-" + slugify(jumpNode.value) + "\"");
					addText(node, jumpNode.value);
					addCloseTag(node, "a");
					addCloseTag(node, "sup");
					return;
				}

				case NodeGroup::footnoteDescribe: {
					const FootnoteDescribeNode& describeNode = node->as<FootnoteDescribeNode>();
					addOpenTagWithClassAttr(node, "div id=\"footnote-" + slugify(describeNode.value) + "\"", "footnote");
					addOpenTag(node, "sup");
					addText(node, describeNode.value);
					addCloseTag(node, "sup");
					return;
				}

				case NodeGroup::admon:
					switch (node->as<AdmonNode>().type) {
						case AdmonType::note: return addOpenTagWithClassAttr(node, "div", "admon-note");
						case AdmonType::hint: return addOpenTagWithClassAttr(node, "div", "admon-hint");
						case AdmonType::important: return addOpenTagWithClassAttr(node, "div", "admon-important");
						case AdmonType::warning: return addOpenTagWithClassAttr(node, "div", "admon-warning");
						case AdmonType::seeAlso: return addOpenTagWithClassAttr(node, "div", "admon-seealso");
						case AdmonType::tip: return addOpenTagWithClassAttr(node, "div", "admon-tip");
					}
					return;

				case NodeGroup::bq:
					return addOpenTagWithClassAttr(node, "blockquote", "bordered");

				case NodeGroup::punct:
					switch (node->as<PunctNode>().type) {
						case PunctType::hyphen: return add(node, "&dash;");
						case PunctType::enDash: return add(node, "&ndash;");
						case PunctType::emDash: return add(node, "&mdash;");

						case PunctType::lsQuo: return add(node, "&lsquo;");
						case PunctType::rsQuo: return add(node, "&rsquo;");
						case PunctType::ldQuo: return add(node, "&ldquo;");
						case PunctType::rdQuo: return add(node, "&rdquo;");
					}
					return;

				case NodeGroup::emoji:
					return add(node, Helper::toEmoji(node->as<EmojiNode>().value));

				default: break;;
			}
		}

		void visit(const std::shared_ptr<Node>& node) {
			preVisit(node);
			for (const std::shared_ptr<Node>& subnode : node->children) {
				visit(subnode);
			}
			postVisit(node);
		}

		void postVisit(const std::shared_ptr<Node>& node) {
			switch (node->group) {
				case NodeGroup::paragraph:
					return addCloseTagLF(node, "p");

				case NodeGroup::fs:
					switch (node->as<FSNode>().type) {
						case FSType::bold: return addCloseTag(node, "b");
						case FSType::italic: return addCloseTag(node, "i");
						case FSType::monospace: return addCloseTag(node, "samp");
					}
					return;

				case NodeGroup::heading:
					switch (node->as<HeadingNode>().type) {
						case HeadingType::chapter: return addCloseTagLF(node, "h1");
						case HeadingType::section: return addCloseTagLF(node, "h2");
						case HeadingType::subsection: return addCloseTagLF(node, "h3");
						case HeadingType::subtitle: return addCloseTag(node, "small");
					}
					return;

				case NodeGroup::indent:
					return addCloseTagLF(node, "blockquote");

				case NodeGroup::list:
					switch (node->as<ListNode>().type) {
						case ListType::bullet: return addCloseTagLF(node, "ul");
						case ListType::index: return addCloseTagLF(node, "ol");
						case ListType::check: return addCloseTagLF(node, "div");
					}
					return;

				case NodeGroup::listItem:
					switch (node->as<ListItemNode>().state) {
						case ListItemState::none: return addCloseTagLF(node, "li");
						case ListItemState::todo: return addCloseTagLF(node, "li");
						case ListItemState::done: return addCloseTagLF(node, "li");
						case ListItemState::canceled:
							addCloseTag(node, "del");
							addCloseTagLF(node, "li");
							return;
					}
					return;

				case NodeGroup::table:
					tableNode = nullptr;
					return addCloseTagLF(node, "table");

				case NodeGroup::tableRow:
					++tableRowCount;
					return addCloseTagLF(node, "tr");

				case NodeGroup::tableCell:
					++tableColumnCount;
					return addCloseTagLF(node, "td");

				case NodeGroup::code: {
					switch (node->as<CodeNode>().type) {
						case CodeType::inline_: return addCloseTag(node, "code");
						case CodeType::block: 
							addCloseTag(node, "code");
							addCloseTagLF(node, "pre");
							return;
					}
				}

				case NodeGroup::footnoteDescribe:
					return addCloseTagLF(node, "div");

				case NodeGroup::admon:
					return addCloseTagLF(node, "div");

				case NodeGroup::bq:
					return addCloseTagLF(node, "blockquote");

				default: break;
			}
		}

		std::string collectText(const std::shared_ptr<Node>& node) {
			std::string text;

			if (node->group == NodeGroup::text) {
				text += node->as<TextNode>().value;
			}

			for (const std::shared_ptr<Node>& subnode : node->children) {
				text += collectText(subnode);
			}

			return text;
		}

		void add(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += buffer;
		}

		void addText(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += escape(buffer);
		}

		void addOpenTag(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += "<" + buffer + ">";
		}

		void addOpenTagLF(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += "<" + buffer + ">\n";
		}

		void addOpenTagWithClassAttr(const std::shared_ptr<Node>& node, const std::string& buffer, const std::string& classAttr) {
			output += "<" + buffer + " class=\"" + classAttr + "\">";
		}

		void addOpenTagWithClassAttrLF(const std::shared_ptr<Node>& node, const std::string& buffer, const std::string& classAttr) {
			output += "<" + buffer + " class=\"" + classAttr + "\">\n";
		}

		void addCloseTag(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += "</" + buffer + ">";
		}

		void addCloseTagLF(const std::shared_ptr<Node>& node, const std::string& buffer) {
			output += "</" + buffer + ">\n";
		}

	private:
		TableNode* tableNode = nullptr;
		size_t tableRowCount = 0;
		size_t tableColumnCount = 0;
		Options options;
		std::string output;
	};

	std::string transpile(const std::string& content) {
		Transpiler transpiler;
		return transpiler.transpile(content, Options{});
	}

	std::string transpileWithRange(const std::string& content) {
		Transpiler transpiler;
		Options options;
		options.rangeEnabled = true;
		return transpiler.transpile(content, options);
	}
}

