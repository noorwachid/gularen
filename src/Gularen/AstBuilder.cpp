#include "AstBuilder.hpp"
#include "IO.hpp"

namespace Gularen {

    AstBuilder::AstBuilder() {
    }

    AstBuilder::~AstBuilder() {
        if (root)
            destroyTree();
    }

    void AstBuilder::setBuffer(const std::string &buffer) {
        lexer.setBuffer(buffer);
        lexer.parse();

        reset();
    }

    void AstBuilder::setTokens(const std::vector<Token> &tokens) {
        lexer.setTokens(tokens);

        reset();
    }

    void AstBuilder::parse() {
        if (getCurrentToken().type == TokenType::DocumentBegin)
            skip();

        parseNewline();

        while (isValid()) {
            switch (getCurrentToken().type) {
                case TokenType::Text:
                    getHead()->add(new ValueNode(NodeType::Text, NodeGroup::Text, NodeShape::InBetween,
                                                 getCurrentToken().value));
                    skip();
                    break;

                case TokenType::Asterisk:
                    pairFHead(NodeType::FBold);
                    break;

                case TokenType::Underline:
                    pairFHead(NodeType::FItalic);
                    break;

                case TokenType::Backtick:
                    pairFHead(NodeType::FMonospace);
                    break;

                case TokenType::Newline: {
                    size_t newlineSize = getCurrentToken().size;
                    // getHead()->add(new SizeNode(NodeType::Newline, GetCurrent().size));
                    skip();
                    parseNewline(newlineSize);
                    break;
                }

                case TokenType::Anchor: {
                    if (getHead()->group == NodeGroup::Header) {
                        static_cast<ValueNode *>(getHead())->value = getCurrentToken().value;
                    }
                    skip();
                    break;
                }

                case TokenType::RevTail:
                    getHead()->add(new Node(NodeType::LineBreak, NodeGroup::Break));
                    skip();
                    break;

                case TokenType::Teeth: {
                    skip();
                    if (getCurrentToken().type == TokenType::Text && getNextToken().type == TokenType::Teeth) {
                        getHead()->add(new ValueNode(NodeType::InlineCode, NodeGroup::Text, NodeShape::InBetween,
                                                     getCurrentToken().value));
                        skip(2);
                    }
                    break;
                }

                case TokenType::LCurlyBracket:
                    skip();
                    switch (getCurrentToken().type) {
                        case TokenType::Symbol:
                            getHead()->add(new ValueNode(NodeType::Curtain, NodeGroup::Text, NodeShape::InBetween,
                                                         getCurrentToken().value));
                            skip();
                            break;

                        case TokenType::Colon:
                            parseLink(NodeType::Link);
                            break;

                        case TokenType::ExclamationMark:
                            parseLink(NodeType::LocalLink);
                            break;

                        case TokenType::QuestionMark:
                            parseLink(NodeType::InlineImage);
                            break;

                        default:
                            break;
                    }

                    break;

                case TokenType::RCurlyBracket:
                    skip();
                    popHead();
                    break;

                case TokenType::Pipe:
                    while (heads.size() > 1 && heads.top()->type != NodeType::TableRow)
                        popHead();
                    pushHead(new Node(NodeType::TableColumn, NodeGroup::Table));
                    skip();
                    break;

                case TokenType::HashSymbol:
                    getHead()->add(new ValueNode(NodeType::HashSymbol, NodeGroup::Tag, NodeShape::InBetween,
                                                 getCurrentToken().value));
                    skip();
                    break;
                case TokenType::AtSymbol:
                    getHead()->add(new ValueNode(NodeType::AtSymbol, NodeGroup::Tag, NodeShape::InBetween,
                                                 getCurrentToken().value));
                    skip();
                    break;

                default:
                    skip();
                    break;
            }
        }

        parseNewline();
    }

    void AstBuilder::reset() {
        tokenIndex = 0;
        tokenSize = lexer.getTokens().size();

        if (root)
            delete root;

        root = new Node(NodeType::Root);

        while (!heads.empty())
            heads.pop();

        heads.push(root);
    }

    void AstBuilder::destroyTree() {
        traverseAndDestroyNode(root);
    }

    std::string AstBuilder::getBuffer() {
        return lexer.getBuffer();
    }

    Node *AstBuilder::getTree() {
        return root;
    }

    std::string AstBuilder::getTokensAsString() {
        return lexer.getTokensAsString();
    }

    std::string AstBuilder::getTreeAsString() {
        buffer.clear();

        traverseAndGenerateBuffer(getTree(), 0);

        return buffer + "\n";
    }

    void AstBuilder::traverseAndGenerateBuffer(Node *node, size_t depth) {
        for (size_t i = 0; i < depth; ++i)
            buffer += "    ";

        buffer += node->toString() + "\n";

        for (Node *child: node->children)
            traverseAndGenerateBuffer(child, depth + 1);
    }

    void AstBuilder::traverseAndDestroyNode(Node *node) {
        for (Node *child: node->children)
            traverseAndDestroyNode(child);

        if (node->group == NodeGroup::Link) {
            Node *packageNode = static_cast<ContainerNode *>(node)->package;
            delete packageNode;
            packageNode = nullptr;

        }

        delete node;
        node = nullptr;
    }

    void AstBuilder::parseNewline(size_t newlineSize) {
        parseIndentation();

        switch (getCurrentToken().type) {
            case TokenType::Tail:
                switch (getCurrentToken().size) {
                    case 1:
                        compareAndPopHead(NodeType::Minisection, newlineSize);
                        pushHead(new ValueNode(NodeType::Minisection, NodeGroup::Header, NodeShape::Line));
                        break;

                    case 2:
                        compareAndPopHead(NodeType::Part, newlineSize);
                        pushHead(new ValueNode(NodeType::Part, NodeGroup::Header, NodeShape::Line));
                        break;

                    case 3:
                        compareAndPopHead(NodeType::Title, newlineSize);
                        pushHead(new ValueNode(NodeType::Title, NodeGroup::Header, NodeShape::Line));
                        break;

                    default:
                        break;
                }
                skip();
                break;

            case TokenType::RevTail:
                if (getCurrentToken().size == 2) {
                    compareAndPopHead(NodeType::ThematicBreak);
                    getHead()->add(new Node(NodeType::ThematicBreak, NodeGroup::Break, NodeShape::Line));
                    skip();
                } else if (getCurrentToken().size > 2) {
                    compareAndPopHead(NodeType::PageBreak);
                    getHead()->add(new Node(NodeType::PageBreak, NodeGroup::Break, NodeShape::Line));
                    skip();
                }
                break;

            case TokenType::Arrow:
                switch (getCurrentToken().size) {
                    case 1:
                        compareAndPopHead(NodeType::Subsubsection, newlineSize);
                        pushHead(new ValueNode(NodeType::Subsubsection, NodeGroup::Header, NodeShape::Line));
                        break;

                    case 2:
                        compareAndPopHead(NodeType::Subsection, newlineSize);
                        pushHead(new ValueNode(NodeType::Subsection, NodeGroup::Header, NodeShape::Line));
                        break;

                    case 3:
                        compareAndPopHead(NodeType::Section, newlineSize);
                        pushHead(new ValueNode(NodeType::Section, NodeGroup::Header, NodeShape::Line));
                        break;

                    case 4:
                        compareAndPopHead(NodeType::Chapter, newlineSize);
                        pushHead(new ValueNode(NodeType::Chapter, NodeGroup::Header, NodeShape::Line));
                        break;

                    default:
                        break;
                }

                break;

            case TokenType::Bullet:
                compareAndPopHead(NodeType::List);

                if (getHead()->type != NodeType::List)
                    pushHead(new Node(NodeType::List, NodeGroup::List, NodeShape::Block));

                pushHead(new Node(NodeType::Item, NodeGroup::Item, NodeShape::Block));
                skip();
                break;

            case TokenType::NBullet:
                compareAndPopHead(NodeType::NList);

                if (getHead()->type != NodeType::NList)
                    pushHead(new Node(NodeType::NList, NodeGroup::List, NodeShape::Block));

                pushHead(new Node(NodeType::Item, NodeGroup::Item, NodeShape::Block));
                skip();
                break;

            case TokenType::CheckBox:
                compareAndPopHead(NodeType::CheckList);

                if (getHead()->type != NodeType::CheckList)
                    pushHead(new Node(NodeType::CheckList, NodeGroup::List, NodeShape::Block));

                pushHead(new TernaryNode(NodeType::CheckItem, NodeGroup::Item, NodeShape::Block,
                                         static_cast<TernaryState>(getCurrentToken().size)));
                skip();
                break;

            case TokenType::Dollar:
                skip();
                parseBlock(getCurrentToken().type);
                break;

            case TokenType::Line:
                while (heads.size() > 1 && heads.top()->shape != NodeShape::Block)
                    heads.pop();

                popHead();
                skip();
                break;

            default:
                if (getHead()->group != NodeGroup::Table) {
                    compareAndPopHead(NodeType::Paragraph, newlineSize);

                    if (getHead()->type != NodeType::Paragraph)
                        pushHead(new Node(NodeType::Paragraph));
                    else
                        getHead()->add(new Node(NodeType::Newline));
                } else {
                    compareAndPopHead(NodeType::Table);
                    pushHead(new Node(NodeType::TableRow, NodeGroup::Table, NodeShape::Line));
                    pushHead(new Node(NodeType::TableColumn, NodeGroup::Table, NodeShape::Line));
                }
                break;
        }
    }

    void AstBuilder::parseIndentation() {
        size_t currentDepth = 0;

        if (getCurrentToken().type == TokenType::Space) {
            currentDepth = getCurrentToken().size / 4;
            skip();
        }

        if (currentDepth > depth) {
            size_t distance = currentDepth - depth - 1;
            for (size_t i = 0; i < distance; ++i)
                pushHead(new Node(NodeType::Indent, NodeGroup::Wrapper, NodeShape::SuperBlock));

            pushHead(new Node(
                    getHead()->group != NodeGroup::Item ? NodeType::Indent : NodeType::Wrapper,
                    NodeGroup::Wrapper,
                    NodeShape::SuperBlock));
        } else if (currentDepth < depth) {
            size_t distance = depth - currentDepth;
            size_t i = 0;
            while (i < distance) {
                if (getHead()->type == NodeType::Root)
                    break;

                if (getHead()->shape == NodeShape::SuperBlock)
                    ++i;

                popHead();
            }
        }

        depth = currentDepth;
    }

    void AstBuilder::parseBreak() {
        switch (getCurrentToken().size) {
            case 1:
                getHead()->add(new Node(NodeType::LineBreak, NodeGroup::Break));
                break;
            case 2:
                getHead()->add(new Node(NodeType::ThematicBreak, NodeGroup::Break));
                break;
            case 3:
                getHead()->add(new Node(NodeType::PageBreak, NodeGroup::Break));
                break;
        }
        skip();
    }

    void AstBuilder::parseLink(NodeType type) {
        ContainerNode *container = new ContainerNode(type, NodeGroup::Link);
        ValueNode *node = new ValueNode();
        skip();

        if (getCurrentToken().type == TokenType::QuotedText) {
            node->type = NodeType::QuotedText;
            node->value = getCurrentToken().value;
            skip();
        } else if (getCurrentToken().type == TokenType::Symbol) {
            node->type = NodeType::Symbol;
            node->value = getCurrentToken().value;
            skip();
        }
        container->package = node;

        if (getCurrentToken().type == TokenType::LCurlyBracket) {
            pushHead(container);
            pushHead(new Node(NodeType::Wrapper));
            skip();
        } else
            pushHead(container);
    }

    void AstBuilder::parseBlock(TokenType type) {
        switch (type) {
            case TokenType::KwTable:
                compareAndPopHead(NodeType::Table);

                while (isValid() && getCurrentToken().type != TokenType::Line) {
                    skip();
                }
                pushHead(new TableNode());
                break;

            case TokenType::KwCode: {
                compareAndPopHead(NodeType::Code);

                CodeNode *codeNode = new CodeNode();
                getHead()->add(codeNode);
                skip();
                if (getCurrentToken().type == TokenType::QuotedText) {
                    codeNode->lang = new ValueNode(NodeType::QuotedText, NodeGroup::Text, NodeShape::Block,
                                                   getCurrentToken().value);
                    skip();
                }

                // skip indentations
                if (getCurrentToken().type == TokenType::Newline)
                    skip();
                if (getCurrentToken().type == TokenType::Space)
                    skip();

                if (getCurrentToken().type == TokenType::Line &&
                    getNextToken(1).type == TokenType::RawText &&
                    getNextToken(2).type == TokenType::Line) {
                    codeNode->value = getNextToken(1).value;
                    skip(3);
                }
                break;
            }

            case TokenType::KwToc:
                while (isValid() && getCurrentToken().type != TokenType::Newline)
                    skip();
                getHead()->add(new Node(NodeType::Toc, NodeGroup::Unknown, NodeShape::Line));
                break;

            case TokenType::KwFile:
                while (isValid() && getCurrentToken().type != TokenType::Newline)
                    skip();
                getHead()->add(new Node(NodeType::File, NodeGroup::Unknown, NodeShape::Line));
                break;

            case TokenType::KwImage:
                while (isValid() && getCurrentToken().type != TokenType::Newline)
                    skip();
                getHead()->add(new Node(NodeType::Image, NodeGroup::Unknown, NodeShape::Line));
                break;

            case TokenType::KwAdmon:
                while (isValid() && getCurrentToken().type != TokenType::Line)
                    skip();
                getHead()->add(new Node(NodeType::Admon, NodeGroup::Unknown, NodeShape::Block));
                break;

//        case TokenType::Symbol:
//            while (isValid() && getCurrentToken().type != TokenType::Line)
//                skip();
//            getHead()->add(new Node(NodeType::Assignment, NodeGroup::Unknown, NodeShape::Line));
//            break;

            default:
                break;
        }
    }

    Node *AstBuilder::getHead() {
        return heads.top();
    }

    void AstBuilder::pushHead(Node *node) {
        getHead()->add(node);
        heads.push(node);
    }

    void AstBuilder::compareAndPopHead(NodeType type) {
        if (heads.size() > 1) {
            if (heads.top()->type != type)
                while (heads.size() > 1 && heads.top()->type != type && heads.top()->shape != NodeShape::SuperBlock)
                    heads.pop();
            else if (heads.top()->shape == NodeShape::Line)
                heads.pop();
        }
    }

    void AstBuilder::compareAndPopHead(NodeType type, size_t newlineSize) {
        if (heads.size() > 1) {
            if (heads.top()->type != type)
                while (heads.size() > 1 && heads.top()->type != type && heads.top()->shape != NodeShape::SuperBlock)
                    heads.pop();
            else if (newlineSize > 1 || heads.top()->shape == NodeShape::Line)
                heads.pop();
        }

    }

    void AstBuilder::popHead() {
        if (heads.size() > 1)
            heads.pop();
    }

    void AstBuilder::pairFHead(NodeType type) {
        if (getHead()->type == type)
            popHead();
        else
            pushHead(new Node(type));

        skip();
    }

    bool AstBuilder::isValid() {
        return tokenIndex < tokenSize;
    }

    Token &AstBuilder::getCurrentToken() {
        return lexer.getToken(tokenIndex);
    }

    Token &AstBuilder::getNextToken(size_t offset) {
        return tokenIndex + offset < tokenSize ? lexer.getToken(tokenIndex + offset) : emptyToken;
    }

    void AstBuilder::skip(size_t offset) {
        tokenIndex += offset;
    }

}
