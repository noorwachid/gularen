#pragma once

#include <string>

namespace Gularen {

enum struct NodeGroup
{

};

enum struct NodeType
{
    Unknown,
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
    Section,
    Subsection,
    Subsubsection,
    Minisection,
    Paragraph,

    // Value types
    Text,
    Link,

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

std::string ToString(NodeType type);

}

