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

## Heading Link

```gularen
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
In textbook often each chapters have the same subsection name like "Exercises".
The backend has to address this issues. Either by always prefixing the section with chapter or
only prefixing with chapter if has subsection conflict.

For subsection conflict you can add another `>` after section.

```gularen
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

For subsubsection conflict you can add another `>` after subsection.

```gularen
[>Chapter>Subsection>Subsubsection]
```
```html
<a href="#Chapter-Subsection-Subsubsection">Chapter Subsection Subsubsection</a>
```
