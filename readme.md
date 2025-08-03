# Gularen
A sweet-spot in markup languages

## Overview
<img width="1440" alt="Screenshot 2024-05-04 at 18 20 30" src="https://github.com/noorwachid/gularen/assets/42460975/58b102f8-2c7c-45c3-b330-828bdc8d0787">

## Live Demo
[Try on web browser](https://noorwach.id/gularen-web/)

## Installation
> [!NOTE]
> I only test it on my linux machine
- You need compiler `gcc` or `clang` that support C++17
- Run `sudo make install`

## Usage
- `gularen document.gularen`: generate html to `stdout`
- `gularen -e document.gularen`: generate embedded html (without head and body tag)
- `gularen -o index.html document.gularen`: generate index.html instead
- `gularen -h`: for help

## Editor Support
- [Neovim](https://github.com/noorwachid/nvim-gularen)

## Why Not Use Markdown?
1. **Shorter**, create strong text with a single asterisk: `*bold*`.
2. **Consistency**, Every expression of Gularen only perform one action, and there is no alternative syntax.
3. **Better order and default**, In the link expression, the URL is the first argument `[https://i.imgflip.com/7r6vdk.jpg]`. You don't even have to write the label.
4. **Comment**, Anything after `# ` (hash-space) in `now you see me # now you don't` will not be rendered.
5. **Secure**, You don't have to worry about inline HTML, because Gularen does not support inline HTML.

I drew most of my inspiration from Python, AsciiDoc and Markdown, so there aren't any drastic changes to the syntax and structures.

