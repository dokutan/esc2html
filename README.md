# esc2html
Convert escape sequences to html

Design goals and features:
- Convert a reasonable subset of ANSI standard and nonstandard [escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code) to html
- Optionally include [Sixel](https://en.wikipedia.org/wiki/Sixel) graphics
- Filter out sequences that have no appropriate representation in html, e.g. cursor movement
- See ``test.ansi`` and ``test.sixel`` for a list of currently supported sequences
- This is **not** a terminal emulator

## Installation
- Clone this repository
- Make sure that you have a C++20 capable compiler installed, then type
```
make
sudo make install
```

## Usage
Input is read from stdin, output is written to stdout. Use the ``-h`` option for a list of available options.

Basic example:
```
cat test.ansi test.sixel | esc2html > test.html
```

Use the ``-s`` option to convert sixel graphics, requires imagemagick ``convert`` and ``base64``:
```
cat test.ansi test.sixel | esc2html -s > test.html
```

Use the ``-t`` and ``-f`` options to set the document title and font:
```
esc2html -t title -f monospace < test.ansi > test.html
```
## Contributing
Your pull requests are welcome.

## License
GNU GPLv3 or later, see LICENSE.
