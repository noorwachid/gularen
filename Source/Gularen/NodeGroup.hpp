#pragma once


namespace Gularen {

enum struct NodeGroup
{
    Unknown = -1,

    Text,

    Header,
    List,
    Item,
    Break,

    Link,
    Table,
    Code,
    Tag,

    Wrapper,
};

}
