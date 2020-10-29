# esc2html
Convert escape sequences to html

## Installation
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
cat test.ansi test.sixel | esc2html -t title -f monospace > test.html
```

## License
GNU GPLv3 or later
