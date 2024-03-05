# Gularen
A sweet-spot of markup language

## Overview
<img width="1426" alt="Screenshot 2024-03-05 at 17 26 30" src="https://github.com/noorwachid/gularen/assets/42460975/d90f21f9-12e0-4d91-ab1e-23e29d9911c8">

## Live Demo
[Right over here](https://noorwach.id/gularen/editor/)

## CLI
[Right over there](cli/readme.md)

## Contributors
[Please read](contributor.md)

## Editor Support
- [VS Code](https://marketplace.visualstudio.com/items?itemName=nwachid.gularen)
- [Vim](https://github.com/noorwachid/vim-gularen)
- [Tree Sitter](https://github.com/noorwachid/tree-sitter-gularen)

## Why Not Use Markdown?
**Shorter**, create bold text with a single asterisk: `*bold*`.

**Consistency**, Every expression of Gularen only perform one action, and there is no alternative syntax.

**Better order and default**, In the image expression, the URL is the first argument `![https://i.imgflip.com/7r6vdk.jpg]`. You don't even have to write the label.

**Comment**, Anything after `~` in `now you see me ~ now you don't` will not be rendered.

I drew most of my inspiration from AsciiDoc and Markdown, so there aren't any drastic changes to the syntax and structures.

## Language Specification
- [01 Comment](spec/01-comment.gr)
- [02 Style](spec/02-style.gr)
- [03 Heading](spec/03-heading.gr)
- [04 Paragraph](spec/04-paragraph.gr)
- [05 Indentation](spec/05-indentation.gr)
- [06 Breaking](spec/06-breaking.gr)
- [07 List](spec/07-list.gr)
- [08 Table](spec/08-table.gr)
- [09 Code](spec/09-code.gr)
- [0A Linker](spec/0A-linker.gr)
- [0B Viewer](spec/0B-viewer.gr)
- [0C Inclusion](spec/0C-inclusion.gr)
- [0D Footnote](spec/0D-footnote.gr)
- [0E Punctuation](spec/0E-punctuation.gr)
- [0F Emoji](spec/0F-emoji.gr)
- [10 Blockquote](spec/10-blockquote.gr)
- [11 Admonition](spec/11-admonition.gr)
- [12 Date-time](spec/12-datetime.gr)

## Changelog
**2024-03-01**:

Breaking changes:
- Admonition label will be writen as is

**2024-02-29**:
Breaking changes:
- Removing heading ID `Heading > ID`

**2023-08-05**:

New features:
- Date-time marker:
    - Date: `<2018-06-12>`
    - Time: `<10:00>` or `<10:00:12>`
    - Date-time: `<2018-06-12 10:00>` or `<2018-06-12 10:00:12>`

Breaking changes:
- Line-break syntax: from `<` into `<<`.
- Page-break syntax: from `<<` into `<<<`.
- Admonition syntax:
    - Tip: from `<+>` to `<tip>`.
    - Note: from `</>` to `<note>`.
    - Hint: from `<?>` to `<hint>`.
    - Reference: from `<&>` to `<reference>`.
    - Important: from `<!>` to `<important>`.
    - Warning: from `<^>` to `<warning>`.


