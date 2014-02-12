#include "sudoku.h"
#include "boardStruct.h"
#include <inttypes.h>

#include <stdio.h>

#define usart_str printf

#include "intermediate.h"
Board table[100];


int main(void)
{
  Board tmp;
  for(uint8_t i = 0; i < 3; ++i)
  {
    tmp = Board(T1[i]);
    tmp.print();
  }
  return 0;
	SudokuBoard::Difficulty difficulty = SudokuBoard::INTERMEDIATE;

  // Initialize the random number generator
  int timeSeed = 0;
  srand(timeSeed);

  // Create a new puzzle board
  // and set the options
  SudokuBoard* ss = new SudokuBoard();
	/*SudokuBoard sss;
	SudokuBoard* ss = &sss;*/
  // Solve puzzle or generate puzzles
  // until end of input for solving, or
  // until we have generated the specified number.
	int number_to_benchmark=1000;
  bool done = false;
  while (!done){

      // Record whether the puzzle was possible or not,
      // so that we don't try to solve impossible givens.
      bool havePuzzle = false;
      // Generate a puzzle
      havePuzzle = ss->generatePuzzle();
      if (!havePuzzle){
          //cout << "Could not generate puzzle.";
          //cout << endl;
				usart_str("Could not generate puzzle.");
				usart_str("\r\n");
      }

      if (havePuzzle){

          // Count the solutions if requested.
          // (Must be done before solving, as it would
          // mess up the stats.)

          // Solve the puzzle
          ss->solve();

          // Bail out if it didn't meet the difficulty standards for generation
          if (difficulty!=SudokuBoard::UNKNOWN && difficulty!=ss->getDifficulty()){
              havePuzzle = false;
          } else {
              // Set loop to terminate if enough have been generated.
							if(!(--number_to_benchmark)) done = true;
							srand(number_to_benchmark);
							//char bufor[5];
							//sprintf(bufor,"%d",1000-number_to_benchmark);
							//usart_str(bufor);
						  //usart_str("\r\n");
          }
      }

      if (havePuzzle){
          //table
          // Print the puzzle itself.
          //ss->printPuzzle();

          //ss->printSolution();
          Board tmp(ss->puzzle,ss->solution);
          tmp.print();
          printf(",\n");
      }
  }
	
}