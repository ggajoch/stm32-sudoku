/*
 * qqwing - A Sudoku solver and generator
 * Copyright (C) 2006-2011 Stephen Ostermiller
 * http://ostermiller.org/qqwing/
 * Copyright (C) 2007 Jacques Bensimon (jacques@ipm.com)
 * Copyright (C) 2011 Jean Guillerez (j.guillerez - orange.fr)
 * Modified (optimalisation for small uC's) 2013 Piotr Rzeszut (piotrva@wp.pl piotr94.net21.pl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//#include <iostream>
//#include <stdio.h>
#include <stdlib.h>
#include "main.h"

//#include <cstdlib>


class SuddokuBoard;

typedef struct operations{
    unsigned int GIVEN:1;
    unsigned int SINGLE:1;
    unsigned int HIDDEN_SINGLE_ROW:1;
    unsigned int HIDDEN_SINGLE_COLUMN:1;
    unsigned int HIDDEN_SINGLE_SECTION:1;
    unsigned int GUESS:1;
    unsigned int ROLLBACK:1;
    unsigned int NAKED_PAIR_ROW:1;
    unsigned int NAKED_PAIR_COLUMN:1;
    unsigned int NAKED_PAIR_SECTION:1;
    unsigned int POINTING_PAIR_TRIPLE_ROW:1;
    unsigned int POINTING_PAIR_TRIPLE_COLUMN:1;
    unsigned int ROW_BOX:1;
    unsigned int COLUMN_BOX:1;
    unsigned int HIDDEN_PAIR_ROW:1;
    unsigned int HIDDEN_PAIR_COLUMN:1;
    unsigned int HIDDEN_PAIR_SECTION:1;
} Operations;

/**
 * The board containing all the memory structures and
 * methods for solving or generating sudoku puzzles.
 */
class SudokuBoard {
    public:
			
			/**
         * The 81 integers that make up a sudoku puzzle.
         * Givens are 1-9, unknows are 0.
         * Once initialized, this puzzle remains as is.
         * The answer is worked out in "solution".
         */
        unsigned char* puzzle;

        /**
         * The 81 integers that make up a sudoku puzzle.
         * The solution is built here, after completion
         * all will be 1-9.
         */
        unsigned char* solution;
			
        enum PrintStyle {
            ONE_LINE,
            COMPACT,
            READABLE,
            CSV,
        };
        enum Difficulty {
            UNKNOWN,
            SIMPLE,
            EASY,
            INTERMEDIATE,
            EXPERT,
        };
        SudokuBoard();
        void printPuzzle();
        void printSolution();
        bool solve();
        int countSolutions();
        void printPossibilities();
        bool isSolved();
        void setRecordHistory(bool recHistory);
        void setPrintStyle(PrintStyle ps);
        bool generatePuzzle();
        int getGivenCount();
        int getSingleCount();
        int getHiddenSingleCount();
        int getNakedPairCount();
        int getHiddenPairCount();
        int getBoxLineReductionCount();
        int getPointingPairTripleCount();
        int getGuessCount();
        int getBacktrackCount();
        SudokuBoard::Difficulty getDifficulty();
        ~SudokuBoard();

    private:
        Operations history[81*2];

        /**
         * Recursion depth at which each of the numbers
         * in the solution were placed.  Useful for backing
         * out solve branches that don't lead to a solution.
         */
        int* solutionRound;

        /**
         * The 729 integers that make up a the possible
         * values for a suduko puzzle. (9 possibilities
         * for each of 81 squares).  If possibilities[i]
         * is zero, then the possibility could still be
         * filled in according to the sudoku rules.  When
         * a possibility is eliminated, possibilities[i]
         * is assigned the round (recursion level) at
         * which it was determined that it could not be
         * a possibility.
         */
        unsigned char* possibilities;

        /**
         * An array the size of the board (81) containing each
         * of the numbers 0-n exactly once.  This array may
         * be shuffled so that operations that need to
         * look at each cell can do so in a random order.
         */
        unsigned char* randomBoardArray;

        /**
         * An array with one element for each position (9), in
         * some random order to be used when trying each
         * position in turn during guesses.
         */
        unsigned char* randomPossibilityArray;

        /**
         * Whether or not to record history
         */
        bool recordHistory;

        /**
         * The style with which to print puzzles and solutions
         */
        PrintStyle printStyle;

        /**
         * The last round of solving
         */
        int lastSolveRound;
        bool reset();
        bool singleSolveMove(int round);
        bool onlyPossibilityForCell(int round);
        bool onlyValueInRow(int round);
        bool onlyValueInColumn(int round);
        bool onlyValueInSection(int round);
        bool solve(int round);
        int countSolutions(int round, bool limitToTwo);
        bool guess(int round, int guessNumber);
        bool isImpossible();
        void rollbackRound(int round);
        bool pointingRowReduction(int round);
        bool rowBoxReduction(int round);
        bool colBoxReduction(int round);
        bool pointingColumnReduction(int round);
        bool hiddenPairInRow(int round);
        bool hiddenPairInColumn(int round);
        bool hiddenPairInSection(int round);
        void mark(int position, int round, int value);
        int findPositionWithFewestPossibilities();
        bool handleNakedPairs(int round);
        int countPossibilities(int position);
        bool arePossibilitiesSame(int position1, int position2);
        void markRandomPossibility(int round);
        void shuffleRandomArrays();
        void print(unsigned char* sudoku);
        void rollbackNonGuesses();
        void clearPuzzle();
        bool removePossibilitiesInOneFromTwo(int position1, int position2, int round);

};

void shuffleArray(unsigned char* array, int size);
static inline int cellToColumn(int cell);
static inline int cellToRow(int cell);
static inline int cellToSectionStartCell(int cell);
static inline int cellToSection(int cell);
static inline int rowToFirstCell(int row);
static inline int columnToFirstCell(int column);
static inline int sectionToFirstCell(int section);
static inline int getPossibilityIndex(int valueIndex, int cell);
static inline int rowColumnToCell(int row, int column);
static inline int sectionToCell(int section, int offset);

#define GRID_SIZE 3
#define ROW_LENGTH          (GRID_SIZE*GRID_SIZE)
#define COL_HEIGHT          (GRID_SIZE*GRID_SIZE)
#define SEC_SIZE            (GRID_SIZE*GRID_SIZE)
#define SEC_COUNT           (GRID_SIZE*GRID_SIZE)
#define SEC_GROUP_SIZE      (SEC_SIZE*GRID_SIZE)
#define NUM_POSS            (GRID_SIZE*GRID_SIZE)
#define BOARD_SIZE          (ROW_LENGTH*COL_HEIGHT)
#define POSSIBILITY_SIZE    (BOARD_SIZE*NUM_POSS)
