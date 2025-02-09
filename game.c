/**
   @file game.c
   @author Michael Warstler (mwwarstl)
   Implementation file that manages game rules and loop. Creates/deletes a game, updates and 
   loops a game, places stones, and can replay a previously saved game.
*/

#include "game.h"
#include "board.h"          
#include "error-codes.h"    
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/** Represents the initial number of moves allowed */
#define INITIAL_NUM_MOVES 16
/** Max string length allowed excluding the null terminator */
#define MAX_STRING_LENGTH 3
/** The amount of stone connections required to win */
#define NEEDED_CONNECTIONS 5
/** Number of open fours allowed before becoming forbidden */
#define ALLOWED_OPEN_FOURS 1

// Prototypes for static functions to calculate winner and check for forbidden moves.
static bool verticalWin( game *g, unsigned char x );
static bool horizontalWin( game *g, unsigned char y );
static bool diagonalDownWin( game *g, unsigned char x, unsigned char y );
static bool diagonalUpWin( game *g, unsigned char x, unsigned char y );
static void doubleOpenFours( game *g, unsigned char x, unsigned char y );
static bool overline( game *g, unsigned char x, unsigned char y );

// Create a game struct based on game type and board size params.
game* game_create(unsigned char board_size, unsigned char game_type)
{
    game *g = (game *)malloc( sizeof( game ) );
    g->board = board_create( board_size );
    g->type = game_type;
    g->stone = BLACK_STONE;
    g->state = GAME_STATE_PLAYING;
    g->winner = EMPTY_INTERSECTION;
    g->moves = ( move * )malloc( INITIAL_NUM_MOVES * sizeof( move ) );
    g->moves_count = 0;
    g->moves_capacity = INITIAL_NUM_MOVES;
    return g;
}

// Free memory held by game struct and its dynamically allocated fields.
void game_delete( game* g)
{
    // Error check
    if ( g == NULL ) {
        exit( NULL_POINTER_ERR );
    }
    
    board_delete( g->board );
    free( g->moves );
    free( g );
}

// Controls the game each turn.
bool game_update( game* g)
{
    // Game must be in playing state.
    if ( g->state != GAME_STATE_PLAYING ) {
        return false;
    }
    
    // Access current move. (Holds no data currently)
    move *currentMove = &g->moves[ g->moves_count ];
    
    // Set up x and y coordinates for grid                                         
    char formal_coord[ MAX_STRING_LENGTH + 1 ];
    
    int matches = 0;
    // Loop until EOF hit.
    while ( matches != EOF ) {
        // Begin prompt to player
        if ( g->stone == BLACK_STONE ) {
            printf( "Black stone's turn, please enter a move: ");
            currentMove->stone = BLACK_STONE;
        }
        // else white stone
        else {
            printf( "White stone's turn, please enter a move: ");
            currentMove->stone = WHITE_STONE;
        }
        
        // Scan user input and break immediately if EOF, otherwise check coordinates.
        matches = scanf( "%s", formal_coord );
        // Immediately break if at EOF
        if ( matches == EOF ) {
            break;
        }
        else {
            // Check for valid coordinates. If board_coord returns the error code, prompt and scan again.
            if ( board_coord( g->board, formal_coord, &currentMove->x, &currentMove->y ) != FORMAL_COORDINATE_ERR ) {
                // Place stone. If stone is able to be placed, return true for game loop.
                if ( game_place_stone( g, currentMove->x, currentMove->y) ) {
                    return true;
                }
            }
            else {
                printf( "The coordinate you entered is invalid, please try again.\n" );
            }
        }
    }
    
    // EOF reached
    g->state = GAME_STATE_STOPPED;
    printf( "The game is stopped.\n" );
    return false;
}

// Prints board and calls game.update() until that returns false.
void game_loop( game* g)
{
    do {
        // Only print when game state is still playing
        if ( g->state == GAME_STATE_PLAYING ) {
            board_print( g->board, true);
        }
    } while ( game_update( g ) );
    
    // Display board and text on finished game.
    if ( g->state == GAME_STATE_FINISHED ) {
        board_print( g->board, true);
        printf( "Game concluded, %s won.\n", g->winner == BLACK_STONE ? "black" : "white" );
    }
    else if ( g->state == GAME_STATE_FORBIDDEN ) {
        board_print( g->board, true);
        printf( "Game concluded, black made a forbidden move, white won.\n" );
    }
}

// Restarts game loop for saved game.
void game_resume( game* g)
{
    // Error check
    if ( g->state != GAME_STATE_STOPPED || ( g->type != GAME_FREESTYLE && g->type != GAME_RENJU ) ) {
        exit( RESUME_ERR );
    }
    
    // Update state and begin loop.
    g->state = GAME_STATE_PLAYING;
    game_loop( g );
}

// Restarts the game loop for a saved game.
void game_replay( game* g)
{    
    // Begin replay. Create new game struct and copy moves, state, and winner.
    game *replayGame = game_create( g->board->size, g->type );
    replayGame->winner = g->winner;
    replayGame->state = g->state;
    
    // Loop through moves array and perform those moves
    for (int i = 0; i < g->moves_count; i++) {
        // Place stone on replayGame using moves from saved game
        game_place_stone( replayGame, g->moves[i].x, g->moves[i].y );
        
        // Print latest board of replayGame
        board_print( replayGame->board, true );
        
        // Print out winner or stopped state on last iteration only.
        if ( i == g->moves_count - 1) {
            if ( replayGame->winner == WHITE_STONE ) {
            replayGame->state == GAME_STATE_FORBIDDEN ? printf( "Game concluded, black made a forbidden move, white won.\n" ) :
                                                        printf( "Game concluded, white won.\n" );
            }
            else if ( replayGame->winner == BLACK_STONE ) {
                printf( "Game concluded, black won.\n" );
            }
            else if ( replayGame->state == GAME_STATE_STOPPED ) {
                printf( "The game is stopped.\n" );
            }
        }
        
        // Print moves list. Grows as more moves are replayed.
        printf( "Moves:\n" );
        for ( int j = 0; j <= i; j++ ) {
            // Set up formal coordinates for output                                         
            char formal_coord[ MAX_STRING_LENGTH + 1 ];
            
            // Even moves are always Black.
            j % 2 == 0 ? printf( "Black: ") : printf( "  White: ");
            
            // Convert x/y move to formal coord and print to display.
            board_formal_coord( replayGame->board, g->moves[j].x, g->moves[j].y, formal_coord );
            printf( "%3s", formal_coord );
            
            // print newline if White or last piece placed
            if ( j % 2 != 0 || j == g->moves_count - 1 ) {
                printf( "\n" );
            }
        }
        
        // Wait one second before calling again
        sleep( 1 );
    }
}

// Enforces game rules and places stone when possible.
bool game_place_stone( game* g, unsigned char x, unsigned char y)
{
    // Check if space is already occupied.
    if ( board_get( g->board, x, y ) != EMPTY_INTERSECTION ) {
        printf( "There is already a stone at the coordinate you entered, please try again.\n" );
        return false;
    }
    
    // Valid coordinate -> Create as new move. Add the params and current game stone to it.
    move playerMove = { x, y, g->stone };  
    
    //Add move to end of moves array, double size if needed. Update moves_count & moves_capacity.
    if ( g->moves_count >= g->moves_capacity ) {
        g->moves_capacity *= 2;
        g->moves = ( move *)realloc( g->moves, g->moves_capacity * sizeof( move ) );
    }
    g->moves[ g->moves_count++ ] = playerMove; // Increment moves_count after adding to array.
   
    // Update the board based on the move coordinates and the current stone.
    board_set( g->board, playerMove.x, playerMove.y, playerMove.stone );    
    
    // Check for game type --> RENJU has specific white win conditions when black is placing stone.
    if ( g->type == GAME_RENJU && g->stone == BLACK_STONE ) {
        // Check for double open four rule.
        doubleOpenFours( g, x, y );
        
        // Check for overline rule. - Only applies when black won on doubleOpenFour check.
        if ( g->winner == BLACK_STONE && overline( g, x, y ) ) {
            // update game state/winner.
            g->winner = WHITE_STONE;
            g->state = GAME_STATE_FORBIDDEN;
        }
        // Otherwise just update game stone
        else {
            g->stone = WHITE_STONE;
        }
    }
    
    // GOMOKU game type and/or standard win condition checks. Update winner & state if win occurs.
    // Check for a vertical, horizontal, diagonal up/down win conditions.
    else if (verticalWin( g, x ) || horizontalWin( g, y ) || diagonalDownWin( g, x, y ) ||
        diagonalUpWin( g, x, y ) )    {
        g->winner = g->stone;
        g->state = GAME_STATE_FINISHED;
    }
    // If no winner, check for full baord - draw.
    else if ( board_is_full( g->board ) ) {
        g->state = GAME_STATE_FINISHED;
        board_print( g->board, true );
        printf( "Game concluded, the board is full, draw.\n" );     // May need to put this by the end of game loop along with a check for winner if some test fail because of this, otherwise leave it here.
    }
    // If none of the above returned true, game is ongoing. Just update stone.
    else {
        g->stone = g->stone == BLACK_STONE ? WHITE_STONE : BLACK_STONE;
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// STATIC METHODS BELOW //
///////////////////////////////////////////////////////////////////////////////////////////////////


/** 
   Check if there is a win on a vertical/column for current stone/player.
   @param g is pointer to current game struct.
   @param x is column for most recently placed stone.
   @return is true if vertical win conditions found, otherwise false.
*/ 
static bool verticalWin( game *g, unsigned char x )
{
    int connections = 0;
    // Loop through column checking for ammount of connections needed to win is reached.
    for ( int i = 0; i < g->board->size; i++ ) {
        if ( board_get( g->board, x, i ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
    }
    return false;
}

/** 
   Check if there is a win on a horizontal/row for current stone/player.
   @param g is pointer to current game struct.
   @param y is row for most recently placed stone.
   @return is true if horizontal win conditions found, otherwise false.
*/ 
static bool horizontalWin( game *g, unsigned char y )
{
    int connections = 0;
    // Loop through column checking for ammount of connections needed to win is reached.
    for ( int i = 0; i < g->board->size; i++ ) {
        if ( board_get( g->board, i, y ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
    }
    return false;
}

/** 
   Check if there is a win on a diagonal down (top left to btm right orientation) for current 
   stone/player.
   @param g is pointer to current game struct.
   @param x is column for most recently placed stone.
   @param y is row for most recently placed stone.
   @return is true if diagonal down conditions found, otherwise false.
*/ 
static bool diagonalDownWin( game *g, unsigned char x, unsigned char y )
{
    int connections = 0;
    
    // Get topmost row/column based on last placed stone. Decrement row and column until top of  
    // board is reached.
    while ( y > 0 && x > 0 ) {
        y--;
        x--;
    }
    // Loop down diagonal counting connections until end of diagonal reached.
    while ( y < g->board->size && x < g->board->size ) {
        if ( board_get( g->board, x, y ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
        y++;
        x++;
    }
    return false;
}

/** 
   Check if there is a win on a diagonal up (btm left to top right orientation) for current 
   stone/player.
   @param g is pointer to current game struct.
   @param x is column for most recently placed stone.
   @param y is row for most recently placed stone.
   @return is true if diagonal down conditions found, otherwise false.
*/ 
static bool diagonalUpWin( game *g, unsigned char x, unsigned char y )
{
    int connections = 0;
    
    // Get bottom most row/column based on last placed stone. Increment row and decrement column 
    // until bottom of board is reached.
    while ( y < g->board->size && x > 0 ) {
        y++;
        x--;
    }
    // Loop up diagonal counting connections until end of diagonal reached.
    while ( y >= 0 && x < g->board->size ) {
        if ( board_get( g->board, x, y ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
        y--;
        x++;
    }
    return false;
}

/**
   Checks for a double open four forbidden move in Renju. This is when a single stone placement 
   leaves 2 separate lines of 4 connections with empty intersections surrounding the line. Updates
   the game winner to white and state to forbidden if move is detected and black does not win.
   @param g is pointer to current game struct.
   @param x is column for most recently placed stone.
   @param y is row for most recently placed stone.  
*/
static void doubleOpenFours( game *g, unsigned char x, unsigned char y )
{
    // Check if player has won for latest move.
    if (verticalWin( g, x ) || horizontalWin( g, y ) || diagonalDownWin( g, x, y ) ||
        diagonalUpWin( g, x, y ) )    {
        g->winner = g->stone;
        g->state = GAME_STATE_FINISHED;
        return; // leave function.
    }
    // If player did not win, check for double open fours
    else { 
        // Track connections (reset to 0 each time) and # of open fours.
        int connections = 0;
        int openFours = 0;
        
        // Check 4 connections vertical
        for ( int i = 0; i < g->board->size; i++ ) {
            if ( board_get( g->board, x, i ) == g->stone ) {
                connections++;
                // Check if 4 connections. If so, check if spaces before/after 4 are empty.
                if ( connections == NEEDED_CONNECTIONS - 1 ) {
                    if ( board_get( g->board, x + 1, i ) == EMPTY_INTERSECTION &&
                         board_get( g->board, x - connections, i ) == EMPTY_INTERSECTION ) {
                        openFours++;
                    }
                }
            }
            else {
                connections = 0;
            }
        }
        
        // Check 4 connections horizontal
        connections = 0;
        for ( int j = 0; j < g->board->size; j++ ) {
            if ( board_get( g->board, j, y ) == g->stone ) {
                connections++;
                // Check if 4 connections. If so, check if spaces before/after 4 are empty.
                if ( connections == NEEDED_CONNECTIONS - 1 ) {
                    if ( board_get( g->board, j, y + 1 ) == EMPTY_INTERSECTION &&
                         board_get( g->board, j, y - connections ) == EMPTY_INTERSECTION ) {
                        openFours++;
                    }
                }
            }
            else {
                connections = 0;
            }
        }
        
        // Check 4 connections diagonal down.
        // Get topmost row/column based on last placed stone. Decrement row and column until top of  
        // board is reached.
        // Temporary boardX/Y variables to indicate movement through the board.
        unsigned char boardX = x;
        unsigned char boardY = y;
        while ( boardY > 0 && boardX > 0 ) {
            boardY--;
            boardX--;
        }
        // Loop down diagonal counting connections until end of diagonal reached.
        connections = 0;
        while ( boardY < g->board->size && boardX < g->board->size ) {
            if ( board_get( g->board, boardX, boardY ) == g->stone ) {
                connections++;
                // Check if 4 connections. If so, check if spaces before/after 4 are empty.
                if ( connections == NEEDED_CONNECTIONS - 1 ) {
                    if ( board_get( g->board, boardX + 1, boardY + 1 ) == EMPTY_INTERSECTION &&
                         board_get( g->board, boardX - connections, boardY - connections ) ==
                         EMPTY_INTERSECTION ) {
                       openFours++;
                    }
                }
            }
            else {
                connections = 0;
            }
            boardY++;
            boardX++;
        }
        
        // Check 4 connections diagonal up
        // Get bottom most row/column based on last placed stone. Increment row and decrement column 
        // until bottom of board is reached.
        // Reset temp variables for boardX/Y to be parameter placement point.
        boardX = x;
        boardY = y;  
        while ( boardY < g->board->size && boardX > 0 ) {
            boardY++;
            boardX--;
        }
        // Loop up diagonal counting connections until end of diagonal reached.
        connections = 0;
        while ( boardY >= 0 && boardX < g->board->size ) {
            if ( board_get( g->board, boardX, boardY ) == g->stone ) {
                connections++;
                // Check if 4 connections. If so, check if spaces before/after 4 are empty.
                if ( connections == NEEDED_CONNECTIONS - 1 ) {
                    if ( board_get( g->board, boardX + 1, boardY - 1 ) == EMPTY_INTERSECTION &&
                         board_get( g->board, boardX - connections, boardY + connections ) ==
                         EMPTY_INTERSECTION ) {
                       openFours++;
                    }
                }
            }
            else {
                connections = 0;
            }
            boardY--;
            boardX++;
        }        
        
        // If # of open fours exceeds allowed amount, update game state/winner.
        if ( openFours > ALLOWED_OPEN_FOURS ) {
            g->state = GAME_STATE_FORBIDDEN;
            g->winner = WHITE_STONE;
        }
    }
}

/**
   Checks for an overline forbidden move in Renju. If detected by black stones exceeding
   connections needed to win, then white stone wins and game concludes with forbidden state.
   @param g is pointer to current game struct.
   @param x is column for most recently placed stone.
   @param y is row for most recently placed stone.  
   @return is true if overline detected, false otherwise.
*/
static bool overline( game *g, unsigned char x, unsigned char y )
{
    // Black is already detected as winner from doubleOpenFours.
    // Check which way Black "won" and check for overline in that orientation.
    int connections = 0;
    
    // Check vertical overline
    // Loop through column checking for overline. 
    for ( int i = 0; i < g->board->size; i++ ) {
        if ( board_get( g->board, x, i ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS + 1 ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
    }
    
    // Check horizontal overline
    connections = 0;
    for ( int j = 0; j < g->board->size; j++ ) {
        if ( board_get( g->board, j, y ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS + 1 ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
    }
    
    // Check diagonal down overline.
    // Get topmost row/column based on last placed stone. Decrement row and column until top of  
    // board is reached.
    // Temporary boardX/Y variables to indicate movement through the board.
    unsigned char boardX = x;
    unsigned char boardY = y;
    while ( boardY > 0 && boardX > 0 ) {
        boardY--;
        boardX--;
    }
    // Loop down diagonal counting connections until end of diagonal reached.
    while ( boardY < g->board->size && boardX < g->board->size ) {
        if ( board_get( g->board, boardX, boardY ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS + 1 ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
        boardY++;
        boardX++;
    }
    
    // Check diagonal up overline.
    // Get bottom most row/column based on last placed stone. Increment row and decrement column 
    // until bottom of board is reached.
    // Reset temp variables for boardX/Y to be parameter placement point.
    boardX = x;
    boardY = y;  
    while ( boardY < g->board->size && boardX > 0 ) {
        boardY++;
        boardX--;
    }
    // Loop up diagonal counting connections until end of diagonal reached.
    while ( boardY >= 0 && boardX < g->board->size ) {
        if ( board_get( g->board, boardX, boardY ) == g->stone ) {
            connections++;
            if ( connections == NEEDED_CONNECTIONS + 1 ) {
                return true;
            }
        }
        else {
            connections = 0;
        }
        boardY--;
        boardX++;
    }
    
    // No overline detected
    return false;
}