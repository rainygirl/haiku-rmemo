# R Memo for Haiku OS

[한국어](README.ko.md)

Notes that sit on the desktop and look like notes. Each one is a borderless window that draws itself — a coloured sheet with a header strip and a folded bottom-right corner.

![Three notes on the Haiku desktop](screenshots/notes.png)

## Requirements

Haiku (x86 or x86_64). Build it on the machine you are going to run it on — no cross-compiler needed.

## Install

```sh
./install.sh
```

This compiles it, puts the binary in `~/config/non-packaged/apps/`, adds it to **Deskbar → Applications**, and puts a link on the Desktop.

```sh
./install.sh --build-only   # compile in place, install nothing
./install.sh --uninstall    # remove it again
```

## Using it

- **The header is the drag handle.** Grab it anywhere to move the note.
- **The folded corner is the resize grip.** Pull it to change the size.
- **The swatch on the left opens the colour menu** — 7 colours.
- **`+` makes a new note**, cascaded down-right of the current one.
- **`×` throws the note away.** If it has anything written on it you are asked first.
- **Closing the last note quits the application**, and launching with nothing saved opens one empty note.

Notes are saved on quit to `~/config/settings/RMemo/notes` and restored with their text, position, size and colour.

The colour menu, the confirmation dialog and the button tooltips follow the system language: English, Korean, Japanese, Simplified and Traditional Chinese, Russian, Turkish, Indonesian, Polish, Hungarian, German, French, Italian, Spanish and Vietnamese.

## License

MIT

## AI disclosure

This program was written with Claude.
