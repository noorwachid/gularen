# Printed Document
Additional syntax for printed document for navigating pages

## Outlines
It's commonly known as table of contents page
```gularen
>>> Table of Contents
% outlines
```
```html
<div class="outlines">
    <ul>
        <li>
            <b>Table of Contents</b>
            <a href="#page-1">1</a>
        </li>
        <li>
            <b>Chapter 1</b>
            <a href="#page-2">2</a>
        </li>
        <li>
            <b>Chapter 2</b>
            <a href="#pageud36">36</a>
        </li>
    </ul>
</div>
```

## Keywords
It's commonly known as an entry in index page.
It will query all of the pages contain the specified keyword
```gularen
% keyword Mythology
```
```html
<div class="keyword">
    <b>Mythology</b>
    <span>
        <a href="#page-43">43</a>
        <a href="#page-59">59</a>
    </span>
</div>
```
