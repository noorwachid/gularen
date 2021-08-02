# Gularen
Well-thought lightweight markup language that actually make sense

### Examples
``` gr
>>> C++: The Pain, The Pleasure and The Migrane

>>--> Chapter 1: Data Types

This section talk about integers.
This is still the same paragraph.

Now its different paragraph.

And the most important features. ^ Comments, seriously even HTML has one.

Font formating is a little bit different from Markdown goes like this.
*Asterisks for bold*, _underlines for italic_ and `backticks for monospaced`.
*Combine them _all_ `together`* with grace.

And the lists:
1. Using numbers
2. So on
3. And so on
.. Or you tired counting you can use two dots
.. It automatically counted for you

- And hyphen for bullet list
- You can easyly create nested list by adding 4 spaces each levels
    - I hate Python but this is nice
    - You can have infinite levels of indentations

And of course the checklist or todo-list or what ever you name it

[+] This one is checked
[ ] This one is not

```

Will be parsed as:
```
Root:
    Title:
        Text: "C++: The Pain, The Pleasure and The Migrane"
    Chapter:
        Text: "Chapter 1: Data Types"
    Paragraph:
        Text: "This section talk about integers."
        Newline:
        Text: "This is still the same paragraph."
        Newline:
        Text: "Now its different paragraph."
        Newline:
        Text: "And the most important features. "
        Newline:
        Text: "Font formating is a little bit different from Markdown goes like this."
        FBold:
            Text: "Asterisks for bold"
        Text: ", "
        FItalic:
            Text: "underlines for italic"
        Text: " and "
        FMonospace:
            Text: "backticks for monospaced"
        Text: "."
        FBold:
            Text: "Combine them "
            FItalic:
                Text: "all"
            Text: " "
            FMonospace:
                Text: "together"
        Text: " with grace."
        Newline:
    Paragraph:
        Text: "And the lists:"
    NList:
        Item:
            Text: "Using numbers"
        Item:
            Text: "So on"
        Item:
            Text: "And so on"
        Item:
            Text: "Or you tired counting you can use two dots"
        Item:
            Text: "It automatically counted for you"
    List:
        Item:
            Text: "And hyphen for bullet list"
        Item:
            Text: "You can easyly create nested list by adding 4 spaces each levels"
            Indent:
                List:
                    Item:
                        Text: "I hate Python but this is nice"
                    Item:
                        Text: "You can have infinite levels of indentations"
    Paragraph:
        Text: "And of course the checklist or todo-list or what ever you name it"
    CheckList:
        CheckItem: false
            Text: "This one is checked"
        CheckItem: false
            Text: "This one is not"
```


### Author Notes
See the [Design.gr](Design.gr) for the design and the implementation progress.
