#include "gularen/parser.h"
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

	bool Parser::Check(size_t offset) const
	{
		return _index + offset < _lexer.Get().size();
	}

	bool Parser::Is(size_t offset, TokenType type) const
	{
		return _lexer.Get()[_index + offset].type == type;
	}

	const Token& Parser::Get(size_t offset) const
	{
		return _lexer.Get()[_index + offset];
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

		NodePtr node = parser.Get();

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

		if (_lexer.Get().empty())
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

		while (Check(0))
		{
			ParseInline();
		}

		ParseBlockEnding();
	}

	const NodePtr& Parser::Get() const
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
		switch (Get(0).type)
		{
			case TokenType::Text:
				AddText(Get(0).value, Get(0).range);
				Advance(0);
				break;

			case TokenType::LSQuo:
				Add(std::make_shared<PunctNode>(PunctType::LSQuo, Get(0).value), Get(0).range);
				Advance(0);
				break;
			case TokenType::RSQuo:
				Add(std::make_shared<PunctNode>(PunctType::RSQuo, Get(0).value), Get(0).range);
				Advance(0);
				break;
			case TokenType::LDQuo:
				Add(std::make_shared<PunctNode>(PunctType::LDQuo, Get(0).value), Get(0).range);
				Advance(0);
				break;
			case TokenType::RDQuo:
				Add(std::make_shared<PunctNode>(PunctType::RDQuo, Get(0).value), Get(0).range);
				Advance(0);
				break;

			case TokenType::Hyphen:
				Add(std::make_shared<PunctNode>(PunctType::Hyphen, Get(0).value), Get(0).range);
				Advance(0);
				break;
			case TokenType::EnDash:
				Add(std::make_shared<PunctNode>(PunctType::EnDash, Get(0).value), Get(0).range);
				Advance(0);
				break;
			case TokenType::EmDash:
				Add(std::make_shared<PunctNode>(PunctType::EmDash, Get(0).value), Get(0).range);
				Advance(0);
				break;

			case TokenType::EmojiMark:
				if (Check(2) && Is(1, TokenType::EmojiCode) && Is(2, TokenType::EmojiMark))
				{
					Add(std::make_shared<EmojiNode>(Get(1).value), Get(0).range);
					Advance(2);
					break;
				}
				AddText(Get(0).value, Get(0).range);
				break;

			case TokenType::FSBold:
				if (GetScope()->group == NodeGroup::FS && static_cast<FSNode*>(GetScope().get())->type == FSType::Bold)
				{
					RemoveScope(Get(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Bold), Get(0).range);
				Advance(0);
				break;

			case TokenType::FSItalic:
				if (GetScope()->group == NodeGroup::FS &&
					static_cast<FSNode*>(GetScope().get())->type == FSType::Italic)
				{
					RemoveScope(Get(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Italic), Get(0).range);
				Advance(0);
				break;

			case TokenType::FSMonospace:
				if (GetScope()->group == NodeGroup::FS &&
					static_cast<FSNode*>(GetScope().get())->type == FSType::Monospace)
				{
					RemoveScope(Get(0).range);
					Advance(0);
					break;
				}
				AddScope(std::make_shared<FSNode>(FSType::Monospace), Get(0).range);
				Advance(0);
				break;

			case TokenType::HeadingIDMark:
				if (Check(1) && Is(1, TokenType::HeadingID) && GetScope()->group == NodeGroup::Heading)
				{
					HeadingNode* headingNode = static_cast<HeadingNode*>(GetScope().get());
					if (headingNode->type == HeadingType::Subtitle)
					{
						AddText("> " + Get(1).value, Get(0).range);
						Advance(1);
						break;
					}
					headingNode->id = Get(1).value;
					Advance(1);
					break;
				}
				AddText(">", Get(0).range);
				Advance(0);
				break;

			case TokenType::Date: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = Get(0).value;
				Add(dateTimeNode, Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::Time: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->time = Get(0).value;
				Add(dateTimeNode, Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::DateTime: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = Get(0).value.substr(0, 10);
				dateTimeNode->time = Get(0).value.substr(11);
				Add(dateTimeNode, Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::Break:
				Add(std::make_shared<BreakNode>(BreakType::Line), Get(0).range);
				Advance(0);
				break;

			case TokenType::Pipe:
				if (GetScope()->group == NodeGroup::TableCell)
				{
					RemoveScope(Get(0).range);
				}

				if (Check(1) && !Is(1, TokenType::Newline) && GetScope()->group == NodeGroup::TableRow)
				{
					AddScope(std::make_shared<TableCellNode>(), Get(0).range);
				}
				Advance(0);
				break;

			case TokenType::CurlyOpen: {
				if (Check(5) && Is(1, TokenType::CodeSource) && Is(2, TokenType::CurlyClose) &&
					Is(3, TokenType::ParenOpen) && Is(4, TokenType::ResourceLabel) && Is(5, TokenType::ParenClose))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Inline;
					codeNode->lang = Get(4).value;
					codeNode->source = Get(1).value;
					Add(codeNode, Get(0).range);
					Advance(5);
					break;
				}

				if (Check(2) && Is(1, TokenType::CodeSource) && Is(2, TokenType::CurlyClose))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Inline;
					codeNode->source = Get(1).value;
					Add(codeNode, Get(0).range);
					Advance(2);
					break;
				}

				AddText("{", Get(0).range);
				Advance(0);
			}

			case TokenType::SquareOpen: {
				std::shared_ptr<ResourceNode> linkNode = nullptr;

				if (Check(2) && Is(1, TokenType::Resource) && Is(2, TokenType::SquareClose))
				{
					linkNode = std::make_shared<ResourceNode>(
						HasNetProtocol(Get(1).value) ? ResourceType::Link : ResourceType::LinkLocal, Get(1).value
					);
					Add(linkNode, Get(0).range);
					Advance(2);
				}
				else if (Check(4) && Is(1, TokenType::Resource) && Is(2, TokenType::ResourceIDMark) && Is(3, TokenType::ResourceID) && Is(4, TokenType::SquareClose))
				{
					linkNode = std::make_shared<ResourceNode>(ResourceType::LinkLocal, Get(1).value);
					linkNode->id = Get(3).value;
					Add(linkNode, Get(0).range);
					Advance(4);
				}

				if (linkNode)
				{
					if (Check(2) && Is(0, TokenType::ParenOpen) && Is(1, TokenType::ResourceLabel) &&
						Is(2, TokenType::ParenClose))
					{
						linkNode->label = Get(1).value;
						Advance(2);
					}
					break;
				}

				AddText("[", Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::PresentMark: {
				std::shared_ptr<ResourceNode> presentNode = nullptr;

				if (Check(3) && Is(1, TokenType::SquareOpen) && Is(2, TokenType::Resource) &&
					Is(3, TokenType::SquareClose))
				{
					presentNode = std::make_shared<ResourceNode>(
						HasNetProtocol(Get(2).value) ? ResourceType::Present : ResourceType::PresentLocal,
						Get(2).value
					);
					Add(presentNode, Get(0).range);
					Advance(3);
				}

				if (presentNode)
				{
					if (Check(2) && Is(0, TokenType::ParenOpen) && Is(1, TokenType::ResourceLabel) &&
						Is(2, TokenType::ParenClose))
					{
						presentNode->label = Get(1).value;
						Advance(2);
					}
					break;
				}

				AddText("!", Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::JumpMark:
				if (Check(3) && Is(1, TokenType::SquareOpen) && Is(2, TokenType::JumpID) &&
					Is(3, TokenType::SquareClose))
				{
					Add(std::make_shared<FootnoteJumpNode>(Get(2).value), Get(0).range);
					Advance(3);
					break;
				}
				AddText("^", Get(0).range);
				Advance(0);
				break;

			case TokenType::IncludeMark:
				#ifndef __EMSCRIPTEN__
				if (Check(3) && Is(1, TokenType::SquareOpen) && Is(2, TokenType::Resource) &&
					Is(3, TokenType::SquareClose) && !HasNetProtocol(Get(2).value))
				{
					std::string directory = _path == "" ? std::filesystem::current_path().string()
														: std::filesystem::path(_path).parent_path().string();

					NodePtr fileNode = RecursiveLoad(directory, Get(2).value);
					Add(fileNode ? fileNode : std::make_shared<DocumentNode>(Get(2).value), Get(0).range);
					Advance(3);
					break;
				}
				#endif

				AddText("?", Get(0).range);
				Advance(0);
				break;

			case TokenType::Newline:
			case TokenType::NewlinePlus:
			case TokenType::End:
				_lastEnding = Get(0).type;
				Advance(0);

				if (Check(0))
				{
					ParseBlock();
				}
				break;

			case TokenType::Comment:
				Add(std::make_shared<CommentNode>(Get(0).value.substr(1)), Get(0).range);
				Advance(0);
				break;

			default:
				AddText(Get(0).value, Get(0).range);
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
					RemoveScope(Get(0).range);
				}
				RemoveScope(Get(0).range);
				break;

			default:
				break;
		}

		while (Check(0) && (Is(0, TokenType::IndentIncr) || Is(0, TokenType::IndentDecr) || Is(0, TokenType::BQIncr) ||
							Is(0, TokenType::BQDecr)))
		{

			if (Is(0, TokenType::IndentIncr))
			{
				AddScope(std::make_shared<IndentNode>(), Get(0).range);
				Advance(0);
				continue;
			}

			if (Is(0, TokenType::IndentDecr))
			{
				while (Check(0) && _scopes.size() > 1 && GetScope()->group != NodeGroup::Indent)
				{
					RemoveScope(Get(0).range);
				}
				RemoveScope(Get(0).range);
				Advance(0);
				continue;
			}

			if (Is(0, TokenType::BQIncr))
			{
				AddScope(std::make_shared<BQNode>(), Get(0).range);
				Advance(0);
				continue;
			}

			if (Is(0, TokenType::BQDecr))
			{
				while (Check(0) && _scopes.size() > 1 && GetScope()->group != NodeGroup::BQ)
				{
					RemoveScope(Get(0).range);
				}
				RemoveScope(Get(0).range);
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
				RemoveScope(Get(0).range);
				ListType type = GetScope()->As<ListNode>().type;

				if (_lastEnding == TokenType::NewlinePlus ||
					(Check(0) && !((Is(0, TokenType::Bullet) && type == ListType::Bullet) ||
								   (Is(0, TokenType::Index) && type == ListType::Index) ||
								   (Is(0, TokenType::Checkbox) && type == ListType::Check))))
				{
					_lastListDeadBecauseNewlinePlus = _lastEnding == TokenType::NewlinePlus;
					RemoveScope(Get(0).range);
				}
				break;
			}

			case NodeGroup::Paragraph:
				if (GetScope()->children.empty())
				{
					RemoveScope(Get(0).range);
					GetScope()->children.pop_back();
				}
				else if (_lastEnding == TokenType::NewlinePlus || (Check(0) && Is(0, TokenType::Bullet) || Is(0, TokenType::Index) || Is(0, TokenType::Checkbox) || Is(0, TokenType::Pipe) || Is(0, TokenType::ChapterMark) || Is(0, TokenType::DescribeMark)))
				{
					RemoveScope(Get(0).range);
				}
				break;

			case NodeGroup::TableCell:
				if (GetScope()->children.empty())
				{
					RemoveScope(Get(0).range);
					GetScope()->children.pop_back();
				}
				else
				{
					RemoveScope(Get(0).range);
				}

				// tableRow
				RemoveScope(Get(0).range);

				if (_lastEnding == TokenType::NewlinePlus || (Check(0) && !Is(0, TokenType::Pipe)))
				{
					RemoveScope(Get(0).range);
				}
				break;

			case NodeGroup::TableRow:
				RemoveScope(Get(0).range);

				if (_lastEnding == TokenType::NewlinePlus || (Check(0) && !Is(0, TokenType::Pipe)))
				{
					RemoveScope(Get(0).range);
				}
				break;

			case NodeGroup::FootnoteDescribe:
				RemoveScope(Get(0).range);
				break;

			case NodeGroup::Admon:
				RemoveScope(Get(0).range);
				break;

			default:
				break;
		}
	}

	void Parser::ParseBlock()
	{
		ParseIndent();
		ParseBlockEnding();

		switch (Get(0).type)
		{
			case TokenType::Bullet:
				if (GetScope()->group != NodeGroup::List)
				{
					AddScope(std::make_shared<ListNode>(ListType::Bullet), Get(0).range);
				}
				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1), Get(0).range);
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
						AddScope(std::make_shared<ListNode>(ListType::Index), Get(0).range);
					}
				}
				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1), Get(0).range);
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
						AddScope(std::make_shared<ListNode>(ListType::Check), Get(0).range);
					}
				}

				ListItemState state = ListItemState::None;
				switch (Get(0).value[1])
				{
					case ' ':
						state = ListItemState::Todo;
						break;
					case 'v':
						state = ListItemState::Done;
						break;
					case 'x':
						state = ListItemState::Canceled;
						break;
				}

				AddScope(std::make_shared<ListItemNode>(GetScope()->children.size() + 1, state), Get(0).range);
				Advance(0);
				break;
			}

			case TokenType::Pipe:
				if (GetScope()->group != NodeGroup::Table)
				{
					AddScope(std::make_shared<TableNode>(), Get(0).range);
				}

				if (Check(1) && Is(1, TokenType::PipeConnector))
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

					while (Check(0) && (Is(0, TokenType::Pipe) || Is(0, TokenType::PipeConnector)))
					{
						if (Is(0, TokenType::PipeConnector))
						{
							if (Get(0).value == ":--")
							{
								tableNode->alignments.push_back(Alignment::Left);
							}
							else if (Get(0).value == "--:")
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
					AddScope(std::make_shared<TableRowNode>(), Get(0).range);
				}
				break;

			case TokenType::CurlyOpen: {
				// see default inline
				break;
			}

			case TokenType::CodeMark: {
				if (Check(2) && Is(1, TokenType::CodeSource) && Is(2, TokenType::CodeMark))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Block;
					codeNode->source = Get(1).value;
					Add(codeNode, Get(0).range);
					Advance(2);
					break;
				}

				if (Check(3) && Is(1, TokenType::CodeLang) && Is(2, TokenType::CodeSource) &&
					Is(3, TokenType::CodeMark))
				{
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::Block;
					codeNode->lang = Get(1).value;
					codeNode->source = Get(2).value;
					Add(codeNode, Get(0).range);
					Advance(3);
					break;
				}

				// see default inline
				break;
			}

			case TokenType::DescribeMark:
				if (Check(3) && Is(1, TokenType::SquareOpen) && Is(2, TokenType::JumpID) &&
					Is(3, TokenType::SquareClose))
				{
					AddScope(std::make_shared<FootnoteDescribeNode>(Get(2).value), Get(0).range);
					Advance(3);
				}

				// see default inline
				break;

			case TokenType::AdmonNote:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Note), Get(0).range);
				Advance(0);
				break;

			case TokenType::AdmonHint:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Hint), Get(0).range);
				Advance(0);
				break;

			case TokenType::AdmonImportant:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Important), Get(0).range);
				Advance(0);
				break;

			case TokenType::AdmonWarning:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Warning), Get(0).range);
				Advance(0);
				break;

			case TokenType::AdmonSeeAlso:
				AddScope(std::make_shared<AdmonNode>(AdmonType::SeeAlso), Get(0).range);
				Advance(0);
				break;

			case TokenType::AdmonTip:
				AddScope(std::make_shared<AdmonNode>(AdmonType::Tip), Get(0).range);
				Advance(0);
				break;

			case TokenType::ChapterMark:
				AddScope(std::make_shared<HeadingNode>(HeadingType::Chapter), Get(0).range);
				Advance(0);
				break;

			case TokenType::SectionMark:
				AddScope(std::make_shared<HeadingNode>(HeadingType::Section), Get(0).range);
				Advance(0);
				break;

			case TokenType::SubsectionMark:
				if (!GetScope()->children.empty() && GetScope()->children.back()->group == NodeGroup::Heading &&
					_lastEnding == TokenType::Newline)
				{
					_scopes.push(GetScope()->children.back());
					AddScope(std::make_shared<HeadingNode>(HeadingType::Subtitle), Get(0).range);
				}
				else
				{
					AddScope(std::make_shared<HeadingNode>(HeadingType::Subsection), Get(0).range);
				}
				Advance(0);
				break;

			case TokenType::Break:
				if (Get(0).value.size() == 3)
				{
					Add(std::make_shared<BreakNode>(BreakType::Page), Get(0).range);
					Advance(0);
					break;
				}
				// see inline break_
				break;

			case TokenType::Dinkus:
				Add(std::make_shared<BreakNode>(BreakType::Thematic), Get(0).range);
				Advance(0);
				break;

			default:
				if (GetScope()->group != NodeGroup::Paragraph)
				{
					AddScope(std::make_shared<ParagraphNode>(), Get(0).range);
				}
				else if (GetScope()->group == NodeGroup::Paragraph)
				{
					AddText("\n", Get(0).range);
				}
				break;
		}
	}
}
