# Gularen
A sweet-spot in markup languages

## Overview

<img width="1423" alt="Overview" src="https://github.com/noorwachid/gularen/assets/42460975/0cd74172-1790-4e01-9227-29e7dc20dbb9">

## Live Demo
[Right over here](https://noorwach.id/gularen-editor/)

## CLI & Installation
- [Rendering to HTML & Markdown](cli/readme.md)
- [Rendering to PDF](https://github.com/noorwachid/gularen-pdf)

## Specification & Examples
[Right over here](spec.md)

## Editor Support
- [VS Code](https://marketplace.visualstudio.com/items?itemName=nwachid.gularen)
- [Vim](https://github.com/noorwachid/vim-gularen)
- [Neovim](https://github.com/noorwachid/nvim-gularen)
- [Tree Sitter](https://github.com/noorwachid/tree-sitter-gularen)

## Why Not Use Markdown?
1. **Shorter**, create bold text with a single asterisk: `*bold*`.
2. **Consistency**, Every expression of Gularen only perform one action, and there is no alternative syntax.
3. **Better order and default**, In the image expression, the URL is the first argument `![https://i.imgflip.com/7r6vdk.jpg]`. You don't even have to write the label.
4. **Comment**, Anything after `~` in `now you see me ~ now you don't` will not be rendered.
5. **Secure**, You dont have to worry about inline HTML, because Gularen does not support inline HTML.

I drew most of my inspiration from AsciiDoc and Markdown, so there aren't any drastic changes to the syntax and structures.

## Contributors
[Please read](contributor.md)

## Changelog
### Version 1.2.0
**2024-03-19**

Breaking changes:
- Block view:
  If the `View` is the only child of a `Paragraph` the view is promoted to block (The `Paragraph` node is removed)

### Version 1.1.0
**2024-03-18**

Breaking changes:
- Moving from `C++11` to `C++17`
- Replacing homebrew template library with `stl`

### Version 1.0.0
**2024-03-13**

New features:
- Highlight:
  Highlight syntax: `=highlighted sentence.=`

- Tag:
  - Account tag syntax: `@mention`
  - Hash tag sytax: `#someTopic`

- Annotation:
  - Annotation syntax: `~~ key: value`

- Citation
  - Citation style configuration (with annotation): `~~ citation-style: apa`
  - Citation syntax: `^[title-year]`
  - Reference syntax: 
    ```
    ^[title-year]:
      title: Some Book,
      author: Some Author,
      year: 2001
    ```
- Definition list:
  Definition syntax: `Stocks :: 10 pcs`

Breaking changes:
- Check list:
  - Cancelled state is removed
  - Done syntax: `[x] done`
- Footnote syntax:
  From `^[1]` with `=[1] description` to `^(description)`
  Numbering is autogenerated and placed in the correct page (PDF and page based document)
- Inline code
  - Anonymous syntax: 
    ```
    `console.log('hello world')`
    ```
  - Labeled syntax:
    ```
    `js``console.log`
    ```
- Monospaced style is removed

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


