/**
   @file io.h
   @author Michael Warstler (mwwarstl)
   Header file for input/output functionality on gomoku/renju programs. This includes 
   importing game from file and exporting game to file.
*/

#ifndef _IO_H_
#define _IO_H_
#include "game.h"

/**
   Import a saved game from file at path parameter. Returns reconstructed game struct.
   If file doesn't exist, can't be read, or doesn't follow specified format, then program
   exits with error.
   @param path is string for file path location
   @return is pointer to primary game struct.
*/
game* game_import(const char* path);

/**
   Exports a game to the file at path parameter. Follows specified Save Game File format.
   If file can't be written, program exits with error.
   @param g is pointer to primary game struct.
   @param path is string for file location to save file to.
*/
void game_export(game* g, const char* path);

#endif