# Contributors

## Project Structures

### `binding`
Contains code for language bindings. Currently, it only includes JavaScript binding.

### `editor`
Code for the live-demo website.

### `cli`
Code for the command line interface.

### `source`
- `Gularen/`: Publicly available APIs
- `Gularen/Internal/`: Should not be used in consumer source code.

### `spec`
Specification documents.

### `test`
Test documents.

## Naming Convention
- Use a tab for indentation.
- Always attach braces on the same line.
- `macro` should be written in `SCREAMING_SNAKE_CASE`.
- `namespace`, `class`, and `enum` should be written in `PascalCase`.
- `function`, `variable`, `enum.field`, and others should be written in `camelCase`.

## Unit Test
Assuming you are using Linux or MacOS:

Create a `build` directory and build the project; you will get the `gularen-test` executable.
Run `sh test/run.sh` to ensure all tests pass.
