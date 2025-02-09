/**
   @file board.c
   @author Michael Warstler (mwwarstl)
   Implementation file for board functionality on gomoku and renju games. This includes 
   creation/deletion of a board of size 15/17/19, displaying the board, conversion of coordinate
   types, setting stones, and checking when full.
*/

#include "board.h"
#include "error-codes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

/** Smallest 2 digit number possible */
#define LOWEST_TWO_DIGITS 10
/** Longest string length allowed (not including null terminator */
#define MAX_STRING_LENGTH 3

// Create a board struct and return pointer to it.
board* board_create(unsigned char size)
{
    // allocate board space
    board *b = (board *)malloc( sizeof( board ) );
    
    // Confirm valid size 15/17/19
    if ( size != BOARD_SIZE_15 && size != BOARD_SIZE_17 && size != BOARD_SIZE_19 ) { 
        exit( BOARD_SIZE_ERR );
    }
    
    // Set struct values.
    b->size = size;
    
    // Allocate dynamic memory - size x size grid of space required.
    b->grid = ( unsigned char (*) )malloc( b->size * b->size * sizeof( unsigned char ) ) ;
    
    // Added......................................................
    unsigned char (*grid)[b->size] = ( unsigned char (*)[b->size] ) b->grid; 
    
    // Initialize grid
    for ( int i = 0; i < size; i++ ) {
        for ( int j = 0; j < size; j++ ) {
            grid[i][j] = EMPTY_INTERSECTION; 
        }
    }
    return b;
}

// Free memory of board struct and board.grid.
void board_delete( board* b)
{
    if ( b == NULL ) {
        exit( NULL_POINTER_ERR );
    }
    free( b->grid );                    
    free( b );
}

// Prints the board.
void board_print( board* b, bool in_place)
{
    // Clear terminal if in_place is true
    if ( in_place ) {
        clear();
    }
    
    // Cast grid pointer to a 2D array.
    unsigned char (*grid)[b->size] = ( unsigned char (*)[b->size] ) b->grid; 
    
    // Print board 
    for ( int i = 0; i < b->size; i++ ) {
        printf( "%2d ", b->size - i );    // print row number
        // Go through row and print out grid values
        for ( int j = 0; j < b->size; j++ ) {
            if ( grid[i][j] == EMPTY_INTERSECTION ) {
                printf( "+" );
            }
            else if ( grid[i][j] == BLACK_STONE ) {
                printf( "\u25CF" );
            }
            else {  // WHITE_STONE
                printf( "\u25CB" );
            }
            // Don't print dash if at last column
            if ( j < b->size - 1 ) {
                printf( "-" );
            }
        }
        printf( "\n" );
    }
    // Print letters for columns
    printf( "   " );
    unsigned char ch;
    for ( ch = 'A'; ch < 'A' + b->size - 1; ch++ ) {
        printf( "%c ", ch );
    }
    // Fence post for last letter.
    printf( "%c\n", ch);
}

// Converts x and y cordinates to formal letter+number format.                                                             
unsigned char board_formal_coord( board* b, unsigned char x, unsigned char y, char* formal_coord)
{
    // Error check. 
    if ( x >= b->size || y >= b->size || x < 0 || y < 0 ) {
        return COORDINATE_ERR;
    }
    
    // convert x and y coordinates to formal_coord string 
    // Convert x to ascii letter
    x += 'A';
    formal_coord[0] = x;
    
    // Convert y to proper DISPLAY row. (x,y) origin 0,0 is at top left of grid. Y axis for display 0, starts on bottom left.
    y = b->size - y;
    // If y is on a row with 2 digits, it must be split up to fit in a string.
    if ( y < LOWEST_TWO_DIGITS ) {
        y += '0';   // convert prev y value to a char representation of that number.
        formal_coord[1] = y;
        formal_coord[2] = '\0';
    }
    else { // y is 10 or greater
        formal_coord[1] = '1';  // First character will always be '1'.
        formal_coord[2] = y - LOWEST_TWO_DIGITS + '0';    // Convert ones digit of y coordinate to its literal character.
    }
    
    return SUCCESS;
}

// Converts formal coordinate to x and y.
unsigned char board_coord( board* b, const char* formal_coord, unsigned char* x, unsigned char* y)
{
    int row;    // used to get multiple characters at once without converting ASCII.
    // Check for correct # of matches for input.
    if ( sscanf( formal_coord, "%c%d", x, &row ) != 2 ) {
        return FORMAL_COORDINATE_ERR;
    }
    
    // Error Check - 1st character is letter, following characters are numbers.
    if ( *x < 'A' || *x >= 'A' + b->size || 
         row < 1 || row > b->size || strlen( formal_coord ) > MAX_STRING_LENGTH ) {
             return FORMAL_COORDINATE_ERR;
    }
    
    // Convert formal coordinates to x and y values/indeces for grid.
    *x = formal_coord[0] - 'A'; // get letter into number range 0-size.
    *y = b->size - row;  // get "numbers" from coordinate into one singular value. Subtract from size to get correct "orientation" since (0,0) with (x,y) starts at top left.
    
    return SUCCESS;
}

// Return intersection occupation state from given coordinates.
unsigned char board_get( board* b, unsigned char x, unsigned char y)
{
    // Cast grid pointer to 2D array. Return stone.
    unsigned char (*grid)[b->size] = ( unsigned char (*)[b->size] ) b->grid; 
    return grid[y][x];  
}

// Set grid coordinates to stone parameter.
void board_set( board* b, unsigned char x, unsigned char y, unsigned char stone)
{
    // Error Check
    if ( stone != BLACK_STONE && stone != WHITE_STONE ) {
        exit( STONE_TYPE_ERR );
    }
    
    // Cast grid pointer to 2D array. Set stone.
    unsigned char (*grid)[b->size] = ( unsigned char (*)[b->size] ) b->grid; 
    grid[y][x] = stone;
}

// Return true if all board intersections are filled with stones, otherwise false.
bool board_is_full( board* b) 
{
    // Cast grid pointer to 2D array.
    unsigned char (*grid)[b->size] = ( unsigned char (*)[b->size] ) b->grid; 
    
    // Loop through grid, checking for an empty intersection.
    for ( int i = 0; i < b->size; i++ ) {
        for ( int j = 0; j < b->size; j++) {
            if ( grid[i][j] == EMPTY_INTERSECTION ) {
                return false;
            }
        }
    }
    return true;
}