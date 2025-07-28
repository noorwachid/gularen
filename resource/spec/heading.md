# Heading

## Heading Title
```gr
>>> Chapter

>> Section

> Subsection
```
```html
<section class="chapter">
	<h1 id="Chapter">Chapter</h1>

	<section class="section">
		<h2 id="Section">Section</h2>

		<section class="subsection">
			<h3 id="Subsection">Subsection</h3>
		</section>
	</section>
</section>
```

## Heading Subtitle
If the subsection modifier is placed in the next line of any title. It will be parsed as subtitle.

```gr
>>> Chapter: Chapter Subtitle

>> Section: Section Subtitle

> Subsection: Subsection Subtitle
```
```html
<section class="chapter">
	<h1 id="Chapter">Chapter <small>Chapter Subtitle</small></h1>

	<section class="section">
		<h2 id="Section">Section <small>Section Subtitle</small></h2>

		<section class="subsection">
			<h3 id="Subsection">Subsection <small>Subsection Subtitle</small><h3>
		</section>
	</section>
</section>
```

## Heading Link

```gr
>>> Laminal Flow

See [>Laminal Flow]

If on another file [Chapter 1.gr>Laminal Flow]
```
```html
<section class="section">
	<h1 id="Laminar-Flow">Laminar Flow</h1>

	<p>See <a href="#Laminar-Flow">Laminal Flow</a></p>

	<p>If on another file <a href="Chapter 1.gr#Laminal-Flow">Chapter 1.gr Laminal Flow</a></p>
</section>
```

## Conflicting Section Link
In textbook often each chapters have the same section name like "Exercises".
The backend has to address this issues. Either by always prefixing the section with chapter or
only prefixing with chapter if has section conflict.

For section conflict you can add another `>` after section.

```gr
>>> Chapter 1
>> Exercises

>>> Chapter 2
>> Exercises

[>Chapter 1>Exercises]
[>Chapter 2>Exercises]
```
```html
<h1 id="Chapter-1">Chapter 1</h1>
<h2 id="Chapter-1-Execises">Exercises</h2>

<h1 id="Chapter-2">Chapter 2</h1>
<h2 id="Chapter-2-Execises">Exercises</h2>

<a href="#Chapter-1-Execises">Chapter 1 Exercises</a>
<a href="#Chapter-2-Execises">Chapter 2 Exercises</a>
```

For subsection conflict you can add another `>` after section.

```gr
[>Chapter>Section>Subsection]
```
```html
<a href="#Chapter-Section-Subsection">Chapter Section Subsection</a>
```
