# Gularen
A sweet-spot in markup languages

## Overview
[Try on web browser](https://noorwach.id/gularen-web/)

## Installation
> [!NOTE]
> I only test it on my linux machine
- You need compiler `gcc` or `clang` that support C++17
- Run `sudo make install`

## Usage
- `gularen document.gularen`: generate html to `stdout`
- `gularen -o index.html document.gularen`: generate `index.html`
- `gularen -t resource/template/article.html -o index.html document.gularen`: generate `index.html` with `article.html` template
- `gularen -h`: for help

## HTML Templating
See the [example.html](resource/template/exapmle.html) for an example.
Gularen compiler will look into special comments and replace it with the appropriate content.
- `<!--%[metadata]-->`: generate meta tags
- `<!--%[title]-->`: generate title from metadata if exists else from first section heading
- `<!--%[content]-->`: generate the document content
- `<!--%[outline]-->`: generate the document outline (table of contents)


## Editor Support
- [Neovim](https://github.com/noorwachid/nvim-gularen)

## Why Not Use Markdown?
1. **Shorter**, create strong text with a single asterisk: `*bold*`.
2. **Consistency**, Every expression of Gularen only perform one action, and there is no alternative syntax.
3. **Better order and default**, In the link expression, the URL is the first argument `[https://i.imgflip.com/7r6vdk.jpg]`. You don't even have to write the label.
4. **Comment**, Anything after `# ` (hash-space) in `now you see me # now you don't` will not be rendered.
5. **Secure**, You don't have to worry about inline HTML, because Gularen does not support inline HTML.

I drew most of my inspiration from Python, AsciiDoc and Markdown, so there aren't any drastic changes to the syntax and structures.

