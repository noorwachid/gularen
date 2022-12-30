#pragma once

#include "Core.h"

namespace Gularen 
{
    struct DocumentNode: Node
    {
        DocumentNode();

		DocumentNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct ParagraphNode: Node
    {
        ParagraphNode();

		ParagraphNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct TextNode: Node
    {
        String content;

        TextNode(const String& content);
        
        virtual String ToString() const override;
    };
}