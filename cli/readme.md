# Gularen CLI

## Installation
### Linux and MacOS
- Run this command on the root of the project directory `sh install.sh`

### Windows
I do not have windows machine, 
but the code does not use platform specific features.
You should be able to compile it on any platform that support `C++11`.

## Manual
**Display help page**
- `gularen help`

**Transpile gularen file to HTML**
- `gularen to html document.gr`

**Transpile gularen file to HTML with custom HTML template**
- `gularen to html --template resource/html/template/article.html document.gr`
  In the `article.html` file you can add special comments for substitution.
  - `<!--[content]-->`: the document content
  - `<!--[toc]-->`: the document table of contents
  - `<!--[[annotation-key]]-->`: document annotation value of `~~ key: value`

**Transpile gularen file to GFM (Github Flavored Markdown)**
- `gularen to gfm document.gr`

**Transpile gularen file to JSON**
- `gularen to json document.gr`

**Parse gularen file to AST**
- `gularen parse document.gr`
