# Annotation
## Document Metadata
First annotation at the begining of document belongs to `Document` node.

``` regex
!* [a-zA-Z-0-9-]+\s*=\s*.*
```
``` gr
!* title        = Book Title
!* author       = Author Name
!* created-at   = 2021-01-10
!* published-at = 2024-01-03
!* category     = Science-Fiction
```

## Entity Annotation
``` gr
!* text-alignment = center
A paragraph annotation
```
``` html
<p class="text-alignment--center">A paragraph annotation</p>
```


