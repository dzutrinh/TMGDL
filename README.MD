# TMGDL

Text Mode Game Development Library for MS-DOS, version **1.5** (build 0925).

TMGDL is a Pascal library for 80x25 colour text-mode games and tools: direct
screen access with clipping, virtual screens, ANSI art, boxes and dialogs, UI
controls (menus, lists, gauges, counters, help viewer...), a millisecond timer,
file and configuration helpers, and a simple package (resource archive) format.

Supported compilers:

- TMT Pascal 3.5+ (32-bit, default)
- Turbo/Borland Pascal 7.0 (16-bit)
- Free Pascal 3.2 for go32v2 (32-bit, needs `CWSDPMI.EXE` at run time)

## Games

| Game | Version | What it is |
| --- | --- | --- |
| [2048](#2048) | 1.4 | Slide and merge tiles to reach 2048 |
| [Arkanoid](#arkanoid) | 1.0 | Brick breaker with power-up capsules |
| [Chomp](#chomp) | 1.0 | Maze game: eat the dots, avoid four ghosts |
| [Game of Life](#game-of-life) | 1.1 | Conway's cellular automaton, with an editor |
| [Gomoku](#gomoku) | 1.0 | Five in a row against a friend or the computer |
| [Minesweeper](#minesweeper) | 1.0 | Clear the field without setting off a mine |
| [Reversi](#reversi) | 1.0 | Othello-style disc flipping against a friend or the computer |
| [Runner](#runner) | 1.0 | Lode Runner-style platforms: dig, climb and grab the gold |
| [Snake](#snake) | 1.0 | Eat apples, grow longer, dodge the obstacles |
| [Sokoban](#sokoban) | 1.0 | Push the crates onto the goals, 12 levels |
| [Solitaire](#solitaire) | 1.0 | Klondike patience, drawing one or three cards |
| [Tetris](#tetris) | 1.1 | The falling-blocks classic |
| [Tic-Tac-Toe](#tic-tac-toe) | 1.2 | Noughts and crosses against a friend or the computer |

## Tools

| Tool | Version | What it is |
| --- | --- | --- |
| [PM](#pm-package-manager) | 1.1 | Full-screen package manager, creates installer packages |
| [PCK](#pck-command-line-packer) | 1.1 | Command-line package tool |
| [SFX](#sfx-self-extractor) | 1.1 | Self-extractor stub for packages |
| [INSTALL](#install-installer) | 1.1 | Installs a package with the settings of `INSTALL.DAT` |
| [ASCII](#ascii-character-map) | - | Character map of the text-mode font |
| [SCANCODE](#scancode-key-code-viewer) | - | Shows the codes of the keys pressed |
| [VIEWBIN](#viewbin-screen-dump-viewer) | - | Shows a screen dump saved with F12 |

## Library

| Unit | Contents |
| --- | --- |
| `LIBTEXT` | Screen, clipping, virtual screens, ANSI art, boxes, dialogs, keyboard, millisecond timer |
| `LIBCTLS` | Controls: menus, lists, sliders, gauges, counters, labels, help viewer, life box, [forms](#forms) |
| `LIBUTIL` | Files, folders, drives, configuration and text files, CRC32, message boxes |
| `LIBPACK` | Package (resource archive) files: create, extract, add, delete, verify, self-extractors |
| `LIBTMGD` | Uses all of the above, plus version and compiler information |

### Forms

`LIBCTLS` has a form control: a dialog of labelled fields, edited in place.
Each field has a kind that decides what can be typed in it and how it is
checked when the user leaves it:

| Kind | Field |
| --- | --- |
| `FK_TEXT` | Any text |
| `FK_UPPER` | Text in capitals |
| `FK_NUMBER` | Whole number, within `formSetRange` |
| `FK_FILENAME` | DOS file name (8.3) |
| `FK_PATH` | Drive, folders and file name |
| `FK_DRIVE` | One drive letter |
| `FK_CHOICE` | One of a list set with `formSetChoices`, picked with the arrows, Space or its first letter |
| `FK_SWITCH` | YES or NO |
| `FK_PASSWORD` | Text shown as stars |

Fields can be required, and can have their own check: a far function that
may tidy the value up and returns `''` when it is right, or the message to
show. Up/Shift-Tab goes back, Down/Tab/Enter goes forward, F10 finishes and
Esc cancels (the values are then left as they were).

```pascal
function checkEven(var value: string): string;
begin
  checkEven := '';
  if odd(atol(value)) then checkEven := 'THE COUNT MUST BE EVEN';
end;

f := formCreate(-1, -1, 0, SHADE_BLUE, ' SETTINGS ');
formAdd(f, 'Player', FK_TEXT, 20, 'Player 1');
formAdd(f, 'Count', FK_NUMBER, 4, '10');
formAdd(f, 'Level', FK_CHOICE, 0, 'NORMAL');
formAdd(f, 'Sound', FK_SWITCH, 3, 'YES');
formSetRequired(f, 1, TRUE);
formSetRange(f, 2, 0, 100);
formSetCheck(f, 2, checkEven);
formSetChoices(f, 3, 'EASY|NORMAL|HARD');
if formExec(f) then
  writeln(formText(f, 1), formNumber(f, 2), formChoice(f, 3), formSwitch(f, 4));
formDestroy(f);
```

PM's *Create installer package* dialog and the installer's destination dialog
are built with it.

## Building

Build settings live in `SRC/MACROS`, shared by every makefile:

- `LIBROOT`/`LIBDIR`: where the compiled units are installed (default `\TMGDL\TEXTMODE\LIB`)
- `BINDIR`: where `make install` copies the games and tools (default `\TMGDL\TEXTMODE\BIN`)
- `TMTROOT`, `TP7ROOT`, `FPCROOT`: compiler locations
- `RM`, `CP`, `MV`, `MD`: file commands used by the makefiles

Every makefile works with the three compilers: TMT Pascal by default, or
Borland Pascal / Free Pascal with `make -DUSE_BPC` / `make -DUSE_FPC`. Each
folder also has a `BUILD.BAT` that takes the compiler as its argument, cleans
and rebuilds. Build and install the library first, then any game or tool,
with the same compiler:

```
cd SRC
BUILD            (TMT Pascal)
BUILD BPC        (Borland Pascal)
BUILD FPC        (Free Pascal)

cd ..\GAMES\2048
BUILD FPC        (or: make -DUSE_FPC)
make -DUSE_FPC install
```

Makefiles with a game or tool: `GAMES\2048`, `GAMES\ARKANOID`, `GAMES\CHOMP`, `GAMES\GOL`,
`GAMES\GOMOKU`, `GAMES\SNAKE`, `GAMES\SOKOBAN`, `GAMES\T3`, `GAMES\TETRIS`, `TOOLS` (ASCII, SCANCODE, VIEWBIN),
`TOOLS\PCKMGR` (PM, PCK, SFX) and `TOOLS\INSTALL`. Programs built with Free
Pascal need `CWSDPMI.EXE` (in `%FPCROOT%\BIN\GO32V2`) in the same folder or on
the `PATH`.

`make clean` in `SRC` removes the installed units. After updating TMGDL, run
`SRC\BUILD.BAT` again and remove any old copies of the units elsewhere in the
compiler's unit path: a mix of old and new units causes "unit version mismatch"
(TPC) or "can't compile unit" (FPC) errors.

The executables stored in some game folders come from earlier releases;
rebuild them to get the versions listed here. Arkanoid, Chomp, Gomoku, Snake
and Sokoban ship as source only: run `BUILD` in their folders.

## Games in detail

### 2048

Slide the tiles of a 4 x 4 board; two equal tiles that meet merge into one of double value. Reach the 2048 tile before the board fills up. Tiles slide with an animation, the game is saved on exit and resumed next time, and the best score is kept.

Keys: `W` `A` `S` `D` slide, `R` new game, `?` help, `!` about, `ESC` quit.

Folder: [`GAMES/2048`](./GAMES/2048)

![](./GAMES/2048/SCREEN01.GIF)

![](./GAMES/2048/SCREEN02.GIF)

### Arkanoid

Bounce the ball off the Vaus to break every brick of the round. Silver bricks take several hits and gold ones never break. Capsules from broken bricks expand the Vaus, catch the ball, slow it down, give an extra life or skip to the next level. Eight levels that come back faster, leader board, five ball speeds.

Keys: arrows or `A` `D` move (held keys glide), `W`/`SPACE` launch, `P` pause, `ESC` quit.

Folder: [`GAMES/ARKANOID`](./GAMES/ARKANOID)

![](./GAMES/ARKANOID/SCREEN01.GIF)

![](./GAMES/ARKANOID/SCREEN02.GIF)

### Chomp

A maze game in the style of the arcade dot-eaters, with an original maze. Eat every dot while four ghosts hunt you, each its own way: straight at you, ahead of you, from the side, or keeping its distance. Power dots turn them blue so you can eat them. Fruit, a tunnel, faster levels, three skill levels and a leader board.

Keys: arrows or `W` `A` `S` `D` move (turns can be pressed early), `P` pause, `ESC` quit.

Folder: [`GAMES/CHOMP`](./GAMES/CHOMP)

![](./GAMES/CHOMP/SCREEN01.GIF)

![](./GAMES/CHOMP/SCREEN02.GIF)

### Game of Life

Conway's cellular automaton: cells are born and die by the number of their neighbours. Step through the generations or let them run at the speed you choose, and draw your own colony in the editor or start from one of the built-in patterns.

Keys: `SPACE` step/auto, `ENTER` next generation, `LEFT`/`RIGHT` speed, `P` pause, `R` random colony, `ESC` back. In the editor: arrows or `W` `A` `S` `D` move, `SPACE` toggles a cell, `P` places a pattern (`TAB` picks it, `ENTER` drops it), `C` clears, `F` fills.

Folder: [`GAMES/GOL`](./GAMES/GOL)

![](./GAMES/GOL/SCREEN01.GIF)

![](./GAMES/GOL/SCREEN02.GIF)

### Gomoku

Five in a row on a 15 x 15 wooden board. Two players, or one against the computer at the easy, medium or hard level; play black, white, or alternate between games. Take back moves, and optionally only an exact five wins.

Keys: arrows or `W` `A` `S` `D` move, `SPACE`/`ENTER` play, `U` undo, `H` help, `?` about, `ESC` quit.

Folder: [`GAMES/GOMOKU`](./GAMES/GOMOKU)

![](./GAMES/GOMOKU/SCREEN01.GIF)

![](./GAMES/GOMOKU/SCREEN02.GIF)

### Minesweeper

Clear a field of hidden mines: every open tile tells how many of its neighbours hide one. Beginner, intermediate and expert fields, or a custom field up to 30 x 16 entered in a form. The first tile opened is never a mine, flags (and optional question marks) mark the mines, and opening a number with all its mines flagged opens its neighbours at once. The best time of each level is kept.

Keys: arrows or `W` `A` `S` `D` move, `SPACE`/`ENTER` open, `F` flag, `N` new game, `H` help, `?` about, `ESC` quit.

Folder: [`GAMES/MINES`](./GAMES/MINES)

![](./GAMES/MINES/SCREEN01.GIF)

![](./GAMES/MINES/SCREEN02.GIF)

### Reversi

Reversi (Othello) on a green 8 x 8 board: enclose the other player's discs to turn them over, and have the most discs when neither side can move. Two players, or one against the computer: easy, medium, or hard, which searches several moves ahead and plays the last eight perfectly. Legal moves can be shown, turned discs flip on screen, and moves can be taken back.

Keys: arrows or `W` `A` `S` `D` move, `SPACE`/`ENTER` play, `U` undo, `M` show moves, `H` help, `?` about, `ESC` quit.

Folder: [`GAMES/REVERSI`](./GAMES/REVERSI)

![](./GAMES/REVERSI/SCREEN01.GIF)

![](./GAMES/REVERSI/SCREEN02.GIF)

### Runner

A platform game in the style of Lode Runner, with 10 original levels. Take every bar of gold, then climb out at the top on the ladders that show up. Run, climb, go hand over hand along ropes, and dig holes in the brick floor: guards that fall in are stuck for a moment, and holes fill up again after a few seconds, burying whoever is inside. The guards find their way to you, carry gold off and drop it somewhere else.

Keys: arrows or `W` `A` `S` `D` run and climb, `Z`/`X` dig left/right, `P` pause, `R` restart the level (costs a life), `H` help, `ESC` quit.

Folder: [`GAMES/RUNNER`](./GAMES/RUNNER)

![](./GAMES/RUNNER/SCREEN01.GIF)

![](./GAMES/RUNNER/SCREEN02.GIF)

### Snake

Steer the snake to the apples; each one makes it longer. Gems show up for a few seconds and are worth more the sooner they are eaten. Eight levels with their own obstacles, three lives, five speeds, and an option to go through the edges.

Keys: arrows or `W` `A` `S` `D` turn, `P` pause, `ESC` quit.

Folder: [`GAMES/SNAKE`](./GAMES/SNAKE)

![](./GAMES/SNAKE/SCREEN01.GIF)

![](./GAMES/SNAKE/SCREEN02.GIF)

### Sokoban

Push every crate onto a goal; crates can't be pulled. Twelve original levels, from 5 to 33 pushes, each checked solvable by a solver; a level unlocks when the one before is solved. Unlimited undo and the fewest moves and pushes kept for every level.

Keys: arrows or `W` `A` `S` `D` walk and push, `U` undo, `R` restart, `N`/`B` next/previous level, `ESC` pause menu (continue, restart level, quit to main menu).

Folder: [`GAMES/SOKOBAN`](./GAMES/SOKOBAN)

![](./GAMES/SOKOBAN/SCREEN01.GIF)

![](./GAMES/SOKOBAN/SCREEN02.GIF)

### Solitaire

Klondike patience with 7 x 4 text-mode cards: build the four foundations from ace to king by suit, moving cards down the seven piles in alternating colors. Draw one or three cards, standard scoring, a clock, statistics, and the bouncing cards of a won game. All from the keyboard: pick up and put down cards, take several cards of a pile at once, or send a card to its best place with one key.

Keys: arrows move, `UP` in a pile takes more cards, `SPACE` pick up/put down, `ENTER` best place, `D` draw, `A` all possible cards up, `U` undo, `N` new deal, `H` help, `ESC` menu.

Folder: [`GAMES/SOLITAIR`](./GAMES/SOLITAIR)

![](./GAMES/SOLITAIR/SCREEN01.GIF)

![](./GAMES/SOLITAIR/SCREEN02.GIF)

### Tetris

Fit the falling pieces into full lines to clear them. Speed and starting height can be chosen, with a shadow hint, the next piece, a colour-blind mode and a leader board.

Keys: `A` `S` `D` move, `W`/`Q` spin, `SPACE` drop (or arrows, `/` and `ENTER`), `H` hint, `B` colour-blind, `P` pause, `ESC` quit.

Folder: [`GAMES/TETRIS`](./GAMES/TETRIS)

![](./GAMES/TETRIS/SCREEN01.GIF)

![](./GAMES/TETRIS/SCREEN02.GIF)

### Tic-Tac-Toe

Noughts and crosses on a 3 x 3 board, against a friend or the computer at three levels (the hard one searches every move with minimax). Choose the colours of the players, or hearts instead of noughts.

Keys: `W` `A` `S` `D` move, `SPACE` play, `H` help, `ESC` quit.

Folder: [`GAMES/T3`](./GAMES/T3)

![](./GAMES/T3/SCREEN01.GIF)

![](./GAMES/T3/SCREEN02.GIF)

## Tools in detail

### PM (package manager)

A two-panel file manager for package files: pack and unpack files (or add to
and delete from an existing package), verify checksums, view package
information, copy, move and rename files, and build self-extracting
executables. **CREATE INSTALLER PACKAGE** (`CTRL-I`) packs the marked files
and writes `INSTALL.DAT` for the installer, with the exact package size; its
form is filled from an `INSTALL.DAT` already in the folder. Press `F1` for the
key list.

Folder: [`TOOLS/PCKMGR`](./TOOLS/PCKMGR)

![](./TOOLS/PCKMGR/SCREEN01.GIF)

![](./TOOLS/PCKMGR/SCREEN02.GIF)

### PCK (command-line packer)

Does what PM does, from the command line or a batch file:

```
PCK L GAMES.PCK                 list
PCK T GAMES.PCK                 test the checksum
PCK X GAMES.PCK C:\GAMES        extract
PCK C GAMES.PCK *.EXE *.HLP     create (or @list.txt)
PCK A GAMES.PCK README.TXT      add or replace files
PCK D GAMES.PCK OLD.EXE         delete files
PCK S GAMES.PCK SETUP           make SETUP.EXE, a self-extractor
```

Folder: [`TOOLS/PCKMGR`](./TOOLS/PCKMGR)

### SFX (self-extractor)

The stub that PM (`CTRL-E`) and PCK (`S`) put in front of a package to make a
self-extracting executable. Keep `SFX.EXE` next to `PM.EXE` and `PCK.EXE`.
The executable unpacks itself into the current folder, or into the folder
given on its command line.

Folder: [`TOOLS/PCKMGR`](./TOOLS/PCKMGR)

### INSTALL (installer)

Installs the files of a package into a chosen drive and folder, after checking
the package size and the free disk space. The destination is entered in a
form that checks right away that the drive is ready and has room for the
package. PACKAGE VERIFICATION checks the
package checksum on request. `INSTALL.DAT` holds the package name,
description, license, default drive and folder, needed space (`SPACE`), and
the package file and its exact size (`SIZE`). PM creates both files
(`CTRL-I`); put `INSTALL.EXE` with them.

```
[INFO]
PACKAGE   = Text-mode games collection
DESC      = Version ~0F1.0
LICENSE   = This is a ~03FREEWARE
DISK      = C
FOLDER    = TMG
SPACE     = 1024000

[FILE]
PACKAGE   = INSTALL.PCK
SIZE      = 683365
```

Folder: [`TOOLS/INSTALL`](./TOOLS/INSTALL)

![](./TOOLS/INSTALL/SCREEN01.GIF)

![](./TOOLS/INSTALL/SCREEN02.GIF)

### ASCII (character map)

Shows the 256 characters of the text-mode font; pick one to see its code in
hexadecimal and decimal. Handy for drawing boxes and ANSI art.

Folder: [`TOOLS`](./TOOLS)

![](./TOOLS/ASCII.GIF)

### SCANCODE (key code viewer)

Shows the code `getkey` returns for every key pressed, with a short history,
to find the `KEY_...` value of a key. `ESC` quits.

Folder: [`TOOLS`](./TOOLS)

![](./TOOLS/SCANCODE.GIF)

### VIEWBIN (screen dump viewer)

`VIEWBIN FILE.BIN` shows a screen dump (4000 bytes of characters and
colours), like the ones the games save with `F12`.

Folder: [`TOOLS`](./TOOLS)

![](./TOOLS/VIEWBIN.GIF)

## What's new in 1.5

- Screen output is clipped to the screen, and dialogs, boxes and menus fit on
  it, so they no longer hang or corrupt memory.
- One millisecond timer (`ltx_timer`, `sleep`) that no longer reprograms the
  PIT; `LIBTIMER` was merged into `LIBTEXT`.
- Packages: add, replace and delete files in existing packages, self-extracting
  executables, safe extraction paths, full I/O error checking.
- Counters hold 32-bit values; many nil-pointer, I/O and off-by-one fixes.
- Free Pascal builds work: register usage in the assembler routines was fixed.
- A form control in `LIBCTLS`: dialogs of typed fields (text, numbers, file
  names, paths, drives, choices, switches, passwords) checked as they are left.
- Nine new games: Arkanoid, Chomp, Gomoku, Minesweeper, Reversi, Runner, Snake,
  Sokoban and Solitaire.

## Known issues

- DOSBox 0.74 emulates the x87 FPU with 64-bit doubles. The Free Pascal 3.2
  run-time library copies memory with 64-bit FPU loads and stores, so FPC
  builds show corrupted text there. Use DOSBox-X or real hardware to run FPC
  builds; TMT and Borland Pascal builds are not affected.

## License

MIT, see [LICENSE](./LICENSE).
