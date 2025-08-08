# Footnote
Footnote description automatically injected in the correct page.

In html footnote should be included before end of section.

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
