/**
   @file io.c
   @author Michael Warstler (mwwarstl)
   Implementation file for input/output functionality on gomoku/renju programs.
*/

#include "board.h"
#include "game.h"
#include "error-codes.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

/** Max string length allowed excluding the null terminator */
#define MAX_STRING_LENGTH 3

// Import a saved game.
game* game_import(const char* path) 
{
    // Open stream if possible
    FILE *inputStream = fopen( path, "r" );
    if ( inputStream == NULL ) {
        exit( FILE_INPUT_ERR );
    } 
    else {
        // Check first line for proper file format
        char inputLine[MAX_STRING_LENGTH + 1];
        if ( fscanf( inputStream, "%s", inputLine ) != 1 ) {  
            exit( FILE_INPUT_ERR );
        }
        if ( strcmp( inputLine, "GA" ) != 0 ) {
            exit( FILE_INPUT_ERR );
        }
        
        // Check lines 2-5 for correct numerical values.
        int size;
        fscanf( inputStream, "%d", &size );
        if ( size != BOARD_SIZE_15 && size != BOARD_SIZE_17 && size != BOARD_SIZE_19 ) {
            exit( FILE_INPUT_ERR );
        }
        int type;
        fscanf( inputStream, "%d", &type );
        if ( type != GAME_FREESTYLE && type != GAME_RENJU ) {
            exit( FILE_INPUT_ERR );
        }    
        int state;
        fscanf( inputStream, "%d", &state );
        if ( state != GAME_STATE_FORBIDDEN && state != GAME_STATE_STOPPED && state != GAME_STATE_FINISHED ) {
            exit( FILE_INPUT_ERR );
        }
        int winner;
        fscanf( inputStream, "%d", &winner );
        if ( winner != EMPTY_INTERSECTION && winner != BLACK_STONE && winner != WHITE_STONE ) {
            exit( FILE_INPUT_ERR );
        }
        
        // Create game using read in values
        game *importGame = game_create( size, type );
        // Update state and winner fields
        importGame->state = state;
        importGame->winner = winner;
        
        // Begin reading in moves. Place on board if valid.
        char formal_coord[MAX_STRING_LENGTH + 1];
        unsigned char x;
        unsigned char y;
        
        while ( fscanf(inputStream, "%s", formal_coord) != EOF ) {
            // Checks for valid coordinates
            if ( board_coord( importGame->board, formal_coord, &x, &y ) != SUCCESS ) {
                exit( FILE_INPUT_ERR );
            }
            // Place on board - saves moves in process.
            game_place_stone( importGame, x, y );
        }
        
        fclose( inputStream );
        return importGame;
    }
}

// Export a completed or stopped game.
void game_export(game* g, const char* path)
{
    // Establish stream
    FILE *outputStream = fopen( path, "w" );
    if ( outputStream == NULL ) {
        exit( FILE_OUTPUT_ERR );
    }
    else {
        
        // Output first line "GA" magic number
        fprintf( outputStream, "GA\n" );
        
        // Output lines 2-5 using game parameter fields.
        fprintf( outputStream, "%d\n", (int)g->board->size );
        fprintf( outputStream, "%d\n", g->type );
        fprintf( outputStream, "%d\n", g->state );
        fprintf( outputStream, "%d\n", g->winner );
        
        // Print out moves until completion
        for (int i = 0; i < g->moves_count; i++ ) {
            // Convert move coordinates to formal coordinate.
            char formal_coord[MAX_STRING_LENGTH + 1];
            board_formal_coord( g->board, g->moves[i].x, g->moves[i].y, formal_coord );
            fprintf( outputStream, "%s\n", formal_coord );
        }
        fclose( outputStream );
    }
}