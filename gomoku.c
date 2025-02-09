/**
   @file gomoku.c
   @author Michael Warstler (mwwarstl)
   Contains main component of gomoku game. Reads in command line arguments to determine
   if game should be saved to file, started up from existing file, or create a new game
   with custom grid size.
*/

#include "error-codes.h"
#include "game.h"
#include "io.h"      
#include <stdlib.h>
#include <stdio.h>
#include <string.h>    

/** How many command line arguments allowed at minimum */
#define MIN_EXTRA_ARGUMENTS 2
/** How many command line arguments allowed at maximum */
#define MAX_EXTRA_ARGUMENTS 4

/**
   Main function takes in command line arguments to see if game should be saved to file location,
   resumed from previous session, or created new with custom grid size. If too many or conflicting
   arguments are detected, program closes with error. Allowed key arguments include "-o" followed by
   a path location, "-r" followed by a path location, and "-b" followed by the number 15/17/19. Key
   arguments can be used together except for "-r" and "-b".
   @param argc are # of command line arguments.
   @param **argv is array of character pointers listing all arguments.
   @return is exit status
*/
int main(int argc, char **argv)
{
    // Declare an active game pointer.
    game *activeGame;
    
    // Initiale variable setup for import/export paths and board size.
    char *exportPath = NULL;
    char *importPath = NULL;
    unsigned char boardSize = 0;
    
    // Check if total arguments is either 1, 3, or 5
    if ( argc == 1 || argc == MIN_EXTRA_ARGUMENTS + 1 || argc == MAX_EXTRA_ARGUMENTS + 1 ) {
        // loop until 2nd to last argument. Increment by 2 to skip past a key argument's requirement.
        for ( int i = 1; i < argc - 1; i++ ) {
            char * keyArgument = argv[i];
            
            // Check if one of the valid 3 options "-o", "-r", "-b"
            if ( strcmp( keyArgument, "-o" ) == 0 ) {
                exportPath = argv[i + 1];
            }
            else if ( strcmp( keyArgument, "-r" ) == 0 ) {
                // Check if -b (board size) has already been read
                if ( boardSize ) {
                    goto error; // line 87
                }
                importPath = argv[i + 1];
            }
            else if ( strcmp( keyArgument, "-b" ) == 0 ) {
                // Check if next argument is valid size. Set board size if valid.
                boardSize = atoi( argv[i + 1] );
                
                // Check if boardSize is not even a number.
                if ( boardSize == 0 ) {
                    goto error; // line 87
                }
                // Check for valid size.
                else if ( boardSize != BOARD_SIZE_15 && boardSize != BOARD_SIZE_17 && 
                    boardSize != BOARD_SIZE_19 ) {
                    exit( BOARD_SIZE_ERR );
                }
                
                // Check if -r (import) has already been read
                if (importPath != NULL ) {
                    goto error; // line 87
                }
            }
            // Not allowed key argument
            else {
                goto error; // line 87
            }
            // Skip past next argument
            i++;
        }
    }
    // Incorrect argument ammount or invalid combination of arguments.
    else {
        error:
        printf("usage: ./gomoku [-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n");
        printf("       -r and -b conflicts with each other\n");
        exit( ARGUMENT_ERR );
    }
    
    // Resume game from existing file if possible.
    if ( importPath != NULL ) {
        activeGame = game_import( importPath );
        game_resume( activeGame );
    }
    // Otherwise create new game using either default or argument board size.
    else {
        // If boardSize was not set up, set it up for default size 15.
        if ( boardSize == 0 ) {
            boardSize = BOARD_SIZE_15;
        }
        // Otherwise create game with argument size.
        activeGame = game_create( boardSize, GAME_FREESTYLE );
        
        // Loop until game state no longer playing
        while ( activeGame->state == GAME_STATE_PLAYING ) {
            game_loop( activeGame );
        } 
    }
    
    // Game state is no longer playing. Check if game should be saved to file.
    if ( exportPath != NULL ) {
        game_export( activeGame, exportPath );
    }
    
    // Delete game and exit.
    game_delete( activeGame );
    return SUCCESS;
}