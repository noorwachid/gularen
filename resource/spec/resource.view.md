# View
Depends on the target ability to view the resource.
Like tsv can be presented as table or code block or a link to the resource.

## Block View
If the `View` is the only child of a `Paragraph` the view is promoted to block (The `Paragraph` node is removed)

## Assets

### Image
```gularen
![cat.png]
```
```html
<img src="cat.png">
```

## URL
```gularen
![https://www.youtube.com/watch?v=dQw4w9WgXcQ]
```
