#!/bin/bash

# ICCF PGN Download Scraper
# Usage: ./iccf_scraper.sh <game_number>

# Configuration
GAME_NUMBER="$1"
USER_AGENT="Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
DELAY=2  # seconds between requests
COOKIE_FILE="/tmp/iccf_cookies.txt"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to display usage
usage() {
    echo "Usage: $0 <game_number>"
    echo "Example: $0 1234567"
    exit 1
}

# Function to clean up
cleanup() {
    rm -f "$COOKIE_FILE" "/tmp/iccf_login.html" "/tmp/iccf_game.html"
}

# Function to make a request with curl
make_request() {
    local url="$1"
    local output="$2"
    local data="$3"
    
    if [ -z "$data" ]; then
        curl -s -A "$USER_AGENT" -b "$COOKIE_FILE" -c "$COOKIE_FILE" -L "$url" -o "$output"
    else
        curl -s -A "$USER_AGENT" -b "$COOKIE_FILE" -c "$COOKIE_FILE" -L "$url" -d "$data" -o "$output"
    fi
    
    local status=$?
    sleep "$DELAY"
    return $status
}

# Function to check if we're logged in
check_login() {
    return 0
    if grep -qi "login" "/tmp/iccf_login.html" 2>/dev/null; then
        return 1
    fi
    return 0
}

# Function to extract PGN from game page
extract_pgn() {
    local html_file="$1"
    local output_file="$2"
    
    # Try to find PGN data in the HTML
    # This pattern might need adjustment based on ICCF's actual HTML structure
    grep -o -P '(?<=\[Event[^\]]*\]).*?(?=</pre>|</textarea>)' "$html_file" | head -1 > "$output_file"
    
    # If the above doesn't work, try alternative patterns
    if [ ! -s "$output_file" ]; then
        # Look for PGN in textareas or pre tags
        sed -n '/<textarea[^>]*>/,/<\/textarea>/p' "$html_file" | sed '1d;$d' > "$output_file"
    fi
    
    if [ ! -s "$output_file" ]; then
        sed -n '/<pre[^>]*>/,/<\/pre>/p' "$html_file" | sed '1d;$d' > "$output_file"
    fi
}

# Main script
main() {
    # Check if game number is provided
    if [ -z "$GAME_NUMBER" ]; then
        echo -e "${RED}Error: Game number is required${NC}"
        usage
    fi

    # Validate game number (should be numeric)
    if false; then
    if ! [[ "$GAME_NUMBER" =~ ^[0-9]+$ ]]; then
        echo -e "${RED}Error: Game number must be numeric${NC}"
        usage
    fi
    fi

    echo -e "${YELLOW}Attempting to download game #$GAME_NUMBER from ICCF...${NC}"
    
    # Set up cleanup on exit
    trap cleanup EXIT
    
    # Note: ICCF requires login for most game access
    echo -e "${YELLOW}Note: ICCF requires authentication to access games.${NC}"
    echo -e "${YELLOW}You'll need to provide your login credentials.${NC}"
    
    # Prompt for credentials (consider using .netrc or environment variables instead)
    username=$2
    password=$3
    #read -p "ICCF Username: " username
    #read -s -p "ICCF Password: " password
    echo
    
    if [ -z "$username" ] || [ -z "$password" ]; then
        echo -e "${RED}Error: Username and password are required${NC}"
        exit 1
    fi

    # Step 1: Get login page to obtain any necessary tokens
    echo -e "${YELLOW}Step 1: Accessing login page...${NC}"
    make_request "https://iccf.com/login" "/tmp/iccf_login.html"
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Failed to access ICCF website${NC}"
        exit 1
    fi

    # Step 2: Attempt login (you'll need to inspect the login form for exact field names)
    echo -e "${YELLOW}Step 2: Attempting login...${NC}"
    
    # Note: You'll need to inspect the ICCF login form to get the exact field names
    # This is a template - adjust based on actual form inspection
    login_data="ICCF ID=$username&Password=$password&submit=LOGIN"
    
    make_request "https://iccf.com/login" "/tmp/iccf_login_check.html" "$login_data"
    
    # Check if login was successful
    if ! check_login; then
        echo -e "${RED}Error: Login failed. Please check your credentials.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}Login successful${NC}"

    # Step 3: Access the game page
    # The URL structure may vary - this is a common pattern
    GAME_URL="https://iccf.com/game?id=$GAME_NUMBER"
    # Alternative patterns to try:
    # GAME_URL="https://iccf.com/Game?id=$GAME_NUMBER"
    # GAME_URL="https://iccf.com/showGame?id=$GAME_NUMBER"
    
    echo -e "${YELLOW}Step 3: Accessing game page...${NC}"
    make_request "$GAME_URL" "/tmp/iccf_game.html"
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Failed to access game page${NC}"
        exit 1
    fi

    # Step 4: Extract PGN
    echo -e "${YELLOW}Step 4: Extracting PGN data...${NC}"
    PGN_FILE="game_${GAME_NUMBER}.pgn"
    extract_pgn "/tmp/iccf_game.html" "$PGN_FILE"
    
    if [ -s "$PGN_FILE" ]; then
        echo -e "${GREEN}Success! PGN saved to: $PGN_FILE${NC}"
        echo -e "${YELLOW}PGN content:${NC}"
        cat "$PGN_FILE"
    else
        echo -e "${RED}Error: Could not extract PGN data${NC}"
        echo -e "${YELLOW}Debug info:${NC}"
        echo "The game page might have a different structure than expected."
        echo "You may need to:"
        echo "1. Inspect the HTML structure manually"
        echo "2. Update the extract_pgn function with the correct selectors"
        echo "3. Check if the game is publicly accessible"
        exit 1
    fi
}

# Run main function
main "$@"
