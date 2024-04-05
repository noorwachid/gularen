# Gularen CLI

## Installation
### Linux and MacOS
- Run this command on the root of the project directory `sh install.sh`

### Windows
I do not have windows machine, 
but the code does not use platform specific features.
You should be able to compile it on any platform that support `C++17`.

## Manual
### Display help page
```sh
gularen help
```

### Transpile to HTML
```sh
gularen to html document.gr
```

Transpile gularen file to HTML with custom HTML template:
```sh
gularen to html --template resource/html/template/article.html document.gr
```

In the `article.html` file you can add special comments for substitution.
- `<!--[content]-->`: the document content
- `<!--[toc]-->`: the document table of contents
- `<!--[[annotation-key]]-->`: document annotation value of `~~ key: value`

### Transpile to GFM (Github Flavored Markdown)
```sh
gularen to gfm document.gr
```

### Transpile to JSON
```sh
gularen to json document.gr
```
