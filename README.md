# Gomoku_Renju_Games
Contains C project of Gomoku and Renju games and a replay program that allows users to replay a saved game. Gomoku and Renju are both games where two players alternate placing colored stones on a board to attempt five connections in a row. Renju is functionally the same as Gomoku except with several additional rules. Games involve UI, save states, file I/O, error conditions, and score keeping.

HOW TO RUN: 
1. These programs are built with a Make file. Inside the command prompt inside the project directory type the following: $ make    

2. User can run one of the three programs (gomoku, renju, replay)

3. All 3 programs begin with shell command ./gomoku, ./renju./ or ./replay followed by additional command line arguments. Gomoku and renju require several command line arguments including to see if game should be 
	       saved to file location, resumed from previous session, or created new with custom grid size. If too many or conflicting arguments are detected, program closes with error. Allowed key arguments include "-o"
         followed by a path location, "-r" followed by a path location, and "-b" followed by the number 15/17/19. Key arguments can be used together except for "-r" and "-b". The replay program only needs 1 extra command 
	       line argument: a file path location. 

