# Contributors

## Project Structures

### `cli`
Code for the command line interface.

### `example`
Runtime example of Gularen documents.

### `source`
- `Gularen/Library`: Data-structures and algorithms used by Gularen.
- `Gularen/Frontend`: Parsing code, from Gularen document to AST.
- `Gularen/Backend`: Composing code, from AST to other content-type.

### `spec`
Specification documents.

### `spec-draft`
Specification draft documents.

### `resource`
resource files.

### `test`
Test files.

## Naming Convention
- Use a tab for indentation.
- Always attach braces on the same line.
- `macro` should be written in `SCREAMING_SNAKE_CASE`.
- `namespace`, `class`, and `enum` should be written in `PascalCase`.
- `function`, `variable`, `enum.field`, and others should be written in `camelCase`.

## Unit Test
Assuming you are using Linux or MacOS:

Run `sh test/build.sh`, you will get the `build/gularen-test` executable.
Run `sh test/run.sh` to ensure all tests pass.
