#pragma once

#include "Core.h"

namespace Gularen 
{
    struct HeadingNode: Node
    {
        String id;

        HeadingNode(NodeType type);

        HeadingNode(NodeType type, const NodeChildren& children);
        
        virtual String ToString() const override;
    };

    struct TitleNode: HeadingNode 
    {
        TitleNode();

        TitleNode(const NodeChildren& children);

        virtual String ToString() const override;
    };
    
    struct SubtitleNode: HeadingNode 
    {
        SubtitleNode();

        SubtitleNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct DocumentNode: HeadingNode 
    {
        DocumentNode();

        DocumentNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct PartNode: HeadingNode 
    {
        PartNode();

        PartNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct ChapterNode: HeadingNode 
    {
        ChapterNode();

        ChapterNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct SectionNode: HeadingNode 
    {
        SectionNode();

        SectionNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct SubsectionNode: HeadingNode 
    {
        SubsectionNode();

        SubsectionNode(const NodeChildren& children);

        virtual String ToString() const override;
    };

    struct SubsubsectionNode: HeadingNode 
    {
        SubsubsectionNode();

        SubsubsectionNode(const NodeChildren& children);
        
        virtual String ToString() const override;
    };
    
    struct SegmentNode: HeadingNode 
    {
        SegmentNode();

        SegmentNode(const NodeChildren& children);

        virtual String ToString() const override;
    };
}