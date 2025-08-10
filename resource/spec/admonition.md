# Admonition
## Single Line
```gularen
NOTE! A note
HINT! A hint
IMPORTANT! Important stuff
WARNING! Warning stuff
SEE! The Amazing Spiderman 2
TIP! A tip
```
```html
<div class="admonition note">
	<div class="label">Note</div>
	<div class="content">A note</div>
</div>
<div class="admonition hint">
	<div class="label">Hint</div>
	<div class="content">a Hint</div>
</div>
<div class="admonition important">
	<div class="label">Important</div>
	<div class="content">Important stuff</div>
</div>
<div class="admonition warning">
	<div class="label">Warning</div>
	<div class="content">Warning sfuff</div>
</div>
<div class="admonition see">
	<div class="label">See Also</div>
	<div class="content">The Amazing Spiderman 2</div>
</div>
<div class="admonition tip">
	<div class="label">Tip</div>
	<div class="content">A tip</div>
</div>
```

## Multi Line
```gularen
NOTE!
Long note.
Very long note.
```
```html
<div class="admonition note">
	<div class="label">Note</div>
	<div class="content">
		<p>Long note.
		Very long note.</p>
	</div>
</div>
```
