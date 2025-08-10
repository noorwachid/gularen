# Footnote
It depends on the publication platform.
In printed document footnote should be included at the end of page.
In html footnote should be included at the end of section.
In epub footnote should be inlined.

```gularen 
Here's a simple footnote,^(This is the first footnote) and here's a another one.^(Very important part)
```
```html
<p>Here's a simple footnote,<sup><a class="footnote" href="Footnote-1">1</a></sup>
and here's a another one.<sup><a class="footnote" href="Footnote-2">2</a></sup></p>

<div class="footnote-description">
	<div id="Footnote-1"><sup>1</sup> This is the first footnote</div>
	<div id="Footnote-2"><sup>2</sup> Very important part</div>
</div>
```

