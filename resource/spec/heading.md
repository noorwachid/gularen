# Heading

## Heading Title
```gularen
>>> Section

>> Subsection

> Subsubsection
```
```html
<section class="section">
	<h1 id="Section">Section</h1>

	<section class="subsection">
		<h2 id="Subsection">Subsection</h2>

		<section class="subsubsection">
			<h3 id="Subsubsection">Subsubsection</h3>
		</section>
	</section>
</section>
```

## Heading Subtitle
If the subsection modifier is placed in the next line of any title. It will be parsed as subtitle.

```gularen
>>> Section: Section Subtitle

>> Subsection: Subsection Subtitle

> Subsubsection: Subsubsection Subtitle
```
```html
<section class="section">
	<h1 id="Section">Section <small>Section Subtitle</small></h1>

	<section class="subsection">
		<h2 id="Subsection">Subsection <small>Subsection Subtitle</small></h2>

		<section class="subsubsection">
			<h3 id="Subsubsection">Subsubsection <small>Subsubsection Subtitle</small><h3>
		</section>
	</section>
</section>
```

## Mention
```gularen
See the ^[Section 1]
```
```html
<p>See the <a href="#Section-1"></a></p>
```
