#pragma once

#include <string>

namespace Gularen {

enum struct NodeType
{
    Unknown = -1,
    Root,

    // Abstract types
    Newline,
    Indent,

    LineBreak,
    ThematicBreak,
    PageBreak,     // PDF or paper like only documents

    // Semiabstract types
    Title,
    Part,
    Chapter,
    Section,
    Subsection,
    Subsubsection,
    Minisection,
    Paragraph,

    InlineCode,
    Code,
    Table,
    TableRow,
    TableColumn,

    Symbol,
    Curtain,
    Wrapper,

    // Value types
    Text,
    QuotedText,
    RawText,
    Link,
    LocalLink,

    InlineImage,
    Image,

    // Font style types
    FBold,
    FItalic,
    FMonospace,

    // List types
    List,
    NList,
    Item,
    CheckList,
    CheckItem,

};

enum struct NodeShape {
    Unknown = -1,
    Line,
    InBetween,
    Block,
    SuperBlock,
};

std::string ToString(NodeType type);

}

