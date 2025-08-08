# Reference

## Mention
```gularen
^[12 Rules] ^[Lateral Thinking] ^[Good Omens] ^[My Lady Jane] ^[Polar Bear]
```
```html
<p>
    <a class="mention" href="#Reference-12-Rules">(Peterson, 2018)</a>
    <a class="mention" href="#Reference-Lateral-Thinking">(de Bono, 1970)</a>
    <a class="mention" href="#Reference-Good-Omens">(Pratchett & Gaiman, 1990)</a>
    <a class="mention" href="#Reference-My-Lady-Jane">(Hand et al., 2016)</a>
    <a class="mention" href="#Reference-Polar-Bear">Figure 1</a>
</p>
```

## Bibliography
```gularen
% bibliography {
    id: "12 Rules"
    type: "Book"
    title: "12 Rules of Life"
    author: "Jordan Bernt Peterson"
    year: "2018"
    publisher: "Random House Canada"
}

% bibliography {
    id: "Lateral Thinking"
    type: "Book"
    title: "Lateral Thinking: Creativity Step by Step"
    author: "Edward de Bono"
    year: "1970"
    publisher: "Harper & Row"
}

% bibliography {
    id: "Good Omens"
    type: "Book"
    title: "Good Omens: The Nice and Accurate Prophecies of Agnes Nutter, Witch"
    authors: "Terry Pratchett, Neil Gaiman"
    year: "1990"
    publisher: "Gollancz"
}

% bibliography {
    id: "My Lady Jane"
    type: "Book"
    title: "My Lady Jane"
    authors: "Cynthia Hand, Brodi Ashton, Jodi Meadows"
    year: "2016"
    publisher: "HarperCollins"
}
```
```html
<div class="reference" id="Reference-12-Rules">Peterson, J.B., (2018). <i>12 Rules of Life</i>. Random House Canada.</div>
<div class="reference" id="Reference-Lateral-Thinking">de Bono, E., (1970). <i>Lateral Thinking: Creativity Step by Step</i>. Harper & Row.</div>
<div class="reference" id="Reference-Good-Omens">Pratchett, T. & Gaiman, N. (1990). <i>Good Omens: The Nice and Accurate Prophecies of Agnes Nutter, Witch</i>. Gollancz .</div>
<div class="reference" id="Reference-My-Lady-Jane">Hand, C., Ashton, B. & Meadows, J. (2016). <i>My Lady Jane</i>. HarperCollins.</div>
```

## Figures
```gularen
% figure {
    id: "Polar Bear"
    image: "Polar Bear.png"
    description: "Polar bear can see 5 minutes into the future"
}
```
```html
<figure id="Reference-Polar-Bear">
    <img src="Polar Bear.png">
    <figcaption>
        Polar bear can see 5 minutes into the future
    </figcaption>
<figure>
```

