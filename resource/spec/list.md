# List

## Bullet List

``` gr
- Fast
- Furious
- Fit
```
``` html
<ul>
	<li>Fast</li>
	<li>Furious</li>
	<li>Fit</li>
</ul>
```

## Numbered List

``` gr
1. One
2. Two 
3. Three
```
``` html
<ol>
	<li>One</li>
	<li>Two</li>
	<li>Three</li>
</ol>
```

## Nested List

``` gr
- One 
- Two 
	- Two sub 1
	- Two sub 2
		- Two sub 2 section A 
		- Two sub 2 section B
	- Two sub 3
```
``` html
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

## List With Description
``` gr
- First 
- Second 
- Third 
	+1 level indentation after current list will create the body of the list.
```
``` html
<ul>
	<li>First</li>
	<li>Second</li>
	<li>Third
		<p>+1 level indentation after current list will create the body of the list.</p>
	</li>
</ul>
```
