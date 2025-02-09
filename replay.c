/**
   @file replay.c
   @author Michael Warstler (mwwarstl)
   Contains main component for executable that replays a previously saved game of gomoku or renju.
*/

#include "error-codes.h"
#include "game.h"
#include "io.h"
#include <stdio.h>

/** How many command line arguments allowed */
#define ALLOWED_ARGUMENTS 2

/**
   Determines if correct number of command line arguments are entered. If so, begins the game
   replay function.
   @param argc are # of command line arguments.
   @param **argv is array of character pointers listing all arguments.
   @return is exit status
*/
int main(int argc, char **argv)
{
    // Accepts only 1 extra command line argument (path location)
    if ( argc != ALLOWED_ARGUMENTS ) {
        printf( "usage: ./replay <saved-match.gmk>\n" );
        exit( ARGUMENT_ERR );
    }
    else {
        // Import game from command line argument. Replay the game.
        game *replayGame = game_import( argv[ALLOWED_ARGUMENTS - 1] );
        game_replay( replayGame );
        
        // Delete game and exit.
        game_delete( replayGame );
        return SUCCESS;
    }
}