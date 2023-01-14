#include "NodeWriter.h"
#include <iostream>

namespace Gularen
{
    void NodeWriter::Write(const RC<Node>& parentNode, UintSize depth)
    {
        if (!parentNode)
            return;

        for (std::size_t i = 0; i < depth; ++i)
            std::cout << "    ";

        std::cout << parentNode->ToString() << "\n";

        for (const RC<Node>& childNode : parentNode->children)
            Write(childNode, depth + 1);
    }
}
