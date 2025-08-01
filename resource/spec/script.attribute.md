# Attribute
## Document Metadata
First attribute at the begining of document belongs to `Document` node.

```regex
% [a-zA-Z-0-9-]:\s.*
```
```gularen
% title: Book Title
% author: Author Name
% created-at: 2021-01-10
% published-at: 2024-01-03
% category: Science-Fiction
```

## Entity Atribute
```gularen
% text-alignment: center
A paragraph attribute
```
```html
<p class="text-alignment--center">A paragraph attribute</p>
```
