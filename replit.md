# Baeagn Chess Engine

## Overview
This is a chess engine project combining a C-based chess engine (baeagn) with an HTML/CSS/JavaScript frontend for visualizing chess positions using FEN (Forsyth-Edwards Notation) notation.

**Author**: Danielzapirtan  
**Language**: C (backend), HTML/CSS/JavaScript (frontend)  
**Import Date**: November 4, 2025

## Project Structure
```
.
├── baeagn.c           # Main chess engine source code (1400+ lines)
├── build.sh           # Build script for compiling the chess engine
├── server.py          # Python HTTP server to serve the frontend
├── index.html         # FEN Chess Board Viewer frontend
├── anl/               # Analysis files
├── bpf/               # Board position files
├── brd/               # Board files
├── pgn/               # PGN (Portable Game Notation) files
└── start.*            # Various starting position files
```

## Key Components

### Chess Engine (baeagn.c)
- Written in C with optimized compilation flags
- Supports chess move generation and analysis
- Can read FEN, PGN, and custom board formats
- Configurable via compile-time flags (_CHESS960, _NOEDIT, _GAME_ini)

### Frontend (index.html)
- Interactive chess board viewer
- Supports FEN notation input
- Features:
  - Load custom FEN positions
  - Reset to initial chess position
  - Flip board view
  - Board coordinates display
  - LocalStorage persistence for positions
- Color-coded pieces (white=pink, black=blue)
- Responsive design with clean UI

### Server (server.py)
- Simple Python HTTP server
- Binds to 0.0.0.0:5000 for Replit environment
- Includes cache-control headers to ensure updates are visible
- Serves static HTML/CSS/JavaScript files

## Building and Running

### Build the Chess Engine
The chess engine requires GCC to compile:
```bash
_CHESS960=0 _NOEDIT=0 ./build.sh
```

This produces the `baeagn` binary.

### Run the Web Frontend
The frontend is served via a Python HTTP server on port 5000:
```bash
python3 server.py
```

Access the chess board viewer in the Replit webview.

## Deployment Configuration
- **Type**: Autoscale (stateless web application)
- **Build**: Compiles the C chess engine
- **Run**: Starts Python HTTP server on port 5000

## Recent Changes (Nov 4, 2025)
1. Installed GCC and build tools via Nix
2. Fixed build configuration to use proper environment variables
3. Created Python HTTP server for frontend hosting
4. Configured workflow for automatic server startup
5. Set up deployment configuration for publishing
6. Updated .gitignore for Python and C build artifacts

## User Preferences
None recorded yet.

## Technical Notes
- The chess engine uses alpha-beta pruning and other chess-specific optimizations
- Build flags control various features (chess960 support, edit modes, etc.)
- The frontend uses Unicode chess symbols for piece display
- No external JavaScript dependencies - pure vanilla JS
