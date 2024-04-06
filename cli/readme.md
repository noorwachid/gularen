# Gularen CLI

## Installation
### Linux and MacOS
Run this command at the root of the project directory 
```sh
sh install.sh
```

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
gularen to html cli/resource/html/article.gr
```

Transpile gularen file to HTML with custom HTML template:

**Article**:
```sh
gularen to html --template cli/resource/html/article.template.html cli/resource/html/article.gr > article.html
```

<img width="1436" alt="Screenshot 2024-04-05 at 18 37 41" src="https://github.com/noorwachid/gularen/assets/42460975/b265c5ac-b220-4021-9370-306e0641ff67">

[Document file](resource/html/article.gr) -
[Template file](resource/html/article.template.html)

**Kanban**:
```sh
gularen to html --template cli/resource/html/kanban.template.html cli/resource/html/kanban.gr > kanban.html
```

<img width="1435" alt="Screenshot 2024-04-05 at 17 04 04" src="https://github.com/noorwachid/gularen/assets/42460975/efdcb478-f217-4d66-8eab-b622ff7f251d">

[Document file](resource/html/kanban.gr) -
[Template file](resource/html/kanban.template.html)

In the template file you can add special comments for substitution.
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
