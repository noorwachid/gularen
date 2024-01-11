# Gularen
A sweet-spot of markup language

## Overview
<img width="352" alt="image" src="https://github.com/noorwachid/gularen/assets/42460975/62d3889b-dcd8-4445-ac5c-fa85623c5375">

## Live Demo
[Right over here](https://noorwach.id/gularen/editor/)

## CLI
[Right over there](cli/readme.md)

## Contributors
[Please read](contributor.md)

## Editor Support
- [VS Code](https://marketplace.visualstudio.com/items?itemName=nwachid.gularen)
- [Vim](https://github.com/noorwachid/vim-gularen)

## Language Specification
- [01 Comment](spec/01-comment.gr)
- [02 Font style](spec/02-font-style.gr)
- [03 Heading](spec/03-heading.gr)
- [04 Paragraph](spec/04-paragraph.gr)
- [05 Indentation](spec/05-indentation.gr)
- [06 Breaking](spec/06-breaking.gr)
- [07 List](spec/07-list.gr)
- [08 Table](spec/08-table.gr)
- [09 Code](spec/09-code.gr)
- [0A Linker](spec/0A-linker.gr)
- [0B Presenter](spec/0B-presenter.gr)
- [0C Inclusion](spec/0C-inclusion.gr)
- [0D Footnote](spec/0D-footnote.gr)
- [0E Punctuation mark](spec/0E-punctuation-mark.gr)
- [0F Emoji](spec/0F-emoji.gr)
- [10 Blockquote](spec/10-blockquote.gr)
- [11 Admonition](spec/11-admonition.gr)
- [12 Date-time](spec/12-datetime.gr)

## Why?
The problem with Markdown was, too many ways to do something. 
And Markdown was designed in mind to be parsed with regex.
Instead of character by character scanner. 

The purpose of Gularen is to create smallest possible footprint of additional characters 
while writing visually pleasing plain text document.

## Changelog
**2023-08-05:**

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


