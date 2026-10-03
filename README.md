# Baeagn

Baeagn is a C chess engine by Antoniu-Daniel Zăpîrțan. It can analyze positions, evaluate a position, and play a game from a FEN position. The repository also contains scripts and GitHub Actions workflows for running engine benchmarks and processing PGN data.

## Requirements

- GCC
- GNU Make is not required; the project builds with `bin/build.sh`
- `jq` and `curl` for the online game and benchmark helper scripts
- `pgn-extract` for PGN-to-FEN parsing from the interactive editor
- Bash

The benchmark workflow installs GCC and `jq` on Ubuntu runners. The engine itself only requires GCC and the system math library.

## Build

Build the standard chess engine:

```sh
_ICCF=0 _NOEDIT=1 _CHESS960=0 sh bin/build.sh
```

This creates the `baeagn` executable in the repository root. The build uses GCC optimizations including `-O3` and `-march=native`.

Build the ICCF variant instead:

```sh
_ICCF=1 _NOEDIT=1 _CHESS960=0 sh bin/build.sh
```

When `_ICCF=1`, the build selects `ctpk.c`; otherwise it selects `baeagn.c`. `_CHESS960` and `_NOEDIT` are compile-time configuration flags used by the engine and editor.

## Running the engine

The engine reads its initial position from `start.fen`.

```sh
# Start the default analysis mode
./baeagn

# Explicitly analyze the position
./baeagn analyze

# Search for a move
./baeagn go

# Evaluate the position
./baeagn eval
```

The repository includes a sample `start.fen`. Replace that file with another valid FEN position before starting the engine. Analysis output can be redirected to a file:

```sh
mkdir -p anl
./baeagn analyze > anl/analysis.txt 2>&1
```

The interactive editor can also use `start.pgn`; PGN parsing requires the `pgn-extract` command to be installed.

## Android app

The repository also contains an Android Studio project in `app/`. Open the repository root in Android Studio with the Android SDK and NDK installed, then build or run the `app` configuration. From a configured command line:

```sh
./gradlew :app:assembleDebug
```

The app is named **baeagn**. It provides a touch chessboard, legal move entry, promotion, move undo/redo, board flip, position setup by FEN, PGN import/export, and computer-player selection for White and Black independently. Analysis mode asks the bundled native baeagn engine for a suggested move without playing it. The C engine is compiled into the Android app through JNI; the existing desktop build and command-line behavior are unchanged.

## Repository layout

| Path | Purpose |
| --- | --- |
| `baeagn.c` | Standard chess engine and interactive analysis implementation |
| `app/` | Native Android frontend and JNI integration for `baeagn.c` |
| `ctpk.c` | ICCF-oriented engine variant |
| `bin/build.sh` | Compiles the selected engine variant |
| `bin/run` | Starts the ICCF executable and writes output to `anl/start.anl` |
| `bin/dlpgn.sh` | Retrieves a position from the configured Chess.com game feed |
| `bin/sandfpgn.py` | PGN-related helper script |
| `start.fen` | Initial position consumed by the engine |
| `start.pgn` | Sample PGN input |
| `bpf/` and `brd/` | Board and position data |
| `pgn/` | Benchmark and game-list PGN data |
| `.github/workflows/` | Manual and automated benchmark/PGN workflows |

## Benchmarks and workflows

The benchmark workflow is available as a manually dispatched GitHub Actions workflow:

```sh
gh workflow run bench.yml
```

It compiles the standard engine, runs one or more positions, and reports the resulting game and engine output. The workflow can also be started from the **Actions** tab in GitHub.

`bin/dlpgn.sh` and `bin/ore.sh` are tailored to the repository owner's Chess.com account and GitHub setup. Review their account names, paths, credentials, and workflow settings before adapting them for another environment.

## License

No license file is currently included in this repository. Treat the source as all rights reserved unless the author provides separate permission or licensing terms.
