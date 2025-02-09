/**
   @file board.h
   @author Michael Warstler (mwwarstl)
   Header file defines behavior for the board in the gomoku/genju games. This includes establishing
   a game board struct, functions for creating/deleting/printing a board, converting coordinates
   styles, getting grid status at a specific coordinate, placing pieces, and checking if the 
   board is full.
*/

#ifndef _BOARD_H_
#define _BOARD_H_
#include <stdbool.h>

/** Represents an empty spot on the board */
#define EMPTY_INTERSECTION 0
/** Represents a black stone for the board */
#define BLACK_STONE 1
/** Represents a white stone for the board */
#define WHITE_STONE 2
/** Used to clear the terminal display */
#define clear() printf("\033[H\033[J")
/** For 15x15 board size */
#define BOARD_SIZE_15 15   
/** For 17x17 board size */ 
#define BOARD_SIZE_17 17   
/** For 19x19 board size */  
#define BOARD_SIZE_19 19 

/**
   Struct holds behavior for board used in the game. Fields include board size and a dynamically 
   allocated array of size size*size that stores state of all intersections. States could 
   contain EMPTY_INTERSECTION, BLACK_STONE, or WHITE_STONE.
*/   
typedef struct {
    unsigned char size;
    unsigned char* grid;
} board;

/**
   Creates a new dynamically allocated board struct and initializes board.size with the parameter
   size. Initializes board.grid with a new dynamically allocated array and initializes all grid
   intersections with EMPTY_INTERSECTION.
   If invalid size is given, program exits with error code.
   @param size is the size to set the board.
   @return is pointer to board struct created.
*/
board* board_create(unsigned char size);

/**
   Frees memory of dynamically allocated board struct and the board's grid field. 
   If pointer parameter is NULL, program exits with error code.
   @param b is pointer to board struct.
*/
void board_delete(board* b);

/**
   Prints the board. If in_place param is true, terminal is cleared first.
   @param b is board struct holding game.
   @param in_place indicates if the latest piece is placed, thus the latest board can be printed.
*/
void board_print(board* b, bool in_place);

/**
   Converts the horizontal/x and vertical/y coordinates for a board.grid to a "letter + number" 
   formal coordinate, and stores the result in the buffer formal_coord. Returns SUCCESS.
   If x and y coordinates are invalid for board b, returns an error code.
   @param b is pointer to board struct holding game.
   @param x is horizontal coordinate.
   @param y is vertical coordinate.
   @param formal_coord is buffer for formal coordinate.
   @return is SUCCESS if conversion successful. If x and y are invalid, returns COORDINATE_ERR.
*/
unsigned char board_formal_coord(board* b, unsigned char x, unsigned char y, char* formal_coord);

/**
   Convers "letter + number" formal coordinate string formal_coord to horizontal and vertical 
   coordinates for a board.grid, and stores results in x and y, which are passed by reference.
   Finally returns SUCCESS, unless formal_coord is invalid for board b, which would return 
   FORMAL_COORDINATE_ERR instead.
   @param b is pointer to board struct holding game.
   @param formal_coord is string of coordinate in letter + number format.
   @param x is pointer to x/horizontal coordinate.
   @param y is pointer to y/vertical coordinate.
   @return is SUCCESS or FORMAL_COORDINATE_ERR depending on valid coordinates.
*/
unsigned char board_coord(board* b, const char* formal_coord, unsigned char* x, unsigned char* y);

/**
   Returns the intersection occupation state stored in board.grid at the parameter x and y 
   coordinates.
   @param b is pointer to board struct holding game.
   @param x is horizontal coordinate. For array access, it is columns.
   @param y is vertical coordinate. For array access, it is rows.
   @return is intersection occupation state at given coordinates. (Empty, black, white)
*/
unsigned char board_get(board* b, unsigned char x, unsigned char y);

/**
   Stores intersection occupation state (stone param) to a board.grid at the given horizontal and
   vertical coordinate pair.
   If stone is neither BLACK_STONE or WHITE_STONE, program exits with error.
   @param b is pointer to board struct holding game.
   @param x is horizontal coordinate. For array access, it is columns.
   @param y is vertical coordinate. For array access, it is rows.
   @param stone is piece to assign to board.grid.
*/
void board_set(board* b, unsigned char x, unsigned char y, unsigned char stone);

/**
   Determines if current board is full or not.
   @param b is pointer to board struct holding game.
   @return is true if board is full, false otherwise.
*/
bool board_is_full(board* b);

#endif
