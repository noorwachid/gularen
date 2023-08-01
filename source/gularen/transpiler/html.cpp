#include <gularen/parser.h>
#include <gularen/helper/emoji.h>
#include <filesystem>

namespace Gularen::Transpiler::HTML {
	struct Options {
		bool rangeEnabled = false;
	};
	
	class Transpiler {
	public:
		const std::string& transpile(const std::string& input, const std::unordered_map<std::string, std::string>& options) {
			Parser parser;
			parser.set(input);
			parser.parse();

			std::shared_ptr<Node> root = parser.get();
			parseOptions(options);
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
					return addOpenTagWithClassAttrLF(node, "div", "indent");

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
						case ListType::check: return addOpenTagWithClassAttrLF(node, "div", "checklist");
					}
					return;

				case NodeGroup::listItem:
					switch (node->as<ListItemNode>().state) {
						case ListItemState::none: return addOpenTag(node, "li");
						case ListItemState::todo:
							addOpenTag(node, "div");
							addOpenTag(node, "input type=\"checkbox\"");
							return;
						case ListItemState::done:
							addOpenTag(node, "div");
							addOpenTag(node, "input type=\"checkbox\" checked");
							return;
						case ListItemState::canceled:
							addOpenTag(node, "div");
							addOpenTag(node, "input type=\"checkbox\" disabled");
							addOpenTag(node, "del");
							return;
					}
					return;

				case NodeGroup::emoji:
					return add(node, Helper::toEmoji(node->as<EmojiNode>().value));

				default: return;
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
					return addCloseTagLF(node, "div");

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
						case ListItemState::todo: return addCloseTagLF(node, "div");
						case ListItemState::done: return addCloseTagLF(node, "div");
						case ListItemState::canceled:
							addCloseTag(node, "del");
							addCloseTagLF(node, "div");
							return;
					}
					return;

				default: return;
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

		void parseOptions(const std::unordered_map<std::string, std::string>& options) {
			if (options.count("range") && options.at("range") == "true") {
				this->options.rangeEnabled = true;
			}
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
		Options options;
		std::string output;
	};

	std::string transpile(const std::string& content, const std::unordered_map<std::string, std::string>& options) {
		Transpiler transpiler;
		return transpiler.transpile(content, options);
	}
}

