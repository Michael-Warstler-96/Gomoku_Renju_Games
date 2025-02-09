/**
   @file game.h
   @author Michael Warstler (mwwarstl)
   Header file defines game rules and methods that manage game loop. This includes establishing
   a move and game struct to handle game functionality. Other functions include creating, deleting,
   updating, replaying, resuming, and looping game. Also defines function for setting pieces.
*/

#ifndef _GAME_H
#define _GAME_H
#include "board.h"
#include <stdbool.h>
#include <stdlib.h>

/** Represents Gomoku game*/
#define GAME_FREESTYLE 0
/** Represents Renju game */
#define GAME_RENJU 1
/** Game state set to playing */
#define GAME_STATE_PLAYING 0
/** Game state set to forbidden (Renju only) */
#define GAME_STATE_FORBIDDEN 1
/** Game state set to stopped */
#define GAME_STATE_STOPPED 2
/** Game state set to finished */
#define GAME_STATE_FINISHED 3

/**
   x and y are horizontal (column) and vertical (row) coordinates on a board. Origin is the top 
   left corner of the board; this way it is easier to access board.grid. The stone field can 
   either be BLACK_STONE or WHITE_STONE as defined in board.h.
*/
typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char stone;
} move;

/**
   Fields are described as follows:
   board - pointer to a board struct for the current board.
   type - represents game type. (GAME_FREESTYLE or GAME_RENJU)
   stone - which player will be placing NEXT stone. (BLACK_STONE or WHITE_STONE)
   state - 4 values for game state such as ongoing, finished successfully, stopped mid-match
           or game concluded due to forbidden move. (GAME_STATE_PLAYING, GAME_STATE_FINISHED,
           GAME_STATE_STOPPED, GAME_STATE_FORBIDDEN)
   moves - dynamically allocated array of moves, stores moves made so far.
   moves_count - stores how many moves stored in moves.
   moves_capacity - stores current max number of moves that can be stored in moves.
*/
typedef struct {
    board* board;
    unsigned char type;
    unsigned char stone;
    unsigned char state;
    unsigned char winner;
    move* moves;
    size_t moves_count;
    size_t moves_capacity;
} game;

/**
   Creates and returns a new dynamically allocated game struct of the specified game type with
   all field initialized. Board struct is created using board_create().  
   @param board_size is size of board.
   @param game_type is type of game being played.
   @return is pointer to primary game struct.
*/
game* game_create(unsigned char board_size, unsigned char game_type);

/**
   Frees memory of dynamically allocated game struct. Also frees memory of all dynamically
   allocated game struct fields. Board is freed via board_delete().
   If parameter is NULL, program exits with error.
   @param g is pointer to primary game struct.
*/
void game_delete( game* g);

/**
   Controls what happens in the game at each turn. Returns false immediately if game state is not
   GAME_STATE_PLAYING. Otherwise, player is prompted to enter a move (re-prompt if player input is
   invalid - out of bounds or bad format). If EOF is reached/entered, the game is stopped. Once a 
   valid move is input, move is enacted through game_place_stone() and function returns true.
   @param g is pointer to primary game struct.
   @return is false if game is not currently in playing state, otherwise true if valid move input.
*/
bool game_update( game* g);

/**
   Controls game loop by printing current board and calling game_update(). Repeats until 
   game_update() returns false.
   @param g is pointer to primary game struct.
*/
void game_loop( game* g);

/**
   Restarts the game loop for a saved game. If state of parameter game is not GAME_STATE_STOPPED,
   program exits with error. Also exits with error if game type does not match gomoku or renju. If
   no issues, game state is changed to GAME_STATE_PLAYING and game_loop() is called.
   @param g is pointer to primary game struct.
*/
void game_resume( game* g);

/**
   Replays a saved game. Creates a new game struct, and re-makes moves saved in parameter game.
   There is a 1 second pause before making every move.
   @param g is pointer to primary game struct.
*/
void game_replay( game* g);

/**
   Enforces rules of game. Checks if desired intersection is already occupied, if so it prompts 
   the player and returns false. Checks if any win (or draw) conditions are met based on game type
   and rules, if so, move is saved, prompts player, changes game state accordingly, and returns 
   true. If game does not conclude based on move, then move is saved and returns true. Specified
   moves are placed at the end of the game.moves array, which when full, is reallocated and doubled
   in size (game.moves_count and game.moves_capacity are updated accordingly).
   @param g is pointer to primary game struct.
   @param x is horizontal coordinate.
   @param y is vertical coordinate.
   @return is false if stone cannot be placed, true otherwise.
*/
bool game_place_stone( game* g, unsigned char x, unsigned char y);

#endif
