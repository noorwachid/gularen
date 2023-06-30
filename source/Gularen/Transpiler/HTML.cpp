#include <Gularen/Parser.h>

namespace Gularen::Transpiler {
	namespace HTML {
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

		template<class T>
		const T& as(const NodePtr& node) {
			return *static_cast<T*>(node.get());
		}

		std::string tag(bool before, const std::string& name, const std::string& props = "", bool newline = false) {
			return before ? "<" + name + (!props.empty() ? " " + props : "") + ">" : "</" + name + ">" + (!before && newline ? "\n" : "");
		}

		std::string decorate(bool before, const NodePtr& node, size_t depth) {
			switch (node->group) {
				case NodeGroup::text: return before ? escape(as<TextNode>(node).value) : "";
				case NodeGroup::paragraph: return tag(before, "p");
				case NodeGroup::fs: 
					switch (as<FSNode>(node).type) {
						case FSType::bold: return tag(before, "b");
						case FSType::italic: return tag(before, "i");
						case FSType::monospace: return tag(before, "code");
					}
				case NodeGroup::heading: {
					std::string tagName = "";
					const HeadingNode& headingNode = as<HeadingNode>(node).type;
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
				case NodeGroup::break_: return tag(before, "br");
				case NodeGroup::table: return tag(before, "table");
				case NodeGroup::tableRow: return tag(before, "tr");
				case NodeGroup::tableCell: return tag(before, "td");
				case NodeGroup::code: {
					const CodeNode& codeNode = as<CodeNode>(node);
					if (before) {
						return "<pre><code class=\"language-" + codeNode.lang + "\">" + escape(codeNode.source);
					}
				}

				case NodeGroup::list: {
					return tag(before, "ul");
				}
				case NodeGroup::listItem: {
					return tag(before, "li");
				}

				default: return "";
			}
		}

		void visit(std::string& out, const NodePtr& node, size_t depth) {
			out += decorate(true, node, depth);

			for (const NodePtr& childNode : node->children) {
				visit(out, childNode, depth + 1);
			}

			out += decorate(false, node, depth);
		}
		
		std::string transpile(const std::string& content) {
			Parser parser;
			parser.set(content);
			parser.parse();

			NodePtr node = parser.get();

			std::string out;

			if (node) {
				visit(out, node, 0);
			}

			return out;
		}
	}
}

