# Link

## Net Link
```gularen
[http://google.com]
```
```html
<p><a href="http://google.com">http://google.com</a></p>
```

### With Label
```gularen
[http://google.com](Google)
```
```html
<p><a href="http://google.com">Google</a></p>
```

## Quoted Source Link
Prefix `\` to escape `"` and `\` e.g. `\"` or `\\`
```gularen
["http://somewebsite.com/product?sort[price]=cheapest&sort[timeline]=latest"](Cheapest product)
```
```html
<a href="http://somewebsite.com/product?sort[price]=cheapest&sort[timeline]=latest">Cheapest product</a>
```

## Heading Link
```gularen
[>Laminal Flow]
```
```html
<p><a href="#Laminar-Flow">Laminar Flow</a></p>
```

### With Label
```gularen
[>Laminal Flow](see this segment)
```
```html
<p><a href="#Laminar-Flow">see this segment</a></p>
```

## Local Link
```gularen
[Another Document.gr]
```
```html
<p><a href="Another Document.gr">Another Document</a></p>
```

### With Label
```gularen
[Another Document.gr](see this document)
```
```html
<p><a href="Another Document.gr">see this document</a></p>
```

### With Specified Heading Title
```gularen
[Another Document.gr>Legal Section]
```
```html
<p><a href="Another%20Document.gr#Legal-Section">Another Document Legal Section</a></p>
```

### With Specified Heading Title and Label
```gularen
[Another Document.gr>Legal Section](see this section)
```
```html
<p><a href="Another%20Document.gr#Legal-Section">see this section</a></p>
```
