# Contributors

## Project Structures
### `binding`
Other language binding code, right now it only has JavaScript binding.

### `editor`
Live-demo website code.

### `source`
- `Gularen/`, publicly available APIs
- `Gularen/Internal/`, should not be used in consumer source code.

### `spec`
Specification documents.

### `test`
Test units.

## Naming Convention
- Tab as indentation
- Always attach braces on the same line
- `macro` has to be written in `SCREAMING_SNAKE_CASE`
- `namespace`, `class` and `enum` has to be written in `PascalCase`
- `function`, `variable`, `enum.field` and others has to be written in `camelCase`

## Unit Test
I assume you are using Linux or MacOS.

Create `build` directory and build the project, you will get `gularen-test` executable.
Run `sh test/run.sh` make sure all tests are passed.
