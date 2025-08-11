# Code

## Block Code
Anything between three or more dashes will be considered as code.
And make sure to have same dashes count between opening and ending.

### Anonymous Block
````gr
```
plain text or do fancy syntax detection here
```
````
```html
<pre><code>plain text or do fancy syntax detection here</code></pre>
```

### Labeled Block
````gr
```cpp
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
```gr
Why do we use `console.log('here')` instead of the majestic breakpoint?
```
````html
<p>Why do we use <code>console.log('here')</code> instead of the majestic breakpoint?</p>
```

### Labeled Inline
```gr
Let's say `latex``\pi = 3` then the building is likely collapsed.
```
```html
<p>Let's say <code class="language-latex">\pi = 3</code> then the building is likely collapsed.</p>
```
