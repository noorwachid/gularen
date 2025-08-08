# Link

## Net Link
```gularen
[http://google.com]
```
```html
<p><a href="http://google.com">http://google.com</a></p>
```

With label:
```gularen
[http://google.com](Google)
```
```html
<p><a href="http://google.com">Google</a></p>
```

## Local Link
```gularen
[Another Document.gr]
```
```html
<p><a href="Another Document.gr">Another Document</a></p>
```

With label:
```gularen
[Another Document.gr](see this document)
```
```html
<p><a href="Another Document.gr">see this document</a></p>
```

## Quoted Source Link
Prefix `\` to escape `"` and `\` e.g. `\"` or `\\`
```gularen
["http://somewebsite.com/product?sort[price]=cheapest&sort[timeline]=latest"](Cheapest product)
```
```html
<a href="http://somewebsite.com/product?sort[price]=cheapest&sort[timeline]=latest">Cheapest product</a>
```
