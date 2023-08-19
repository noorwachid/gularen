#include "gularen/helper/emoji.h"
#include "gularen/parser.h"
#include <filesystem>

namespace Gularen::Transpiler::HTML
{
	struct Options
	{
		bool lineSync = false;
	};

	class Transpiler
	{
	public:
		const std::string& Transpile(const std::string& input, const Options& options)
		{
			Parser parser;
			parser.Set(input);
			parser.Parse();

			this->_options = options;
			std::shared_ptr<Node> root = parser.Get();
			Visit(root);
			return _output;
		}

		std::string Slugify(const std::string& from)
		{
			std::string to;

			for (char letter : from)
			{
				if (letter >= '0' && letter <= '9')
				{
					to += letter;
				}
				else if (letter >= 'a' && letter <= 'z')
				{
					to += letter;
				}
				else if (letter >= 'A' && letter <= 'Z')
				{
					to += letter + ' ';
				}
				else if (letter == ' ')
				{
					to += '-';
				}
			}

			return to;
		}

		std::string Escape(const std::string& from)
		{
			std::string to;

			for (char c : from)
			{
				switch (c)
				{
					case '<':
						to += "&lt;";
						break;
					case '>':
						to += "&gt;";
						break;
					default:
						to += c;
				}
			}

			return to;
		}

		void PreVisit(const std::shared_ptr<Node>& node)
		{
			switch (node->group)
			{
				case NodeGroup::Text:
					return AddText(node, node->As<TextNode>().value);

				case NodeGroup::Paragraph:
					return AddOpenTag(node, "p");

				case NodeGroup::FS:
					switch (node->As<FSNode>().type)
					{
						case FSType::Bold:
							return AddOpenTag(node, "b");
						case FSType::Italic:
							return AddOpenTag(node, "i");
						case FSType::Monospace:
							return AddOpenTag(node, "samp");
					}
					return;

				case NodeGroup::Heading: {
					const HeadingNode& headingNode = node->As<HeadingNode>();
					if (headingNode.type == HeadingType::Subtitle)
						return AddOpenTag(node, "small");
					std::string tag;

					switch (headingNode.type)
					{
						case HeadingType::Chapter:
							tag = "h1";
							break;
						case HeadingType::Section:
							tag = "h2";
							break;
						case HeadingType::Subsection:
							tag = "h3";
							break;

						default:
							break;
					}

					std::string id = node->As<HeadingNode>().id;

					if (id.empty())
					{
						id = CollectText(node);
					}

					return AddOpenTag(node, tag + " id=\"" + Slugify(id) + "\"");
				}

				case NodeGroup::Indent:
					return AddOpenTag(node, "blockquote");

				case NodeGroup::Break:
					switch (node->As<BreakNode>().type)
					{
						case BreakType::Line:
							return AddOpenTagLF(node, "br");
						case BreakType::Thematic:
							return AddOpenTagLF(node, "hr");
						case BreakType::Page: {
							AddOpenTagWithClassAttr(node, "div", "pagebreak");
							AddCloseTagLF(node, "div");
							return;
						}
						default:
							break;
					}
					return;

				case NodeGroup::List:
					switch (node->As<ListNode>().type)
					{
						case ListType::Bullet:
							return AddOpenTagLF(node, "ul");
						case ListType::Index:
							return AddOpenTagLF(node, "ol");
						case ListType::Check:
							return AddOpenTagWithClassAttrLF(node, "ul", "checklist");
					}
					return;

				case NodeGroup::ListItem:
					switch (node->As<ListItemNode>().state)
					{
						case ListItemState::None:
							return AddOpenTag(node, "li");
						case ListItemState::Todo:
							AddOpenTag(node, "li");
							AddOpenTag(node, "input type=\"checkbox\"");
							return;
						case ListItemState::Done:
							AddOpenTag(node, "li");
							AddOpenTag(node, "input type=\"checkbox\" checked");
							return;
						case ListItemState::Canceled:
							AddOpenTag(node, "li");
							AddOpenTag(node, "input type=\"checkbox\" disabled");
							AddOpenTag(node, "del");
							return;
					}
					return;

				case NodeGroup::Table:
					_tableNode = static_cast<TableNode*>(node.get());
					_tableRowCount = 0;
					return AddOpenTagLF(node, "table");

				case NodeGroup::TableRow:
					_tableColumnCount = 0;
					return AddOpenTagLF(node, "tr");

				case NodeGroup::TableCell: {
					std::string alignment = "cell-left";
					switch (_tableNode->alignments[_tableColumnCount])
					{
						case Alignment::Left:
							alignment = "cell-left";
							break;
						case Alignment::Center:
							alignment = "cell-center";
							break;
						case Alignment::Right:
							alignment = "cell-right";
							break;
					}
					if (_tableRowCount < _tableNode->header ||
						(_tableNode->footer > _tableNode->header && _tableRowCount >= _tableNode->footer))
					{
						return AddOpenTagWithClassAttr(node, "th", alignment);
					}
					else
					{
						return AddOpenTagWithClassAttr(node, "td", alignment);
					}
				}

				case NodeGroup::Code: {
					const CodeNode& codeNode = node->As<CodeNode>();
					if (codeNode.lang.empty())
					{
						switch (codeNode.type)
						{
							case CodeType::Inline:
								AddOpenTag(node, "code");
								AddText(node, codeNode.source);
								AddCloseTag(node, "code");
								return;
							case CodeType::Block:
								AddOpenTag(node, "pre");
								AddOpenTag(node, "code");
								AddText(node, codeNode.source);
								AddCloseTag(node, "code");
								AddCloseTagLF(node, "pre");
								return;
						}
					}
					if (codeNode.lang.size() > 3 && (codeNode.lang.substr(codeNode.lang.size() - 3, 3) == "-pr"))
					{
						std::string lang = codeNode.lang.substr(0, codeNode.lang.size() - 3);
						switch (codeNode.type)
						{
							case CodeType::Inline:
								AddOpenTagWithClassAttr(node, "span", "language-presenter language-presenter-" + lang);
								AddText(node, codeNode.source);
								AddCloseTag(node, "span");
								return;

							case CodeType::Block:
								AddOpenTagWithClassAttr(node, "div", "language-presenter language-presenter-" + lang);
								AddText(node, codeNode.source);
								AddCloseTag(node, "div");
								return;
						}
					}

					switch (codeNode.type)
					{
						case CodeType::Inline:
							AddOpenTagWithClassAttr(node, "code", "language-" + codeNode.lang);
							AddText(node, codeNode.source);
							AddCloseTag(node, "code");
							return;

						case CodeType::Block:
							AddOpenTag(node, "pre");
							AddOpenTagWithClassAttr(node, "code", "language-" + codeNode.lang);
							AddText(node, codeNode.source);
							AddCloseTag(node, "code");
							AddCloseTagLF(node, "pre");
							return;
					}
					return;
				}

				case NodeGroup::Resource: {
					const ResourceNode& resourceNode = node->As<ResourceNode>();
					std::string value = resourceNode.value;
					if (!resourceNode.id.empty())
					{
						value += "#" + Slugify(resourceNode.id);
					}
					std::string label = resourceNode.label.empty() ? value : resourceNode.label;

					switch (resourceNode.type)
					{
						case ResourceType::Present:
						case ResourceType::PresentLocal: {
							std::string extension = std::filesystem::path(resourceNode.value).extension();
							if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" ||
								extension == ".gif" || extension == ".bmp")
							{
								if (resourceNode.label.empty())
								{
									return AddOpenTag(node, "img src=\"" + value + "\"");
								}
								AddOpenTagLF(node, "figure");
								AddOpenTagLF(node, "img src=\"" + value + "\"");
								AddOpenTag(node, "figcaption");
								AddText(node, resourceNode.label);
								AddCloseTagLF(node, "figcaption");
								AddCloseTagLF(node, "figure");
								return;
							}
						}
						default:
							break;
					}

					AddOpenTag(node, "a href=\"" + value + "\"");
					AddText(node, label);
					AddCloseTag(node, "a");
					return;
				}

				case NodeGroup::FootnoteJump: {
					const FootnoteJumpNode& jumpNode = node->As<FootnoteJumpNode>();
					AddOpenTag(node, "sup");
					AddOpenTag(node, "a href=\"#footnote-" + Slugify(jumpNode.value) + "\"");
					AddText(node, jumpNode.value);
					AddCloseTag(node, "a");
					AddCloseTag(node, "sup");
					return;
				}

				case NodeGroup::FootnoteDescribe: {
					const FootnoteDescribeNode& describeNode = node->As<FootnoteDescribeNode>();
					AddOpenTagWithClassAttr(
						node, "div id=\"footnote-" + Slugify(describeNode.value) + "\"", "footnote"
					);
					AddOpenTag(node, "sup");
					AddText(node, describeNode.value);
					AddCloseTag(node, "sup");
					return;
				}

				case NodeGroup::Admon:
					switch (node->As<AdmonNode>().type)
					{
						case AdmonType::Note:
							return AddOpenTagWithClassAttr(node, "div", "admon-note");
						case AdmonType::Hint:
							return AddOpenTagWithClassAttr(node, "div", "admon-hint");
						case AdmonType::Important:
							return AddOpenTagWithClassAttr(node, "div", "admon-important");
						case AdmonType::Warning:
							return AddOpenTagWithClassAttr(node, "div", "admon-warning");
						case AdmonType::SeeAlso:
							return AddOpenTagWithClassAttr(node, "div", "admon-seealso");
						case AdmonType::Tip:
							return AddOpenTagWithClassAttr(node, "div", "admon-tip");
					}
					return;

				case NodeGroup::DateTime: {
					const DateTimeNode& dateTimeNode = node->As<DateTimeNode>();
					if (dateTimeNode.time.empty())
					{
						_output += "<time datetime=\"" + dateTimeNode.date + "\">" + dateTimeNode.date + "</time>";
						return;
					}
					if (dateTimeNode.date.empty())
					{
						_output += "<time datetime=\"" + dateTimeNode.time + "\">" + dateTimeNode.time + "</time>";
						return;
					}
					_output += "<time datetime=\"" + dateTimeNode.date + " " + dateTimeNode.time + "\">";
					_output += dateTimeNode.date + " " + dateTimeNode.time;
					_output += "</time>";
					return;
				}

				case NodeGroup::BQ:
					return AddOpenTagWithClassAttr(node, "blockquote", "bordered");

				case NodeGroup::Punct:
					switch (node->As<PunctNode>().type)
					{
						case PunctType::Hyphen:
							return Add(node, "&dash;");
						case PunctType::EnDash:
							return Add(node, "&ndash;");
						case PunctType::EmDash:
							return Add(node, "&mdash;");

						case PunctType::LSQuo:
							return Add(node, "&lsquo;");
						case PunctType::RSQuo:
							return Add(node, "&rsquo;");
						case PunctType::LDQuo:
							return Add(node, "&ldquo;");
						case PunctType::RDQuo:
							return Add(node, "&rdquo;");
					}
					return;

				case NodeGroup::Emoji:
					return Add(node, Helper::ShortcodeToEmoji(node->As<EmojiNode>().value));

				default:
					break;
					;
			}
		}

		void Visit(const std::shared_ptr<Node>& node)
		{
			PreVisit(node);
			for (const std::shared_ptr<Node>& subnode : node->children)
			{
				Visit(subnode);
			}
			PostVisit(node);
		}

		void PostVisit(const std::shared_ptr<Node>& node)
		{
			switch (node->group)
			{
				case NodeGroup::Paragraph:
					return AddCloseTagLF(node, "p");

				case NodeGroup::FS:
					switch (node->As<FSNode>().type)
					{
						case FSType::Bold:
							return AddCloseTag(node, "b");
						case FSType::Italic:
							return AddCloseTag(node, "i");
						case FSType::Monospace:
							return AddCloseTag(node, "samp");
					}
					return;

				case NodeGroup::Heading:
					switch (node->As<HeadingNode>().type)
					{
						case HeadingType::Chapter:
							return AddCloseTagLF(node, "h1");
						case HeadingType::Section:
							return AddCloseTagLF(node, "h2");
						case HeadingType::Subsection:
							return AddCloseTagLF(node, "h3");
						case HeadingType::Subtitle:
							return AddCloseTag(node, "small");
					}
					return;

				case NodeGroup::Indent:
					return AddCloseTagLF(node, "blockquote");

				case NodeGroup::List:
					switch (node->As<ListNode>().type)
					{
						case ListType::Bullet:
							return AddCloseTagLF(node, "ul");
						case ListType::Index:
							return AddCloseTagLF(node, "ol");
						case ListType::Check:
							return AddCloseTagLF(node, "ul");
					}
					return;

				case NodeGroup::ListItem:
					switch (node->As<ListItemNode>().state)
					{
						case ListItemState::None:
							return AddCloseTagLF(node, "li");
						case ListItemState::Todo:
							return AddCloseTagLF(node, "li");
						case ListItemState::Done:
							return AddCloseTagLF(node, "li");
						case ListItemState::Canceled:
							AddCloseTag(node, "del");
							AddCloseTagLF(node, "li");
							return;
					}
					return;

				case NodeGroup::Table:
					_tableNode = nullptr;
					return AddCloseTagLF(node, "table");

				case NodeGroup::TableRow:
					++_tableRowCount;
					return AddCloseTagLF(node, "tr");

				case NodeGroup::TableCell:
					++_tableColumnCount;
					return AddCloseTagLF(node, "td");

				case NodeGroup::FootnoteDescribe:
					return AddCloseTagLF(node, "div");

				case NodeGroup::Admon:
					return AddCloseTagLF(node, "div");

				case NodeGroup::BQ:
					return AddCloseTagLF(node, "blockquote");

				default:
					break;
			}
		}

		std::string CollectText(const std::shared_ptr<Node>& node)
		{
			std::string text;

			if (node->group == NodeGroup::Text)
			{
				text += node->As<TextNode>().value;
			}

			for (const std::shared_ptr<Node>& subnode : node->children)
			{
				text += CollectText(subnode);
			}

			return text;
		}

		std::string CreateLS(const std::shared_ptr<Node>& node)
		{
			if (!_options.lineSync)
			{
				return "";
			}

			return " data-line=\"" + std::to_string(node->range.begin.line) + "\"";
		}

		void Add(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += buffer;
		}

		void AddText(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += Escape(buffer);
		}

		void AddOpenTag(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += "<" + buffer + CreateLS(node) + ">";
		}

		void AddOpenTagLF(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += "<" + buffer + CreateLS(node) + ">\n";
		}

		void AddOpenTagWithClassAttr(
			const std::shared_ptr<Node>& node, const std::string& buffer, const std::string& classAttr
		)
		{
			_output += "<" + buffer + " class=\"" + classAttr + "\"" + CreateLS(node) + ">";
		}

		void AddOpenTagWithClassAttrLF(
			const std::shared_ptr<Node>& node, const std::string& buffer, const std::string& classAttr
		)
		{
			_output += "<" + buffer + " class=\"" + classAttr + "\"" + CreateLS(node) + ">\n";
		}

		void AddCloseTag(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += "</" + buffer + ">";
		}

		void AddCloseTagLF(const std::shared_ptr<Node>& node, const std::string& buffer)
		{
			_output += "</" + buffer + ">\n";
		}

	private:
		TableNode* _tableNode = nullptr;
		size_t _tableRowCount = 0;
		size_t _tableColumnCount = 0;
		Options _options;
		std::string _output;
	};

	std::string Transpile(const std::string& content)
	{
		Transpiler transpiler;
		return transpiler.Transpile(content, Options{});
	}

	std::string TranspileLS(const std::string& content)
	{
		Transpiler transpiler;
		Options options;
		options.lineSync = true;
		return transpiler.Transpile(content, options);
	}
}
