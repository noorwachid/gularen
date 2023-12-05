#include "Gularen/Parser.h"
#include <fstream>
#include <iostream>

namespace Gularen
{
	bool HasNetProtocol(const std::string& reference)
	{
		if (reference.empty())
			return false;

		if (reference[0] >= 'a' && reference[0] <= 'z')
		{
			size_t i = 0;
			size_t size = 0;
			while (i < reference.size() && reference[i] >= 'a' && reference[i] <= 'z')
			{
				++i;
				++size;
			}

			if (i + 2 < reference.size() && reference[i] == ':' && reference[i + 1] == '/' && reference[i + 2] == '/')
			{
				return true;
			}

			return false;
		}

		return false;
	}

	bool Parser::CheckBoundary(size_t offset) const
	{
		return _index + offset < _lexer.GetTokens().size();
	}

	bool Parser::IsToken(size_t offset, TokenType type) const
	{
		return _lexer.GetTokens()[_index + offset].type == type;
	}

	const Token& Parser::GetToken(size_t offset) const
	{
		return _lexer.GetTokens()[_index + offset];
	}

	void Parser::Advance(size_t offset)
	{
		_index += 1 + offset;
	}

	void Parser::Add(const NodePtr& node, const Range& range)
	{
		node->range = range;
		_scopes.top()->children.push_back(node);
	}

	void Parser::AddText(const std::string& value, const Range& range)
	{
		if (!_scopes.top()->children.empty() && _scopes.top()->children.back()->group == NodeGroup::Text)
		{
			Node* node = _scopes.top()->children.back().get();
			static_cast<TextNode*>(node)->value += value;
		}
		else
		{
			Add(std::make_shared<TextNode>(value), range);
		}
	}

	void Parser::AddScope(const NodePtr& node, const Range& range)
	{
		Add(node, range);
		_scopes.push(node);
	}

	void Parser::RemoveScope(const Range& range)
	{
		if (_scopes.size() > 1)
			_scopes.pop();
	}

	const NodePtr& Parser::GetScope()
	{
		return _scopes.top();
	}

	void Parser::Set(const std::string& content, const std::string& path)
	{
		_lexer.Set(content);
		this->_pathVirtual = true;
		this->_path = path;
		this->_previousPaths.clear();
		this->_previousPaths.push_back(path);
	}

	void Parser::Load(const std::string& path)
	{
		this->_pathVirtual = false;
		this->_path = path;
		this->_previousPaths.clear();
		this->_previousPaths.push_back(path);

		if (std::filesystem::is_directory(path))
		{
			_lexer.Set("");
			return;
		}

		std::ifstream file(path);

		if (!file.is_open())
		{
			_lexer.Set("");
			return;
		}

		std::string content;
		file.seekg(0, std::ios::end);
		content.reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		_lexer.Set(content);
	}

	NodePtr Parser::RecursiveLoad(const std::string& directory, const std::string& nextPath)
	{
		std::string nextFullPath = directory + "/" + nextPath;
		for (const std::string& previousPath : _previousPaths)
		{
			if (previousPath == nextFullPath)
			{
				return nullptr;
			}
		}

		Parser parser;
		parser.Load(nextFullPath);
		parser._previousPaths.insert(parser._previousPaths.end(), _previousPaths.begin(), _previousPaths.end());
		parser.Parse();

		NodePtr node = parser.GetRoot();

		if (node)
		{
			static_cast<DocumentNode*>(node.get())->path = nextPath;
		}

		return node;
	}

	void Parser::Parse()
	{
		_lexer.Tokenize();
		_root = nullptr;

		if (_lexer.GetTokens().empty())
			return;

		_index = 0;
		_lastEnding = TokenType::Newline;

		while (!_scopes.empty())
		{
			_scopes.pop();
		}

		_root = std::make_shared<DocumentNode>(_pathVirtual ? "" : _path);
		_scopes.push(_root);

		ParseBlock();

		while (CheckBoundary(0))
		{
			ParseInline();
		}

		ParseBlockEnding();
	}

	const NodePtr& Parser::GetRoot() const
	{
		return _root;
	}

	void Parser::Visit(const Visitor& visitor)
	{
		RecursiveVisit(visitor, _root);
	}

	void Parser::RecursiveVisit(const Visitor& visitor, const NodePtr& node)
	{
		if (!visitor || !node)
			return;

		visitor(node);

		for (const NodePtr& childNode : node->children)
		{
			RecursiveVisit(visitor, childNode);
		}
	}

	void Parser::ParseInline()
	{
		switch (GetToken(0).type)
		{
			case TokenType::Text:
				AddText(GetToken(0).value, GetToken(0).range);
				Advance(0);
				break;

			case TokenType::SingleQuoteOpen:
				Add(std::make_shared<PunctNode>(PunctType::SingleQuoteOpen, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;
			case TokenType::SingleQuoteClose:
				Add(std::make_shared<PunctNode>(PunctType::SingleQuoteClose, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;
			case TokenType::QuoteOpen:
				Add(std::make_shared<PunctNode>(PunctType::QuoteOpen, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;
			case TokenType::QuoteClose:
				Add(std::make_shared<PunctNode>(PunctType::QuoteClose, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Hyphen:
				Add(std::make_shared<PunctNode>(PunctType::Hyphen, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;
			case TokenType::EnDash:
				Add(std::make_shared<PunctNode>(PunctType::EnDash, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;
			case TokenType::EmDash:
				Add(std::make_shared<PunctNode>(PunctType::EmDash, GetToken(0).value), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::EmojiMark:
				if (CheckBoundary(2) && IsToken(1, TokenType::EmojiCode) && IsToken(2, TokenType::EmojiMark))
				{
					Add(std::make_shared<EmojiNode>(GetToken(1).value), GetToken(0).range);
					Advance(2);
					break;
				}
				AddText(GetToken(0).value, GetToken(0).range);
				break;

			case TokenType::FSBold:
				if (GetScope()->group == NodeGroup::FS && static_cast<FSNode*>(GetScope().get())->type == FSType::Bold)
				{
					RemoveScope(GetToken(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Bold), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::FSItalic:
				if (GetScope()->group == NodeGroup::FS &&
					static_cast<FSNode*>(GetScope().get())->type == FSType::Italic)
				{
					RemoveScope(GetToken(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Italic), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::FSMonospace:
				if (GetScope()->group == NodeGroup::FS &&
					static_cast<FSNode*>(GetScope().get())->type == FSType::Monospace)
				{
					RemoveScope(GetToken(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Monospace), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::HeadingIDMark:
				if (CheckBoundary(1) && IsToken(1, TokenType::HeadingID) && GetScope()->group == NodeGroup::Heading)
				{
					HeadingNode* headingNode = static_cast<HeadingNode*>(GetScope().get());
					if (headingNode->type == HeadingType::Subtitle)
					{
						AddText("> " + GetToken(1).value, GetToken(0).range);
						Advance(1);
						break;
					}
					headingNode->id = GetToken(1).value;
					Advance(1);
					break;
				}
				AddText(">", GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Date: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = GetToken(0).value;
				Add(dateTimeNode, GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::Time: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->time = GetToken(0).value;
				Add(dateTimeNode, GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::DateTime: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = GetToken(0).value.substr(0, 10);
				dateTimeNode->time = GetToken(0).value.substr(11);
				Add(dateTimeNode, GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::Break:
				Add(std::make_shared<BreakNode>(BreakType::Line), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Pipe:
				if (GetScope()->group == NodeGroup::TableCell)
				{
					RemoveScope(GetToken(0).range);
				}

				if (CheckBoundary(1) && !IsToken(1, TokenType::Newline) && GetScope()->group == NodeGroup::TableRow)
				{
					AddScope(std::make_shared<TableCellNode>(), GetToken(0).range);
				}
				Advance(0);
				break;

			case TokenType::CurlyOpen: {
				if (CheckBoundary(5) && IsToken(1, TokenType::CodeSource) && IsToken(2, TokenType::CurlyClose) &&
					IsToken(3, TokenType::ParenOpen) && IsToken(4, TokenType::ResourceLabel) &&
					IsToken(5, TokenType::ParenClose))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Inline;
					codeNode->lang = GetToken(4).value;
					codeNode->source = GetToken(1).value;
					Add(codeNode, GetToken(0).range);
					Advance(5);
					break;
				}

				if (CheckBoundary(2) && IsToken(1, TokenType::CodeSource) && IsToken(2, TokenType::CurlyClose))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Inline;
					codeNode->source = GetToken(1).value;
					Add(codeNode, GetToken(0).range);
					Advance(2);
					break;
				}

				AddText("{", GetToken(0).range);
				Advance(0);
			}

			case TokenType::SquareOpen: {
				std::shared_ptr<ResourceNode> linkNode = nullptr;

				if (CheckBoundary(2) && IsToken(1, TokenType::Resource) && IsToken(2, TokenType::SquareClose))
				{
					linkNode = std::make_shared<ResourceNode>(
						HasNetProtocol(GetToken(1).value) ? ResourceType::Link : ResourceType::LinkLocal,
						GetToken(1).value
					);
					Add(linkNode, GetToken(0).range);
					Advance(2);
				}
				else if (CheckBoundary(4) && IsToken(1, TokenType::Resource) && IsToken(2, TokenType::ResourceIDMark) && IsToken(3, TokenType::ResourceID) && IsToken(4, TokenType::SquareClose))
				{
					linkNode = std::make_shared<ResourceNode>(ResourceType::LinkLocal, GetToken(1).value);
					linkNode->id = GetToken(3).value;
					Add(linkNode, GetToken(0).range);
					Advance(4);
				}

				if (linkNode)
				{
					if (CheckBoundary(2) && IsToken(0, TokenType::ParenOpen) && IsToken(1, TokenType::ResourceLabel) &&
						IsToken(2, TokenType::ParenClose))
					{
						linkNode->label = GetToken(1).value;
						Advance(2);
					}
					break;
				}

				AddText("[", GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::PresentMark: {
				std::shared_ptr<ResourceNode> presentNode = nullptr;

				if (CheckBoundary(3) && IsToken(1, TokenType::SquareOpen) && IsToken(2, TokenType::Resource) &&
					IsToken(3, TokenType::SquareClose))
				{
					presentNode = std::make_shared<ResourceNode>(
						HasNetProtocol(GetToken(2).value) ? ResourceType::Present : ResourceType::PresentLocal,
						GetToken(2).value
					);
					Add(presentNode, GetToken(0).range);
					Advance(3);
				}

				if (presentNode)
				{
					if (CheckBoundary(2) && IsToken(0, TokenType::ParenOpen) && IsToken(1, TokenType::ResourceLabel) &&
						IsToken(2, TokenType::ParenClose))
					{
						presentNode->label = GetToken(1).value;
						Advance(2);
					}
					break;
				}

				AddText("!", GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::JumpMark:
				if (CheckBoundary(3) && IsToken(1, TokenType::SquareOpen) && IsToken(2, TokenType::JumpID) &&
					IsToken(3, TokenType::SquareClose))
				{
					Add(std::make_shared<FootnoteJumpNode>(GetToken(2).value), GetToken(0).range);
					Advance(3);
					break;
				}
				AddText("^", GetToken(0).range);
				Advance(0);
				break;

			case TokenType::IncludeMark:
#ifndef __EMSCRIPTEN__
				if (CheckBoundary(3) && IsToken(1, TokenType::SquareOpen) && IsToken(2, TokenType::Resource) &&
					IsToken(3, TokenType::SquareClose) && !HasNetProtocol(GetToken(2).value))
				{
					std::string directory = _path == "" ? std::filesystem::current_path().string()
														: std::filesystem::path(_path).parent_path().string();

					NodePtr fileNode = RecursiveLoad(directory, GetToken(2).value);
					Add(fileNode ? fileNode : std::make_shared<DocumentNode>(GetToken(2).value), GetToken(0).range);
					Advance(3);
					break;
				}
#endif

				AddText("?", GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Newline:
			case TokenType::NewlinePlus:
			case TokenType::End:
				_lastEnding = GetToken(0).type;
				Advance(0);

				if (CheckBoundary(0))
				{
					ParseBlock();
				}
				break;

			case TokenType::Comment:
				Add(std::make_shared<CommentNode>(GetToken(0).value.substr(1)), GetToken(0).range);
				Advance(0);
				break;

			default:
				AddText(GetToken(0).value, GetToken(0).range);
				Advance(0);
				break;
		}
	}

	void Parser::ParseIndent()
	{
		switch (GetScope()->group)
		{
			case NodeGroup::Heading:
				if (GetScope()->As<HeadingNode>().type == HeadingType::Subtitle)
				{
					RemoveScope(GetToken(0).range);
				}
				RemoveScope(GetToken(0).range);
				break;

			default:
				break;
		}

		while (CheckBoundary(0) && (IsToken(0, TokenType::IndentIncr) || IsToken(0, TokenType::IndentDecr) ||
									IsToken(0, TokenType::BQIncr) || IsToken(0, TokenType::BQDecr)))
		{

			if (IsToken(0, TokenType::IndentIncr))
			{
				AddScope(std::make_shared<IndentNode>(), GetToken(0).range);
				Advance(0);
				continue;
			}

			if (IsToken(0, TokenType::IndentDecr))
			{
				while (CheckBoundary(0) && _scopes.size() > 1 && GetScope()->group != NodeGroup::Indent)
				{
					RemoveScope(GetToken(0).range);
				}
				IndentNode* indentNode = static_cast<IndentNode*>(GetScope().get());
				RemoveScope(GetToken(0).range);
				if (GetScope()->group == NodeGroup::ListItem)
				{
					indentNode->skipable = true;
				}
				Advance(0);
				continue;
			}

			if (IsToken(0, TokenType::BQIncr))
			{
				AddScope(std::make_shared<BQNode>(), GetToken(0).range);
				Advance(0);
				continue;
			}

			if (IsToken(0, TokenType::BQDecr))
			{
				while (CheckBoundary(0) && _scopes.size() > 1 && GetScope()->group != NodeGroup::BQ)
				{
					RemoveScope(GetToken(0).range);
				}
				RemoveScope(GetToken(0).range);
				Advance(0);
				continue;
			}

			break;
		}
	}

	void Parser::ParseBlockEnding()
	{
		switch (GetScope()->group)
		{
			case NodeGroup::ListItem: {
				RemoveScope(GetToken(0).range);
				ListType type = GetScope()->As<ListNode>().type;

				if (_lastEnding == TokenType::NewlinePlus ||
					(CheckBoundary(0) && !((IsToken(0, TokenType::Bullet) && type == ListType::Bullet) ||
										   (IsToken(0, TokenType::Index) && type == ListType::Index) ||
										   (IsToken(0, TokenType::Checkbox) && type == ListType::Check))))
				{
					_lastListDeadBecauseNewlinePlus = _lastEnding == TokenType::NewlinePlus;
					RemoveScope(GetToken(0).range);
				}
				break;
			}

			case NodeGroup::Paragraph:
				if (GetScope()->children.empty())
				{
					RemoveScope(GetToken(0).range);
					GetScope()->children.pop_back();
				}
				else if (_lastEnding == TokenType::NewlinePlus || (CheckBoundary(0) && IsToken(0, TokenType::Bullet) || IsToken(0, TokenType::Index) || IsToken(0, TokenType::Checkbox) || IsToken(0, TokenType::Pipe) || IsToken(0, TokenType::ChapterMark) || IsToken(0, TokenType::DescribeMark)))
				{
					RemoveScope(GetToken(0).range);
				}
				break;

			case NodeGroup::TableCell:
				if (GetScope()->children.empty())
				{
					RemoveScope(GetToken(0).range);
					GetScope()->children.pop_back();
				}
				else
				{
					RemoveScope(GetToken(0).range);
				}

				// tableRow
				RemoveScope(GetToken(0).range);

				if (_lastEnding == TokenType::NewlinePlus || (CheckBoundary(0) && !IsToken(0, TokenType::Pipe)))
				{
					RemoveScope(GetToken(0).range);
				}
				break;

			case NodeGroup::TableRow:
				RemoveScope(GetToken(0).range);

				if (_lastEnding == TokenType::NewlinePlus || (CheckBoundary(0) && !IsToken(0, TokenType::Pipe)))
				{
					RemoveScope(GetToken(0).range);
				}
				break;

			case NodeGroup::FootnoteDescribe:
				RemoveScope(GetToken(0).range);
				break;

			case NodeGroup::Admon:
				RemoveScope(GetToken(0).range);
				break;

			default:
				break;
		}
	}

	void Parser::ParseBlock()
	{
		ParseIndent();
		ParseBlockEnding();

		switch (GetToken(0).type)
		{
			case TokenType::Bullet:
				if (GetScope()->group != NodeGroup::List)
				{
					AddScope(std::make_shared<ListNode>(ListType::Bullet), GetToken(0).range);
				}
				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Index:
				if (GetScope()->group != NodeGroup::List)
				{
					if (!GetScope()->children.empty() && GetScope()->children.back()->group == NodeGroup::List &&
						GetScope()->children.back()->As<ListNode>().type == ListType::Index &&
						!_lastListDeadBecauseNewlinePlus)
					{
						_scopes.push(GetScope()->children.back());
					}
					else
					{
						AddScope(std::make_shared<ListNode>(ListType::Index), GetToken(0).range);
					}
				}
				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::Checkbox: {
				if (GetScope()->group != NodeGroup::List)
				{
					if (!GetScope()->children.empty() && GetScope()->children.back()->group == NodeGroup::List &&
						GetScope()->children.back()->As<ListNode>().type == ListType::Check &&
						!_lastListDeadBecauseNewlinePlus)
					{
						_scopes.push(GetScope()->children.back());
					}
					else
					{
						AddScope(std::make_shared<ListNode>(ListType::Check), GetToken(0).range);
					}
				}

				ListItemState state = ListItemState::None;
				switch (GetToken(0).value[1])
				{
					case ' ':
						state = ListItemState::Todo;
						break;
					case 'v':
						state = ListItemState::Done;
						break;
					case 'x':
						state = ListItemState::Cancelled;
						break;
				}

				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1, state), GetToken(0).range);
				Advance(0);
				break;
			}

			case TokenType::Pipe:
				if (GetScope()->group != NodeGroup::Table)
				{
					AddScope(std::make_shared<TableNode>(), GetToken(0).range);
				}

				if (CheckBoundary(1) && IsToken(1, TokenType::PipeConnector))
				{
					TableNode* tableNode = static_cast<TableNode*>(GetScope().get());
					if (tableNode->header == 0)
					{
						tableNode->header = tableNode->children.size();
						tableNode->footer = tableNode->children.size();
					}
					else
					{
						tableNode->footer = tableNode->children.size();
					}

					while (CheckBoundary(0) && (IsToken(0, TokenType::Pipe) || IsToken(0, TokenType::PipeConnector)))
					{
						if (IsToken(0, TokenType::PipeConnector))
						{
							if (GetToken(0).value == ":--")
							{
								tableNode->alignments.push_back(Alignment::Left);
							}
							else if (GetToken(0).value == "--:")
							{
								tableNode->alignments.push_back(Alignment::Right);
							}
							else
							{
								tableNode->alignments.push_back(Alignment::Center);
							}
						}
						Advance(0);
					}
					break;
				}

				if (GetScope()->group == NodeGroup::Table)
				{
					AddScope(std::make_shared<TableRowNode>(), GetToken(0).range);
				}
				break;

			case TokenType::CurlyOpen: {
				// see default inline
				break;
			}

			case TokenType::CodeMark: {
				if (CheckBoundary(2) && IsToken(1, TokenType::CodeSource) && IsToken(2, TokenType::CodeMark))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Block;
					codeNode->source = GetToken(1).value;
					Add(codeNode, GetToken(0).range);
					Advance(2);
					break;
				}

				if (CheckBoundary(3) && IsToken(1, TokenType::CodeLang) && IsToken(2, TokenType::CodeSource) &&
					IsToken(3, TokenType::CodeMark))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Block;
					codeNode->lang = GetToken(1).value;
					codeNode->source = GetToken(2).value;
					Add(codeNode, GetToken(0).range);
					Advance(3);
					break;
				}

				// see default inline
				break;
			}

			case TokenType::DescribeMark:
				if (CheckBoundary(3) && IsToken(1, TokenType::SquareOpen) && IsToken(2, TokenType::JumpID) &&
					IsToken(3, TokenType::SquareClose))
				{
					AddScope(std::make_shared<FootnoteDescribeNode>(GetToken(2).value), GetToken(0).range);
					Advance(3);
				}

				// see default inline
				break;

			case TokenType::AdmonNote:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Note), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::AdmonHint:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Hint), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::AdmonImportant:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Important), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::AdmonWarning:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Warning), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::AdmonSeeAlso:
				AddScope(std::make_shared<AdmonNode>(AdmonType::SeeAlso), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::AdmonTip:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Tip), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::ChapterMark:
				AddScope(std::make_shared<HeadingNode>(HeadingType::Chapter), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::SectionMark:
				AddScope(std::make_shared<HeadingNode>(HeadingType::Section), GetToken(0).range);
				Advance(0);
				break;

			case TokenType::SubsectionMark:
				if (!GetScope()->children.empty() && GetScope()->children.back()->group == NodeGroup::Heading &&
					_lastEnding == TokenType::Newline)
				{
					_scopes.push(GetScope()->children.back());
					AddScope(std::make_shared<HeadingNode>(HeadingType::Subtitle), GetToken(0).range);
				}
				else
				{
					AddScope(std::make_shared<HeadingNode>(HeadingType::Subsection), GetToken(0).range);
				}
				Advance(0);
				break;

			case TokenType::Break:
				if (GetToken(0).value.size() == 3)
				{
					Add(std::make_shared<BreakNode>(BreakType::Page), GetToken(0).range);
					Advance(0);
					break;
				}
				// see inline break_
				break;

			case TokenType::Dinkus:
				Add(std::make_shared<BreakNode>(BreakType::Thematic), GetToken(0).range);
				Advance(0);
				break;

			default:
				if (GetScope()->group != NodeGroup::Paragraph)
				{
					AddScope(std::make_shared<ParagraphNode>(), GetToken(0).range);
				}
				else if (GetScope()->group == NodeGroup::Paragraph)
				{
					AddText("\n", GetToken(0).range);
				}
				break;
		}
	}
}
