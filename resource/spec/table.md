# Table 

## Table Data Only

```gr
| 1 | 2 |
| 3 | 4 |
```
```html
<table>
	<tr>
		<td>1</td>
		<td>2</td>
	</tr>
	<tr>
		<td>3</td>
		<td>4</td>
	</tr>
</table>
```

## Table With Header

```gr
| ID | Name   |
|----|--------|
|  1 | Name 1 |
|  2 | Name 2 |
```
```html
<table>
	<tr>
		<th>ID</th>
		<th>Name</th>
	</tr>
	<tr>
		<td>1</td>
		<td>Name 1</td>
	</tr>
	<tr>
		<td>2</td>
		<td>Name 2</td>
	</tr>
</table>
```

## Table With Header And Alignment

```gr
| Left | Center | Right |
|:-----|:------:|------:|
| 1    |  Ada   |    19 |
| 2    | Fowler |    23 |
```
```html
<table>
	<tr>
		<th class="text-left">Left</th>
		<th class="text-left">Center</th>
		<th class="text-left">Right</th>
	</tr>
	<tr>
		<td class="text-center">1</td>
		<td class="text-center">Ada</td>
		<td class="text-center">19</td>
	</tr>
	<tr>
		<td class="text-right">2</td>
		<td class="text-right">Fowler</td>
		<td class="text-right">23</td>
	</tr>
</table>
```

## Table With Header And Footer
Footer only works if the header is defined.

```gr
| ID | Name   |
|----|--------|
|  1 | Name 1 |
|  2 | Name 2 |
|----|--------|
| ID | Name   |
```
```html
<table>
	<tr>
		<th>ID</th>
		<th>Name</th>
	</tr>
	<tr>
		<td>1</td>
		<td>Name 1</td>
	</tr>
	<tr>
		<td>2</td>
		<td>Name 2</td>
	</tr>
	<tr>
		<th>ID</th>
		<th>Name</th>
	</tr>
</table>
```
