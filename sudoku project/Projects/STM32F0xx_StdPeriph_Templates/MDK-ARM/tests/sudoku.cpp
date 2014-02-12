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
#include <cstdio>
#include "sudoku.h"
 
 
#define usart_str printf
/**
 * Create a new Sudoku board
 */
SudokuBoard::SudokuBoard(){
    puzzle = new unsigned char[BOARD_SIZE];
    solution = new unsigned char[BOARD_SIZE];
    solutionRound = new int[BOARD_SIZE];
    possibilities = new unsigned char[POSSIBILITY_SIZE];
    recordHistory = true;
    printStyle = READABLE;
    randomBoardArray = new unsigned char[BOARD_SIZE];
    randomPossibilityArray = new unsigned char[NUM_POSS];
		/*puzzle =  (int*)malloc(sizeof(int)*BOARD_SIZE);
    solution = (int*)malloc(sizeof(int)*BOARD_SIZE);
    solutionRound = (int*)malloc(sizeof(int)*BOARD_SIZE);
    possibilities = (int*)malloc(sizeof(int)*POSSIBILITY_SIZE);
    recordHistory = true;
    printStyle = READABLE;
    randomBoardArray = (int*)malloc(sizeof(int)*BOARD_SIZE);
    randomPossibilityArray = (int*)malloc(sizeof(int)*NUM_POSS);*/
    
    /*
    Kasowanie tablicy binarnej z histori¹ operacji (zamiast listy dynamicznej)
    */
    for(int i=0;i<81*2;i++){
        history[i].GIVEN=0;
        history[i].SINGLE=0;
        history[i].HIDDEN_SINGLE_ROW=0;
        history[i].HIDDEN_SINGLE_COLUMN=0;
        history[i].HIDDEN_SINGLE_SECTION=0;
        history[i].GUESS=0;
        history[i].ROLLBACK=0;
        history[i].NAKED_PAIR_ROW=0;
        history[i].NAKED_PAIR_COLUMN=0;
        history[i].NAKED_PAIR_SECTION=0;
        history[i].POINTING_PAIR_TRIPLE_ROW=0;
        history[i].POINTING_PAIR_TRIPLE_COLUMN=0;
        history[i].ROW_BOX=0;
        history[i].COLUMN_BOX=0;
        history[i].HIDDEN_PAIR_ROW=0;
        history[i].HIDDEN_PAIR_COLUMN=0;
        history[i].HIDDEN_PAIR_SECTION=0;    
    }
    
    {for (int i=0; i<BOARD_SIZE; i++){
        randomBoardArray[i] = i;
    }}
    {for (int i=0; i<NUM_POSS; i++){
        randomPossibilityArray[i] = i;
    }}
}

/**
 * Get the number of cells that are
 * set in the puzzle (as opposed to
 * figured out in the solution
 */
int SudokuBoard::getGivenCount(){
    int count = 0;
    {for (int i=0; i<BOARD_SIZE; i++){
        if (puzzle[i] != 0) count++;
    }}
    return count;
}

/**
 * Reset the board to its initial state with
 * only the givens.
 * This method clears any solution, resets statistics,
 * and clears any history messages.
 */
bool SudokuBoard::reset(){
    {for (int i=0; i<BOARD_SIZE; i++){
        solution[i] = 0;
    }}
    {for (int i=0; i<BOARD_SIZE; i++){
        solutionRound[i] = 0;
    }}
    {for (int i=0; i<POSSIBILITY_SIZE; i++){
        possibilities[i] = 0;
    }}
    
    /*
    Kasowanie tablicy binarnej z histori¹ operacji (zamiast listy dynamicznej)
    */
    for(int i=0;i<81*2;i++){
        history[i].GIVEN=0;
        history[i].SINGLE=0;
        history[i].HIDDEN_SINGLE_ROW=0;
        history[i].HIDDEN_SINGLE_COLUMN=0;
        history[i].HIDDEN_SINGLE_SECTION=0;
        history[i].GUESS=0;
        history[i].ROLLBACK=0;
        history[i].NAKED_PAIR_ROW=0;
        history[i].NAKED_PAIR_COLUMN=0;
        history[i].NAKED_PAIR_SECTION=0;
        history[i].POINTING_PAIR_TRIPLE_ROW=0;
        history[i].POINTING_PAIR_TRIPLE_COLUMN=0;
        history[i].ROW_BOX=0;
        history[i].COLUMN_BOX=0;
        history[i].HIDDEN_PAIR_ROW=0;
        history[i].HIDDEN_PAIR_COLUMN=0;
        history[i].HIDDEN_PAIR_SECTION=0;    
    }

    int round = 1;
    for (int position=0; position<BOARD_SIZE; position++){
        if (puzzle[position] > 0){
            int valIndex = puzzle[position]-1;
            int valPos = getPossibilityIndex(valIndex,position);
            int value = puzzle[position];
            if (possibilities[valPos] != 0) return false;
            mark(position,round,value);
            if (recordHistory) history[round].GIVEN=1;
        }
    }

    return true;
}

/**
 * Get the difficulty rating.
 */
SudokuBoard::Difficulty SudokuBoard::getDifficulty(){
    if (getGuessCount() > 0) return SudokuBoard::EXPERT;
    if (getBoxLineReductionCount() > 0) return SudokuBoard::INTERMEDIATE;
    if (getPointingPairTripleCount() > 0) return SudokuBoard::INTERMEDIATE;
    if (getHiddenPairCount() > 0) return SudokuBoard::INTERMEDIATE;
    if (getNakedPairCount() > 0) return SudokuBoard::INTERMEDIATE;
    if (getHiddenSingleCount() > 0) return SudokuBoard::EASY;
    if (getSingleCount() > 0) return SudokuBoard::SIMPLE;
    return SudokuBoard::UNKNOWN;
}

/**
 * Get the number of cells for which the solution was determined
 * because there was only one possible value for that cell.
 */
int SudokuBoard::getSingleCount(){
    //return getLogCount(solveInstructions, LogItem::SINGLE);
    for(int i=0;i<81*2;i++){
        if(history[i].SINGLE==1)return 1;    
    }
    return 0;
}

/**
 * Get the number of cells for which the solution was determined
 * because that cell had the only possibility for some value in
 * the row, column, or section.
 */
int SudokuBoard::getHiddenSingleCount(){
    /*return getLogCount(solveInstructions, LogItem::HIDDEN_SINGLE_ROW) +
            getLogCount(solveInstructions, LogItem::HIDDEN_SINGLE_COLUMN) +
            getLogCount(solveInstructions, LogItem::HIDDEN_SINGLE_SECTION);*/
    for(int i=0;i<81*2;i++){
        if(history[i].HIDDEN_SINGLE_ROW==1)return 1; 
        if(history[i].HIDDEN_SINGLE_COLUMN==1)return 1; 
        if(history[i].HIDDEN_SINGLE_SECTION==1)return 1;    
    }
    return 0;
}

/**
 * Get the number of naked pair reductions that were performed
 * in solving this puzzle.
 */
int SudokuBoard::getNakedPairCount(){
    /*return getLogCount(solveInstructions, LogItem::NAKED_PAIR_ROW) +
            getLogCount(solveInstructions, LogItem::NAKED_PAIR_COLUMN) +
            getLogCount(solveInstructions, LogItem::NAKED_PAIR_SECTION);*/
    for(int i=0;i<81*2;i++){
        if(history[i].NAKED_PAIR_ROW==1)return 1;
        if(history[i].NAKED_PAIR_COLUMN==1)return 1;
        if(history[i].NAKED_PAIR_SECTION==1)return 1;    
    }
    return 0;
}

/**
 * Get the number of hidden pair reductions that were performed
 * in solving this puzzle.
 */
int SudokuBoard::getHiddenPairCount(){
    /*return getLogCount(solveInstructions, LogItem::HIDDEN_PAIR_ROW) +
            getLogCount(solveInstructions, LogItem::HIDDEN_PAIR_COLUMN) +
            getLogCount(solveInstructions, LogItem::HIDDEN_PAIR_SECTION);*/
    for(int i=0;i<81*2;i++){
        if(history[i].HIDDEN_PAIR_ROW==1)return 1;
        if(history[i].HIDDEN_PAIR_COLUMN==1)return 1;
        if(history[i].HIDDEN_PAIR_SECTION==1)return 1;    
    }
    return 0;
}

/**
 * Get the number of pointing pair/triple reductions that were performed
 * in solving this puzzle.
 */
int SudokuBoard::getPointingPairTripleCount(){
    /*return getLogCount(solveInstructions, LogItem::POINTING_PAIR_TRIPLE_ROW)+
        getLogCount(solveInstructions, LogItem::POINTING_PAIR_TRIPLE_COLUMN);*/
    for(int i=0;i<81*2;i++){
        if(history[i].POINTING_PAIR_TRIPLE_ROW==1)return 1;
        if(history[i].POINTING_PAIR_TRIPLE_COLUMN==1)return 1;  
    }
    return 0;    
}

/**
 * Get the number of box/line reductions that were performed
 * in solving this puzzle.
 */
int SudokuBoard::getBoxLineReductionCount(){
    /*return getLogCount(solveInstructions, LogItem::ROW_BOX)+
        getLogCount(solveInstructions, LogItem::COLUMN_BOX);*/
    for(int i=0;i<81*2;i++){
        if(history[i].ROW_BOX==1)return 1;
        if(history[i].COLUMN_BOX==1)return 1;  
    }
    return 0; 
}

/**
 * Get the number lucky guesses in solving this puzzle.
 */
int SudokuBoard::getGuessCount(){
    /*return getLogCount(solveInstructions, LogItem::GUESS);*/
    for(int i=0;i<81*2;i++){
        if(history[i].GUESS==1)return 1;
    }
    return 0;
}

/**
 * Get the number of backtracks (unlucky guesses) required
 * when solving this puzzle.
 */
int SudokuBoard::getBacktrackCount(){
    /*return getLogCount(solveHistory, LogItem::ROLLBACK);*/
    for(int i=0;i<81*2;i++){
        if(history[i].ROLLBACK==1)return 1;
    }
    return 0;
}

void SudokuBoard::markRandomPossibility(int round){
    int remainingPossibilities = 0;
    {for (int i=0; i<POSSIBILITY_SIZE; i++){
        if (possibilities[i] == 0) remainingPossibilities++;
    }}

    int randomPossibility = rand()%remainingPossibilities;

    int possibilityToMark = 0;
    {for (int i=0; i<POSSIBILITY_SIZE; i++){
        if (possibilities[i] == 0){
            if (possibilityToMark == randomPossibility){
                int position = i/NUM_POSS;
                int value = i%NUM_POSS+1;
                mark(position, round, value);
                return;
            }
            possibilityToMark++;
        }
    }}
}

void SudokuBoard::shuffleRandomArrays(){
    shuffleArray(randomBoardArray, BOARD_SIZE);
    shuffleArray(randomPossibilityArray, NUM_POSS);
}

void SudokuBoard::clearPuzzle(){
    // Clear any existing puzzle
    {for (int i=0; i<BOARD_SIZE; i++){
        puzzle[i] = 0;
    }}
    reset();
}

bool SudokuBoard::generatePuzzle(){

    // Don't record history while generating.
    bool recHistory = recordHistory;
    setRecordHistory(false);

    clearPuzzle();

    // Start by getting the randomness in order so that
    // each puzzle will be different from the last.
    shuffleRandomArrays();

    // Now solve the puzzle the whole way.  The solve
    // uses random algorithms, so we should have a
    // really randomly totally filled sudoku
    // Even when starting from an empty grid
    solve();

    // Rollback any square for which it is obvious that
    // the square doesn't contribute to a unique solution
    // (ie, squares that were filled by logic rather
    // than by guess)
    rollbackNonGuesses();

    // Record all marked squares as the puzzle so
    // that we can call countSolutions without losing it.
    {for (int i=0; i<BOARD_SIZE; i++){
        puzzle[i] = solution[i];
    }}

    // Rerandomize everything so that we test squares
    // in a different order than they were added.
    shuffleRandomArrays();

    // Remove one value at a time and see if
    // the puzzle still has only one solution.
    // If it does, leave it0 out the point because
    // it is not needed.
    {for (int i=0; i<BOARD_SIZE; i++){
        // check all the positions, but in shuffled order
        int position = randomBoardArray[i];
        if (puzzle[position] > 0){
            // try backing out the value and
            // counting solutions to the puzzle
            int savedValue = puzzle[position];
            puzzle[position] = 0;
            reset();
            if (countSolutions(2, true) > 1){
                // Put it back in, it is needed
                puzzle[position] = savedValue;
            }
        }
    }}

    // Clear all solution info, leaving just the puzzle.
    reset();

    // Restore recording history.
    setRecordHistory(recHistory);
    
    
    
    

    return true;

}

void SudokuBoard::rollbackNonGuesses(){
    // Guesses are odd rounds
    // Non-guesses are even rounds
    {for (int i=2; i<=lastSolveRound; i+=2){
        rollbackRound(i);
    }}
}

void SudokuBoard::setPrintStyle(PrintStyle ps){
    printStyle = ps;
}

void SudokuBoard::setRecordHistory(bool recHistory){
    recordHistory = recHistory;
}

bool SudokuBoard::solve(){
    reset();
    shuffleRandomArrays();
    return solve(2);
}

//int max_round=0;

bool SudokuBoard::solve(int round){
    lastSolveRound = round;
    /*if(round>max_round){
        max_round=round;
        cout<<max_round<<";";    
    }*/

    while (singleSolveMove(round)){
        if (isSolved()) return true;
        if (isImpossible()) return false;
    }

    int nextGuessRound = round+1;
    int nextRound = round+2;
    for (int guessNumber=0; guess(nextGuessRound, guessNumber); guessNumber++){
        if (isImpossible() || !solve(nextRound)){
            rollbackRound(nextRound);
            rollbackRound(nextGuessRound);
        } else {
            return true;
        }
    }
    return false;
}

int SudokuBoard::countSolutions(){
    // Don't record history while generating.
    bool recHistory = recordHistory;
    setRecordHistory(false);

    reset();
    int solutionCount = countSolutions(2, false);

    // Restore recording history.
    setRecordHistory(recHistory);

    return solutionCount;
}

int SudokuBoard::countSolutions(int round, bool limitToTwo){
    while (singleSolveMove(round)){
        if (isSolved()){
            rollbackRound(round);
            return 1;
        }
        if (isImpossible()){
            rollbackRound(round);
            return 0;
        }
    }

    int solutions = 0;
    int nextRound = round+1;
    for (int guessNumber=0; guess(nextRound, guessNumber); guessNumber++){
        solutions += countSolutions(nextRound, limitToTwo);
        if (limitToTwo && solutions >=2){
            rollbackRound(round);
            return solutions;
        }
    }
    rollbackRound(round);
    return solutions;
}

void SudokuBoard::rollbackRound(int round){
    if (recordHistory) history[round].ROLLBACK=1;
    {for (int i=0; i<BOARD_SIZE; i++){
        if (solutionRound[i] == round){
            solutionRound[i] = 0;
            solution[i] = 0;
        }
    }}
    {for (int i=0; i<POSSIBILITY_SIZE; i++){
        if (possibilities[i] == round){
            possibilities[i] = 0;
        }
    }}
    /*
    kasujê dane o operacjach z tej rundy
    */
    history[round].GIVEN=0;
    history[round].SINGLE=0;
    history[round].HIDDEN_SINGLE_ROW=0;
    history[round].HIDDEN_SINGLE_COLUMN=0;
    history[round].HIDDEN_SINGLE_SECTION=0;
    history[round].GUESS=0;
    history[round].ROLLBACK=0;
    history[round].NAKED_PAIR_ROW=0;
    history[round].NAKED_PAIR_COLUMN=0;
    history[round].NAKED_PAIR_SECTION=0;
    history[round].POINTING_PAIR_TRIPLE_ROW=0;
    history[round].POINTING_PAIR_TRIPLE_COLUMN=0;
    history[round].ROW_BOX=0;
    history[round].COLUMN_BOX=0;
    history[round].HIDDEN_PAIR_ROW=0;
    history[round].HIDDEN_PAIR_COLUMN=0;
    history[round].HIDDEN_PAIR_SECTION=0; 
}

bool SudokuBoard::isSolved(){
    {for (int i=0; i<BOARD_SIZE; i++){
        if (solution[i] == 0){
            return false;
        }
    }}
    return true;
}

bool SudokuBoard::isImpossible(){
    for (int position=0; position<BOARD_SIZE; position++){
        if (solution[position] == 0){
            int count = 0;
            for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0) count++;
            }
            if (count == 0) {
                return true;
            }
        }
    }
    return false;
}

int SudokuBoard::findPositionWithFewestPossibilities(){
    int minPossibilities = 10;
    int bestPosition = 0;
    {for (int i=0; i<BOARD_SIZE; i++){
        int position = randomBoardArray[i];
        if (solution[position] == 0){
            int count = 0;
            for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0) count++;
            }
            if (count < minPossibilities){
                minPossibilities = count;
                bestPosition = position;
            }
        }
    }}
    return bestPosition;
}

bool SudokuBoard::guess(int round, int guessNumber){
    int localGuessCount = 0;
    int position = findPositionWithFewestPossibilities();
    {for (int i=0; i<NUM_POSS; i++){
        int valIndex = randomPossibilityArray[i];
        int valPos = getPossibilityIndex(valIndex,position);
        if (possibilities[valPos] == 0){
            if (localGuessCount == guessNumber){
                int value = valIndex+1;
                if (recordHistory) history[round].GUESS=1;
                
                mark(position, round, value);
                return true;
            }
            localGuessCount++;
        }
    }}
    return false;
}

bool SudokuBoard::singleSolveMove(int round){
    if (onlyPossibilityForCell(round)) return true;
    if (onlyValueInSection(round)) return true;
    if (onlyValueInRow(round)) return true;
    if (onlyValueInColumn(round)) return true;
    if (handleNakedPairs(round)) return true;
    if (pointingRowReduction(round)) return true;
    if (pointingColumnReduction(round)) return true;
    if (rowBoxReduction(round)) return true;
    if (colBoxReduction(round)) return true;
    if (hiddenPairInRow(round)) return true;
    if (hiddenPairInColumn(round)) return true;
    if (hiddenPairInSection(round)) return true;
    return false;
}

bool SudokuBoard::colBoxReduction(int round){
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        for (int col=0; col<9; col++){
            int colStart = columnToFirstCell(col);
            bool inOneBox = true;
            int colBox = -1;
            {for (int i=0; i<3; i++){
                for (int j=0; j<3; j++){
                    int row = i*3+j;
                    int position = rowColumnToCell(row, col);
                    int valPos = getPossibilityIndex(valIndex,position);
                    if(possibilities[valPos] == 0){
                        if (colBox == -1 || colBox == i){
                            colBox = i;
                        } else {
                            inOneBox = false;
                        }
                    }

                }
            }}
            if (inOneBox && colBox != -1){
                bool doneSomething = false;
                int row = 3*colBox;
                int secStart = cellToSectionStartCell(rowColumnToCell(row, col));
                int secStartRow = cellToRow(secStart);
                int secStartCol = cellToColumn(secStart);
                {for (int i=0; i<3; i++){
                    for (int j=0; j<3; j++){
                        int row2 = secStartRow+i;
                        int col2 = secStartCol+j;
                        int position = rowColumnToCell(row2, col2);
                        int valPos = getPossibilityIndex(valIndex,position);
                        if (col != col2 && possibilities[valPos] == 0){
                            possibilities[valPos] = round;
                            doneSomething = true;
                        }
                    }
                }}
                if (doneSomething){
                    if (recordHistory) history[round].COLUMN_BOX=1;
                    
                    
                    return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::rowBoxReduction(int round){
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        for (int row=0; row<9; row++){
            int rowStart = rowToFirstCell(row);
            bool inOneBox = true;
            int rowBox = -1;
            {for (int i=0; i<3; i++){
                for (int j=0; j<3; j++){
                    int column = i*3+j;
                    int position = rowColumnToCell(row, column);
                    int valPos = getPossibilityIndex(valIndex,position);
                    if(possibilities[valPos] == 0){
                        if (rowBox == -1 || rowBox == i){
                            rowBox = i;
                        } else {
                            inOneBox = false;
                        }
                    }

                }
            }}
            if (inOneBox && rowBox != -1){
                bool doneSomething = false;
                int column = 3*rowBox;
                int secStart = cellToSectionStartCell(rowColumnToCell(row, column));
                int secStartRow = cellToRow(secStart);
                int secStartCol = cellToColumn(secStart);
                {for (int i=0; i<3; i++){
                    for (int j=0; j<3; j++){
                        int row2 = secStartRow+i;
                        int col2 = secStartCol+j;
                        int position = rowColumnToCell(row2, col2);
                        int valPos = getPossibilityIndex(valIndex,position);
                        if (row != row2 && possibilities[valPos] == 0){
                            possibilities[valPos] = round;
                            doneSomething = true;
                        }
                    }
                }}
                if (doneSomething){
                    if (recordHistory) history[round].ROW_BOX=1;
                    return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::pointingRowReduction(int round){
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        for (int section=0; section<9; section++){
            int secStart = sectionToFirstCell(section);
            bool inOneRow = true;
            int boxRow = -1;
            for (int j=0; j<3; j++){
                {for (int i=0; i<3; i++){
                    int secVal=secStart+i+(9*j);
                    int valPos = getPossibilityIndex(valIndex,secVal);
                    if(possibilities[valPos] == 0){
                        if (boxRow == -1 || boxRow == j){
                            boxRow = j;
                        } else {
                            inOneRow = false;
                        }
                    }
                }}
            }
            if (inOneRow && boxRow != -1){
                bool doneSomething = false;
                int row = cellToRow(secStart) + boxRow;
                int rowStart = rowToFirstCell(row);

                {for (int i=0; i<9; i++){
                    int position = rowStart+i;
                    int section2 = cellToSection(position);
                    int valPos = getPossibilityIndex(valIndex,position);
                    if (section != section2 && possibilities[valPos] == 0){
                        possibilities[valPos] = round;
                        doneSomething = true;
                    }
                }}
                if (doneSomething){
                    if (recordHistory) history[round].POINTING_PAIR_TRIPLE_ROW=1;
                    
                    return true;
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::pointingColumnReduction(int round){
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        for (int section=0; section<9; section++){
            int secStart = sectionToFirstCell(section);
            bool inOneCol = true;
            int boxCol = -1;
            {for (int i=0; i<3; i++){
                for (int j=0; j<3; j++){
                    int secVal=secStart+i+(9*j);
                    int valPos = getPossibilityIndex(valIndex,secVal);
                    if(possibilities[valPos] == 0){
                        if (boxCol == -1 || boxCol == i){
                            boxCol = i;
                        } else {
                            inOneCol = false;
                        }
                    }
                }
            }}
            if (inOneCol && boxCol != -1){
                bool doneSomething = false;
                int col = cellToColumn(secStart) + boxCol;
                int colStart = columnToFirstCell(col);

                {for (int i=0; i<9; i++){
                    int position = colStart+(9*i);
                    int section2 = cellToSection(position);
                    int valPos = getPossibilityIndex(valIndex,position);
                    if (section != section2 && possibilities[valPos] == 0){
                        possibilities[valPos] = round;
                        doneSomething = true;
                    }
                }}
                if (doneSomething){
                    if (recordHistory) history[round].POINTING_PAIR_TRIPLE_COLUMN=1;
                    
                    return true;
                }
            }
        }
    }
    return false;
}

int SudokuBoard::countPossibilities(int position){
    int count = 0;
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        int valPos = getPossibilityIndex(valIndex,position);
        if (possibilities[valPos] == 0) count++;
    }
    return count;
}

bool SudokuBoard::arePossibilitiesSame(int position1, int position2){
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        int valPos1 = getPossibilityIndex(valIndex,position1);
        int valPos2 = getPossibilityIndex(valIndex,position2);
        if ((possibilities[valPos1] == 0 || possibilities[valPos2] == 0) && (possibilities[valPos1] != 0 || possibilities[valPos2] != 0)){
            return false;
        }
    }
    return true;
}

bool SudokuBoard::removePossibilitiesInOneFromTwo(int position1, int position2, int round){
    bool doneSomething = false;
    for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
        int valPos1 = getPossibilityIndex(valIndex,position1);
        int valPos2 = getPossibilityIndex(valIndex,position2);
        if (possibilities[valPos1] == 0 && possibilities[valPos2] == 0){
            possibilities[valPos2] = round;
            doneSomething = true;
        }
    }
    return doneSomething;
}

bool SudokuBoard::hiddenPairInColumn(int round){
    for (int column=0; column<9; column++){
        for (int valIndex=0; valIndex<9; valIndex++){
            int r1 = -1;
            int r2 = -1;
            int valCount = 0;
            for (int row=0; row<9; row++){
                int position = rowColumnToCell(row,column);
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    if (r1 == -1 || r1 == row){
                        r1 = row;
                    } else if (r2 == -1 || r2 == row){
                        r2 = row;
                    }
                    valCount++;
                }
            }
            if (valCount==2){
                for (int valIndex2=valIndex+1; valIndex2<9; valIndex2++){
                    int r3 = -1;
                    int r4 = -1;
                    int valCount2 = 0;
                    for (int row=0; row<9; row++){
                        int position = rowColumnToCell(row,column);
                        int valPos = getPossibilityIndex(valIndex2,position);
                        if (possibilities[valPos] == 0){
                            if (r3 == -1 || r3 == row){
                                r3 = row;
                            } else if (r4 == -1 || r4 == row){
                                r4 = row;
                            }
                            valCount2++;
                        }
                    }
                    if (valCount2==2 && r1==r3 && r2==r4){
                        bool doneSomething = false;
                        for (int valIndex3=0; valIndex3<9; valIndex3++){
                            if (valIndex3 != valIndex && valIndex3 != valIndex2){
                                int position1 = rowColumnToCell(r1,column);
                                int position2 = rowColumnToCell(r2,column);
                                int valPos1 = getPossibilityIndex(valIndex3,position1);
                                int valPos2 = getPossibilityIndex(valIndex3,position2);
                                if (possibilities[valPos1] == 0){
                                    possibilities[valPos1] = round;
                                    doneSomething = true;
                                }
                                if (possibilities[valPos2] == 0){
                                    possibilities[valPos2] = round;
                                    doneSomething = true;
                                }
                            }
                        }
                        if (doneSomething){
                            if (recordHistory) history[round].HIDDEN_PAIR_COLUMN=1;
                            
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::hiddenPairInSection(int round){
    for (int section=0; section<9; section++){
        for (int valIndex=0; valIndex<9; valIndex++){
            int si1 = -1;
            int si2 = -1;
            int valCount = 0;
            for (int secInd=0; secInd<9; secInd++){
                int position = sectionToCell(section,secInd);
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    if (si1 == -1 || si1 == secInd){
                        si1 = secInd;
                    } else if (si2 == -1 || si2 == secInd){
                        si2 = secInd;
                    }
                    valCount++;
                }
            }
            if (valCount==2){
                for (int valIndex2=valIndex+1; valIndex2<9; valIndex2++){
                    int si3 = -1;
                    int si4 = -1;
                    int valCount2 = 0;
                    for (int secInd=0; secInd<9; secInd++){
                        int position = sectionToCell(section,secInd);
                        int valPos = getPossibilityIndex(valIndex2,position);
                        if (possibilities[valPos] == 0){
                            if (si3 == -1 || si3 == secInd){
                                si3 = secInd;
                            } else if (si4 == -1 || si4 == secInd){
                                si4 = secInd;
                            }
                            valCount2++;
                        }
                    }
                    if (valCount2==2 && si1==si3 && si2==si4){
                        bool doneSomething = false;
                        for (int valIndex3=0; valIndex3<9; valIndex3++){
                            if (valIndex3 != valIndex && valIndex3 != valIndex2){
                                int position1 = sectionToCell(section,si1);
                                int position2 = sectionToCell(section,si2);
                                int valPos1 = getPossibilityIndex(valIndex3,position1);
                                int valPos2 = getPossibilityIndex(valIndex3,position2);
                                if (possibilities[valPos1] == 0){
                                    possibilities[valPos1] = round;
                                    doneSomething = true;
                                }
                                if (possibilities[valPos2] == 0){
                                    possibilities[valPos2] = round;
                                    doneSomething = true;
                                }
                            }
                        }
                        if (doneSomething){
                            if (recordHistory) history[round].HIDDEN_PAIR_SECTION=1;
                            
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::hiddenPairInRow(int round){
    for (int row=0; row<9; row++){
        for (int valIndex=0; valIndex<9; valIndex++){
            int c1 = -1;
            int c2 = -1;
            int valCount = 0;
            for (int column=0; column<9; column++){
                int position = rowColumnToCell(row,column);
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    if (c1 == -1 || c1 == column){
                        c1 = column;
                    } else if (c2 == -1 || c2 == column){
                        c2 = column;
                    }
                    valCount++;
                }
            }
            if (valCount==2){
                for (int valIndex2=valIndex+1; valIndex2<9; valIndex2++){
                    int c3 = -1;
                    int c4 = -1;
                    int valCount2 = 0;
                    for (int column=0; column<9; column++){
                        int position = rowColumnToCell(row,column);
                        int valPos = getPossibilityIndex(valIndex2,position);
                        if (possibilities[valPos] == 0){
                            if (c3 == -1 || c3 == column){
                                c3 = column;
                            } else if (c4 == -1 || c4 == column){
                                c4 = column;
                            }
                            valCount2++;
                        }
                    }
                    if (valCount2==2 && c1==c3 && c2==c4){
                        bool doneSomething = false;
                        for (int valIndex3=0; valIndex3<9; valIndex3++){
                            if (valIndex3 != valIndex && valIndex3 != valIndex2){
                                int position1 = rowColumnToCell(row,c1);
                                int position2 = rowColumnToCell(row,c2);
                                int valPos1 = getPossibilityIndex(valIndex3,position1);
                                int valPos2 = getPossibilityIndex(valIndex3,position2);
                                if (possibilities[valPos1] == 0){
                                    possibilities[valPos1] = round;
                                    doneSomething = true;
                                }
                                if (possibilities[valPos2] == 0){
                                    possibilities[valPos2] = round;
                                    doneSomething = true;
                                }
                            }
                        }
                        if (doneSomething){
                            if (recordHistory) history[round].HIDDEN_PAIR_ROW=1;
                            
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SudokuBoard::handleNakedPairs(int round){
    for (int position=0; position<BOARD_SIZE; position++){
        int possibilities = countPossibilities(position);
        if (possibilities == 2){
            int row = cellToRow(position);
            int column = cellToColumn(position);
            int section = cellToSectionStartCell(position);
            for (int position2=position; position2<BOARD_SIZE; position2++){
                if (position != position2){
                    int possibilities2 = countPossibilities(position2);
                    if (possibilities2 == 2 && arePossibilitiesSame(position, position2)){
                        if (row == cellToRow(position2)){
                            bool doneSomething = false;
                            for (int column2=0; column2<9; column2++){
                                int position3 = rowColumnToCell(row,column2);
                                if (position3 != position && position3 != position2 && removePossibilitiesInOneFromTwo(position, position3, round)){
                                    doneSomething = true;
                                }
                            }
                            if (doneSomething){
                                if (recordHistory) history[round].NAKED_PAIR_ROW=1;
                                
                                return true;
                            }
                        }
                        if (column == cellToColumn(position2)){
                            bool doneSomething = false;
                            for (int row2=0; row2<9; row2++){
                                int position3 = rowColumnToCell(row2,column);
                                if (position3 != position && position3 != position2 && removePossibilitiesInOneFromTwo(position, position3, round)){
                                    doneSomething = true;
                                }
                            }
                            if (doneSomething){
                                if (recordHistory) history[round].NAKED_PAIR_COLUMN=1;
                                
                                return true;
                            }
                        }
                        if (section == cellToSectionStartCell(position2)){
                            bool doneSomething = false;
                            int secStart = cellToSectionStartCell(position);
                            {for (int i=0; i<3; i++){
                                for (int j=0; j<3; j++){
                                    int position3=secStart+i+(9*j);
                                    if (position3 != position && position3 != position2 && removePossibilitiesInOneFromTwo(position, position3, round)){
                                        doneSomething = true;
                                    }
                                }
                            }}
                            if (doneSomething){
                                if (recordHistory) history[round].NAKED_PAIR_SECTION=1;
                                
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Mark exactly one cell which is the only possible value for some row, if
 * such a cell exists.
 * This method will look in a row for a possibility that is only listed
 * for one cell.  This type of cell is often called a "hidden single"
 */
bool SudokuBoard::onlyValueInRow(int round){
    for (int row=0; row<ROW_LENGTH; row++){
        for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
            int count = 0;
            int lastPosition = 0;
            for (int col=0; col<COL_HEIGHT; col++){
                int position = (row*ROW_LENGTH)+col;
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    count++;
                    lastPosition = position;
                }
            }
            if (count == 1){
                int value = valIndex+1;
                if (recordHistory) history[round].HIDDEN_SINGLE_ROW=1;
                
                mark(lastPosition, round, value);
                return true;
            }
        }
    }
    return false;
}

/**
 * Mark exactly one cell which is the only possible value for some column, if
 * such a cell exists.
 * This method will look in a column for a possibility that is only listed
 * for one cell.  This type of cell is often called a "hidden single"
 */
bool SudokuBoard::onlyValueInColumn(int round){
    for (int col=0; col<COL_HEIGHT; col++){
        for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
            int count = 0;
            int lastPosition = 0;
            for (int row=0; row<ROW_LENGTH; row++){
                int position = rowColumnToCell(row,col);
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    count++;
                    lastPosition = position;
                }
            }
            if (count == 1){
                int value = valIndex+1;
                if (recordHistory) history[round].HIDDEN_SINGLE_COLUMN=1;
                
                mark(lastPosition, round, value);
                return true;
            }
        }
    }
    return false;
}

/**
 * Mark exactly one cell which is the only possible value for some section, if
 * such a cell exists.
 * This method will look in a section for a possibility that is only listed
 * for one cell.  This type of cell is often called a "hidden single"
 */
bool SudokuBoard::onlyValueInSection(int round){
    for (int sec=0; sec<SEC_COUNT; sec++){
        int secPos = sectionToFirstCell(sec);
        for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
            int count = 0;
            int lastPosition = 0;
            {for (int i=0; i<3; i++){
                for (int j=0; j<3; j++){
                    int position = secPos + i + 9*j;
                    int valPos = getPossibilityIndex(valIndex,position);
                    if (possibilities[valPos] == 0){
                        count++;
                        lastPosition = position;
                    }
                }
            }}
            if (count == 1){
                int value = valIndex+1;
                if (recordHistory) history[round].HIDDEN_SINGLE_SECTION=1;
                
                mark(lastPosition, round, value);
                return true;
            }
        }
    }
    return false;
}

/**
 * Mark exactly one cell that has a single possibility, if such a cell exists.
 * This method will look for a cell that has only one possibility.  This type
 * of cell is often called a "single"
 */
bool SudokuBoard::onlyPossibilityForCell(int round){
    for (int position=0; position<BOARD_SIZE; position++){
        if (solution[position] == 0){
            int count = 0;
            int lastValue = 0;
            for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
                int valPos = getPossibilityIndex(valIndex,position);
                if (possibilities[valPos] == 0){
                    count++;
                    lastValue=valIndex+1;
                }
            }
            if (count == 1){
                mark(position, round, lastValue);
                if (recordHistory) history[round].SINGLE=1;
                
                return true;
            }
        }
    }
    return false;
}

/**
 * Mark the given value at the given position.  Go through
 * the row, column, and section for the position and remove
 * the value from the possibilities.
 *
 * @param position Position into the board (0-80)
 * @param round Round to mark for rollback purposes
 * @param value The value to go in the square at the given position
 */
void SudokuBoard::mark(int position, int round, int value){
    //if (solution[position] != 0) throw ("Marking position that already has been marked.");
    //if (solutionRound[position] !=0) throw ("Marking position that was marked another round.");
    int valIndex = value-1;
    solution[position] = value;

    int possInd = getPossibilityIndex(valIndex,position);
    //if (possibilities[possInd] != 0) throw ("Marking impossible position.");

    // Take this value out of the possibilities for everything in the row
    solutionRound[position] = round;
    int rowStart = cellToRow(position)*9;
    for (int col=0; col<COL_HEIGHT; col++){
        int rowVal=rowStart+col;
        int valPos = getPossibilityIndex(valIndex,rowVal);
        //cout << "Row Start: " << rowStart << " Row Value: " << rowVal << " Value Position: " << valPos << endl;
        if (possibilities[valPos] == 0){
            possibilities[valPos] = round;
        }
    }

    // Take this value out of the possibilities for everything in the column
    int colStart = cellToColumn(position);
    {for (int i=0; i<9; i++){
        int colVal=colStart+(9*i);
        int valPos = getPossibilityIndex(valIndex,colVal);
        //cout << "Col Start: " << colStart << " Col Value: " << colVal << " Value Position: " << valPos << endl;
        if (possibilities[valPos] == 0){
            possibilities[valPos] = round;
        }
    }}

    // Take this value out of the possibilities for everything in section
    int secStart = cellToSectionStartCell(position);
    {for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            int secVal=secStart+i+(9*j);
            int valPos = getPossibilityIndex(valIndex,secVal);
            //cout << "Sec Start: " << secStart << " Sec Value: " << secVal << " Value Position: " << valPos << endl;
            if (possibilities[valPos] == 0){
                possibilities[valPos] = round;
            }
        }
    }}

    //This position itself is determined, it should have possibilities.
    {for (int valIndex=0; valIndex<9; valIndex++){
        int valPos = getPossibilityIndex(valIndex,position);
        if (possibilities[valPos] == 0){
            possibilities[valPos] = round;
        }
    }}

    //cout << "Col Start: " << colStart << " Row Start: " << rowStart << " Section Start: " << secStart<< " Value: " << value << endl;
    //printPossibilities();
}

/**
 * Print a human readable list of all the possibilities for the
 * squares that have not yet been filled in.
 */
void SudokuBoard::printPossibilities(){
    for(int i=0; i<BOARD_SIZE; i++){
        //cout << " ";
        for (int valIndex=0; valIndex<NUM_POSS; valIndex++){
            int posVal = (9*i)+valIndex;
            int value = valIndex+1;
            if (possibilities[posVal]==0){
                //cout << value;
            } else {
                //cout << ".";
            }
        }
        if (i != BOARD_SIZE-1 && i%SEC_GROUP_SIZE==SEC_GROUP_SIZE-1){
            //cout << endl << "-------------------------------|-------------------------------|-------------------------------" << endl;
        } else if (i%9==8){
            //cout << endl;
        } else if (i%3==2){
            //cout << " |";
        }
    }
    //cout << endl;
}

/**
 * print the given BOARD_SIZEd array of ints
 * as a sudoku puzzle.  Use print options from
 * member variables.
 */
void SudokuBoard::print(unsigned char* sudoku){
    for(int i=0; i<BOARD_SIZE; i++){
        if (printStyle == READABLE){
            //cout << " ";
					usart_str(" ");
        }
        if (sudoku[i]==0){
            //cout << '.';
					usart_str(".");
        } else {
            //cout << sudoku[i];
					char bufor[5];
					sprintf(bufor,"%d",sudoku[i]);
					usart_str(bufor);
        }
        if (i == BOARD_SIZE-1){
            if (printStyle == CSV){
                //cout << ",";
							usart_str(",");
            } else {
                //cout << endl;
							usart_str("\r\n");
            }
            if (printStyle == READABLE || printStyle == COMPACT){
                //cout << endl;
							usart_str("\r\n");
            }
        } else if (i%9==8){
            if (printStyle == READABLE || printStyle == COMPACT){
                //cout << endl;
							usart_str("\r\n");
            }
            if (i%SEC_GROUP_SIZE==SEC_GROUP_SIZE-1){
                if (printStyle == READABLE){
                    //cout << "-------|-------|-------" << endl;
									usart_str("-------|-------|-------\r\n");
                }
            }
        } else if (i%3==2){
            if (printStyle == READABLE){
                //cout << " |";
							usart_str(" |");
            }
        }
    }
}

/**
 * Print the sudoku puzzle.
 */
void SudokuBoard::printPuzzle(){
    print(puzzle);
}

/**
 * Print the sudoku solution.
 */
void SudokuBoard::printSolution(){
    print(solution);
}

SudokuBoard::~SudokuBoard(){
    clearPuzzle();
    delete puzzle;
    delete solution;
    delete possibilities;
    delete solutionRound;
    delete randomBoardArray;
    delete randomPossibilityArray;
}

/**
 * Shuffle the values in an array of integers.
 */
void shuffleArray(unsigned char* array, int size){
    {for (int i=0; i<size; i++){
        int tailSize = size-i;
        int randTailPos = rand()%tailSize+i;
        int temp = array[i];
        array[i] = array[randTailPos];
        array[randTailPos] = temp;
    }}
}


/**
 * Given the index of a cell (0-80) calculate
 * the column (0-8) in which that cell resides.
 */
static inline int cellToColumn(int cell){
    return cell%COL_HEIGHT;
}

/**
 * Given the index of a cell (0-80) calculate
 * the row (0-8) in which it resides.
 */
static inline int cellToRow(int cell){
    return cell/ROW_LENGTH;
}

/**
 * Given the index of a cell (0-80) calculate
 * the section (0-8) in which it resides.
 */
static inline int cellToSection(int cell){
    return (cell/SEC_GROUP_SIZE*GRID_SIZE)
            + (cellToColumn(cell)/GRID_SIZE);
}

/**
 * Given the index of a cell (0-80) calculate
 * the cell (0-80) that is the upper left start
 * cell of that section.
 */
static inline int cellToSectionStartCell(int cell){
    return (cell/SEC_GROUP_SIZE*SEC_GROUP_SIZE)
            + (cellToColumn(cell)/GRID_SIZE*GRID_SIZE);
}

/**
 * Given a row (0-8) calculate the first cell (0-80)
 * of that row.
 */
static inline int rowToFirstCell(int row){
    return 9*row;
}

/**
 * Given a column (0-8) calculate the first cell (0-80)
 * of that column.
 */
static inline int columnToFirstCell(int column){
    return column;
}

/**
 * Given a section (0-8) calculate the first cell (0-80)
 * of that section.
 */
static inline int sectionToFirstCell(int section){
    return (section%GRID_SIZE*GRID_SIZE)
            + (section/GRID_SIZE*SEC_GROUP_SIZE);
}

/**
 * Given a value for a cell (0-8) and a cell (0-80)
 * calculate the offset into the possibility array (0-728).
 */
static inline int getPossibilityIndex(int valueIndex, int cell){
    return valueIndex+(NUM_POSS*cell);
}

/**
 * Given a row (0-8) and a column (0-8) calculate the
 * cell (0-80).
 */
static inline int rowColumnToCell(int row, int column){
    return (row*COL_HEIGHT)+column;
}

/**
 * Given a section (0-8) and an offset into that section (0-8)
 * calculate the cell (0-80)
 */
static inline int sectionToCell(int section, int offset){
    return sectionToFirstCell(section)
            + ((offset/GRID_SIZE)*SEC_SIZE)
            + (offset%GRID_SIZE);
}
