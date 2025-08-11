# Footnote
Footnote description automatically injected in the correct page.

In html footnote should be included before page break or section.

```gularen 
Here's a simple footnote,^(This is the first footnote.) and here's a another one.^(Very important part)
```
```html
<p>Here's a simple footnote,<sup><a href="Footnote-1">1</a></sup>
and here's a another one.<sup><a href="Footnote-2">2</a></sup></p>

<div class="footnote-desc">
	<p><sup>1</sup> This is the first footnote.</p>
	<p><sup>2</sup> Very important part</p>
</div>
```
