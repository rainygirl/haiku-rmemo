# R Memo for Haiku OS

한국어 버전은 [`README.ko.md`](README.ko.md) 참고 / For Korean, see [`README.ko.md`](README.ko.md).

A note-taking app for Haiku OS that sits on your desktop and looks just like real notes.

![Three notes on the Haiku desktop](screenshots/notes.png)

Each note is a borderless window that draws itself — a coloured sheet with a
header strip and a folded bottom-right corner. Nothing here uses the window
decorator, which is the whole point: a note with a title bar reads as a
dialog, not as a note.

## Behaviour

- **The header is the drag handle.** Grab it anywhere to move the note.
- **The folded corner is the resize grip.** Pull it to change the size.
- **The swatch on the left opens the colour menu** — 7 colours.
- **`+` makes a new note**, cascaded down-right of the current one.
- **`×` throws the note away.** If it has anything written on it you are asked
  first.
- **Closing the last note quits the application**, and launching with nothing
  saved opens one empty note. There is never a running instance with no
  visible window, and never a window with no way back.

Notes are saved on quit to `~/config/settings/RMemo/notes` and restored
with their text, position, size and colour.

## Requirements

Haiku OS (x86 or x86_64). Build it on the machine you are going to run it on.

## Build and install

```sh
./install.sh
./install.sh --build-only   # compile in place, install nothing
./install.sh --uninstall    # remove it again
```

`install.sh` puts the binary in `~/config/non-packaged/apps/` and symlinks it
under `~/config/non-packaged/data/deskbar/menu/Applications/`, which is what
makes it appear in **Deskbar → Applications**. Deskbar does not read
`~/config/settings/deskbar/menu` directly — it follows the search list in
`/boot/system/data/deskbar/menu_entries`, and that `non-packaged` path is the
user-writable entry in it.

## License

MIT
