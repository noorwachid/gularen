# List

## Bullet List

```gularen
- Fast
- Furious
- Fit
```
```html
<ul>
	<li>Fast</li>
	<li>Furious</li>
	<li>Fit</li>
</ul>
```

## Numbered List

```gularen
1. One
2. Two 
3. Three
```
```html
<ol>
	<li>One</li>
	<li>Two</li>
	<li>Three</li>
</ol>
```

## Check List
```gularen
[ ] To-do one 
[ ] To-do two
[x] Done three
[x] Done four
```
```html
<ul class="check-list">
	<li><input type="checkbox"> To-do one</li>
	<li><input type="checkbox"> To-do two</li>
	<li><input type="checkbox" checked> Done three</li>
	<li><input type="checkbox" checked> Done four</li>
</ul>
```

## Extra

### Nested List

```gularen
- One 
- Two 
	- Two sub 1
	- Two sub 2
		- Two sub 2 section A 
		- Two sub 2 section B
	- Two sub 3
```
```html
<ul>
	<li>One</li>
	<li>Two
		<ul>
			<li>Two sub 1</li>
			<li>Two sub 2
				<ul>
					<li>Two sub 2 section A</li>
					<li>Two sub 2 section B</li>
				</ul>
			</li>
			<li>Two sub 3</li>
		</ul>
	</li>
</ul>
```

### Description List
```gularen
- First 
- Second 
- Third 
	+1 level indentation after current list will create the body of the list.
```
```html
<ul>
	<li>First</li>
	<li>Second</li>
	<li>Third
		<p>+1 level indentation after current list will create the body of the list.</p>
	</li>
</ul>
```

