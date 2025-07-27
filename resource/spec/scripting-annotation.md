# Annotation
## Document Metadata
First annotation at the begining of document belongs to `Document` node.

``` regex
% [a-zA-Z-0-9-]:\s.*
```
``` gr
% title: Book Title
% author: Author Name
% created-at: 2021-01-10
% published-at: 2024-01-03
% category: Science-Fiction
```

## Entity Annotation
``` gr
% text-alignment: center
A paragraph annotation
```
``` html
<p class="text-alignment--center">A paragraph annotation</p>
```

## Table-Span Annotation
```gr
| A | B | C |
% row-span: 2 2
| D | E | F |
% column-span: 0 2
| G |
```
```html
<table>
    <tr>
        <td>A</td>
        <td>B</td>
        <td>C</td>
    </tr>
    <tr>
        <td>D</td>
        <td>E</td>
        <td rowspan="2">F</td>
    </tr>
    <tr>
        <td colspan="2">G</td>
    </tr>
</table>
```
