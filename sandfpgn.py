#!/usr/bin/env python3
"""
PGN Database Search and Filter Tools
Handles PGN files with specific format:
- One or more uncommented games per file
- Moves grouped 5 per line
- No blank character after move number (e.g., "1.e4" not "1. e4")
"""

import re
import os
from typing import List, Dict, Callable, Optional
from dataclasses import dataclass


@dataclass
class Game:
    """Represents a single chess game from PGN"""
    headers: Dict[str, str]
    moves: str
    raw_text: str


class PGNParser:
    """Parse PGN files with the specific format"""
    
    @staticmethod
    def parse_file(filepath: str) -> List[Game]:
        """Parse a PGN file and return list of games"""
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        games = []
        # Split by empty lines between games
        game_texts = re.split(r'\n\n(?=\[)', content.strip())
        
        for game_text in game_texts:
            if not game_text.strip():
                continue
            
            game = PGNParser._parse_game(game_text)
            if game:
                games.append(game)
        
        return games
    
    @staticmethod
    def _parse_game(game_text: str) -> Optional[Game]:
        """Parse a single game text"""
        lines = game_text.split('\n')
        headers = {}
        moves_lines = []
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
            
            # Parse header
            if line.startswith('[') and line.endswith(']'):
                match = re.match(r'\[(\w+)\s+"([^"]*)"\]', line)
                if match:
                    headers[match.group(1)] = match.group(2)
            else:
                # Parse moves
                moves_lines.append(line)
        
        if not headers:
            return None
        
        moves = ' '.join(moves_lines)
        return Game(headers=headers, moves=moves, raw_text=game_text)


class PGNFilter:
    """Filter games based on various criteria"""
    
    def __init__(self, games: List[Game]):
        self.games = games
    
    def filter_by_player(self, player_name: str, color: Optional[str] = None) -> List[Game]:
        """
        Filter games by player name
        color: 'white', 'black', or None (either color)
        """
        results = []
        player_lower = player_name.lower()
        
        for game in self.games:
            white = game.headers.get('White', '').lower()
            black = game.headers.get('Black', '').lower()
            
            if color is None or color.lower() == 'white':
                if player_lower in white:
                    results.append(game)
                    continue
            
            if color is None or color.lower() == 'black':
                if player_lower in black:
                    results.append(game)
        
        return results
    
    def filter_by_result(self, result: str) -> List[Game]:
        """
        Filter games by result
        result: '1-0', '0-1', '1/2-1/2', or '*'
        """
        return [g for g in self.games if g.headers.get('Result') == result]
    
    def filter_by_opening(self, opening_name: str) -> List[Game]:
        """Filter games by opening name (case-insensitive)"""
        opening_lower = opening_name.lower()
        results = []
        
        for game in self.games:
            eco = game.headers.get('ECO', '').lower()
            opening = game.headers.get('Opening', '').lower()
            variation = game.headers.get('Variation', '').lower()
            
            if (opening_lower in eco or 
                opening_lower in opening or 
                opening_lower in variation):
                results.append(game)
        
        return results
    
    def filter_by_eco(self, eco_code: str) -> List[Game]:
        """Filter games by ECO code (e.g., 'C50', 'C5*' for wildcard)"""
        if eco_code.endswith('*'):
            prefix = eco_code[:-1]
            return [g for g in self.games 
                   if g.headers.get('ECO', '').startswith(prefix)]
        else:
            return [g for g in self.games 
                   if g.headers.get('ECO') == eco_code]
    
    def filter_by_year_range(self, start_year: int, end_year: int) -> List[Game]:
        """Filter games by year range"""
        results = []
        for game in self.games:
            date = game.headers.get('Date', '')
            match = re.match(r'(\d{4})', date)
            if match:
                year = int(match.group(1))
                if start_year <= year <= end_year:
                    results.append(game)
        return results
    
    def filter_by_moves(self, move_pattern: str) -> List[Game]:
        """
        Filter games containing a specific move sequence
        Example: '1.e4 e5 2.Nf3' or just 'Qh5'
        """
        # Normalize the pattern
        pattern_normalized = move_pattern.replace(' ', '')
        results = []
        
        for game in self.games:
            moves_normalized = game.moves.replace(' ', '')
            if pattern_normalized in moves_normalized:
                results.append(game)
        
        return results
    
    def filter_by_custom(self, condition: Callable[[Game], bool]) -> List[Game]:
        """Filter games using a custom condition function"""
        return [g for g in self.games if condition(g)]


class PGNSearcher:
    """Search across multiple PGN files"""
    
    def __init__(self, directory: str = '.'):
        self.directory = directory
    
    def search_files(self, pattern: str = '*.pgn') -> List[str]:
        """Find all PGN files in directory"""
        pgn_files = []
        for root, dirs, files in os.walk(self.directory):
            for file in files:
                if file.endswith('.pgn'):
                    pgn_files.append(os.path.join(root, file))
        return pgn_files
    
    def load_all_games(self, pgn_files: Optional[List[str]] = None) -> List[Game]:
        """Load all games from multiple PGN files"""
        if pgn_files is None:
            pgn_files = self.search_files()
        
        all_games = []
        for pgn_file in pgn_files:
            try:
                games = PGNParser.parse_file(pgn_file)
                all_games.extend(games)
                print(f"Loaded {len(games)} games from {pgn_file}")
            except Exception as e:
                print(f"Error loading {pgn_file}: {e}")
        
        return all_games


class PGNExporter:
    """Export filtered games to new PGN file"""
    
    @staticmethod
    def export_games(games: List[Game], output_file: str):
        """Export games to a PGN file"""
        with open(output_file, 'w', encoding='utf-8') as f:
            for i, game in enumerate(games):
                f.write(game.raw_text)
                if i < len(games) - 1:
                    f.write('\n\n')
        
        print(f"Exported {len(games)} games to {output_file}")


# Example usage functions
def example_search_by_player():
    """Example: Search for all games by a specific player"""
    searcher = PGNSearcher('.')
    all_games = searcher.load_all_games()
    
    filter_obj = PGNFilter(all_games)
    fischer_games = filter_obj.filter_by_player('Fischer')
    
    print(f"Found {len(fischer_games)} games by Fischer")
    PGNExporter.export_games(fischer_games, 'fischer_games.pgn')


def example_search_by_opening():
    """Example: Search for games with specific opening"""
    searcher = PGNSearcher('.')
    all_games = searcher.load_all_games()
    
    filter_obj = PGNFilter(all_games)
    sicilian_games = filter_obj.filter_by_opening('Sicilian')
    
    print(f"Found {len(sicilian_games)} Sicilian Defense games")
    PGNExporter.export_games(sicilian_games, 'sicilian_games.pgn')


def example_complex_search():
    """Example: Complex search with multiple criteria"""
    searcher = PGNSearcher('.')
    all_games = searcher.load_all_games()
    
    # Chain multiple filters
    filter_obj = PGNFilter(all_games)
    
    # Find all Kasparov games as White in the 1990s that he won
    results = filter_obj.filter_by_player('Kasparov', 'white')
    filter_obj = PGNFilter(results)
    results = filter_obj.filter_by_year_range(1990, 1999)
    filter_obj = PGNFilter(results)
    results = filter_obj.filter_by_result('1-0')
    
    print(f"Found {len(results)} games matching criteria")
    PGNExporter.export_games(results, 'kasparov_white_wins_90s.pgn')


def example_search_by_moves():
    """Example: Search for games with specific move sequence"""
    searcher = PGNSearcher('.')
    all_games = searcher.load_all_games()
    
    filter_obj = PGNFilter(all_games)
    # Find games with Scholar's Mate pattern
    results = filter_obj.filter_by_moves('Qh5')
    
    print(f"Found {len(results)} games with Qh5")
    PGNExporter.export_games(results, 'qh5_games.pgn')


if __name__ == '__main__':
    # Example: Load and display statistics
    print("PGN Database Search Tool")
    print("-" * 40)
    
    searcher = PGNSearcher('.')
    pgn_files = searcher.search_files()
    print(f"Found {len(pgn_files)} PGN files")
    
    if pgn_files:
        all_games = searcher.load_all_games(pgn_files[:1])  # Load first file as example
        print(f"Total games loaded: {len(all_games)}")
        
        if all_games:
            print("\nFirst game headers:")
            for key, value in all_games[0].headers.items():
                print(f"  {key}: {value}")
            print(f"\nMoves preview: {all_games[0].moves[:100]}...")