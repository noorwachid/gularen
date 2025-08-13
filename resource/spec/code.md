# Code

## Block Code
Anything between three or more dashes will be considered as code.
And make sure to have same dashes count between opening and ending.

### Anonymous Block
````gularen
```
plain text or do fancy syntax detection here
```
````
```html
<pre><code>plain text or do fancy syntax detection here</code></pre>
```

### Labeled Block
````gularen
``` cpp
#include <iostream>

int main() 
{
	std::cout << "Hello World\n";

	return 0;
}
```
````
```html
<pre><code class="language-cpp">#include <iostream>

int main() 
{
	std::cout &lt;&lt; "Hello World\n";

	return 0;
}</code></pre>
```

## Inline Code

### Anonymous Inline
```gularen
Why do we use `console.log('here')` instead of the majestic breakpoint?
```
```html
<p>Why do we use <code>console.log('here')</code> instead of the majestic breakpoint?</p>
```

### Labeled Inline
```gularen
Type `python``print('hello world')` into the python console to print hello world.
```
```html
<p>Type <code class="language-python">print('hello world')</code> into the python console to print hello world.</p>
```

## View Hint
Add `!` before language code to view the visual output
```gularen
Let's say `!latex``\pi = 3` then the building is likely collapsed.
```
```html
<p>Let's say <span class="view language-latex">\pi = 3</span> then the building is likely collapsed.</p>
```

````gularen
``` !latex
\sum_{x=1}^{5} x
```
````
```html
<div class="view language-latex">\sum_{x=1}^{5} x</div>
```
