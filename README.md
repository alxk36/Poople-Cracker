# Poople Cracker

**Author:** alxk36

Poople Cracker is a small C++ command-line helper for **Poople.io**. Poople
describes itself as a daily word game where you turn words into “poop” and try
to find the shortest path. Like Wordle, it is a daily word puzzle, but this
program models the word-to-word transformation challenge rather than guessing a
hidden word from colored-letter clues.

Given a four-letter starting word, this program searches for a sequence of
valid four-letter words that ends at `POOP`. Each step changes exactly one
letter. It prints the shortest path it can find using its bundled dictionary.

This is a standalone helper: it does not open a browser, communicate with
Poople.io, or submit guesses. The game’s accepted words and rules may differ
from the bundled list and from the assumptions in this program.

## Requirements

- A C++20 compiler (the implementation uses `std::unordered_set::contains`).
- `dictionary.txt` in the current working directory when running the program.

## Build and run

From the project directory, using g++:

```sh
g++ -std=c++20 -O2 -Wall -Wextra -pedantic poopleCracker.cpp -o poopleCracker
./poopleCracker
```

On Windows, run the executable from the project directory:

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -pedantic poopleCracker.cpp -o poopleCracker.exe
.\poopleCracker.exe
```

With Microsoft Visual C++ (Developer Command Prompt):

```bat
cl /std:c++20 /EHsc /W4 poopleCracker.cpp
poopleCracker.exe
```

The program prompts for a four-letter starting word, for example `cold`.
Input is case-insensitive. The dictionary file is opened relative to the
current working directory, so run the executable with `dictionary.txt` beside
it or start it from the project directory.

Possible output:

```text
Enter a 4-letter word: cold
Shortest path (4 changes):
COLD
...
POOP
```

The exact route depends on which words are in `dictionary.txt`. If no path can
be found, the program prints `No path found.` It exits with an error if the
dictionary cannot be opened, the input is not exactly four letters, or the
dictionary does not contain `poop`.

## How it works

1. Opens `dictionary.txt`, converts each token to lowercase, and keeps unique
   alphabetic words that contain exactly four letters. Other lines/tokens are
   ignored.
2. Reads and validates the starting word, then uses `poop` as the fixed target.
3. Calls `findShortestPath`, which runs breadth-first search (BFS). For each
   word, it tries replacing each character with every letter from `a` to `z`;
   a candidate is valid if it appears in the dictionary and has not already
   been visited.
4. Records each discovered word's parent. BFS visits words in increasing
   number of changes, so the first route it finds to `poop` has the fewest
   one-letter changes within this dictionary.
5. The BFS function follows parent links from `poop` back to the start and
   returns the reversed route. `main` prints each word in uppercase, or reports
   that no path was found.

The reported number of changes is one less than the number of printed words.
For example, a path containing five words has four changes.

## Dictionary source and limitations

`dictionary.txt` is the word list bundled with this project. Its original
upstream source, creator, and license are not identified in the available
project files, so no external source attribution can be reliably provided.
The list contains four-letter entries and is treated as plain whitespace-
separated text. It should not be assumed to match Poople.io's official
dictionary or accepted-word list. Check the source and license before
redistributing or replacing it with another word list.

## Source code map

- `poopleCracker.cpp` contains input validation, dictionary loading, BFS, and
  path reconstruction.
- `dictionary.txt` supplies the candidate words.
