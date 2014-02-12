#include <cstdio>

#include <iostream>
#include <bitset>

#include "font.h"
#include "sudoku.h"
#define FOR(i,a,b) for(int i = a; i <= b; ++i)
#define REP(i,a) for(int i = 0; i < a; ++i)
#define byte unsigned char


#include "T1.h"

const uint8_t char_positions[] = {1,7,13,19,25,31,37,43,49};
const uint8_t thick_lines_nr = 2, thick_line_positions[] = {18,36};
const uint8_t thin_lines_nr = 6, thin_line_positions[] = {6,12,24,30,42,48};
uint8_t lines_nr = 8, line_positions[] = {6,12,18,24,30,36,42,48};

uint8_t line[60];

void print_line(uint8_t * val, uint8_t thickness)
{

	#define WIDTH 25
	REP(i,54) line[i] = 0xFF;
	uint8_t end;
	if( thickness == 0 )
		end = 0;
	else if( thickness == 1 )
		end = 3;	
	else 
		end = 5;	
	REP(i,end)
	{
		REP(j, WIDTH)
		{
			//printf("%d ", line[j]);
			int c = line[j];
			std::bitset<sizeof(char) * 8> binary(c);
			std::cout << binary;
		}
		printf("\n");
	}
	REP(i,54) line[i] = 0;
	REP(i,48)
	{
		REP(xx,9)
		{
			if( val[xx] != 0 )
			{
				FontInfo.add_char_piece(line, val[xx] + '0', i, char_positions[xx]);
			}
		}
		REP(xx,thick_lines_nr)
		{
			line[thick_line_positions[xx]] = 0x7E; //5 bits width
		}		
		REP(xx,thin_lines_nr)
		{
			line[thin_line_positions[xx]] = 0x38; //3 bits width
		}
		REP(j, WIDTH)
		{
			//printf("%d ", line[j]);
			int c = line[j];
			std::bitset<sizeof(char) * 8> binary(c);
			std::cout << binary;
		}
		printf("\n");
	}
}
void print_sudoku_puzzle(Board x)
{
	uint8_t table[9], iter = 0;

#define CPY_TO_TABLE REP(i,9) { table[i] = x.fields[iter + i].puzzle; } iter += 9;

	CPY_TO_TABLE
	print_line(table,0);
	CPY_TO_TABLE
	print_line(table,1);
	CPY_TO_TABLE
	print_line(table,1);
	CPY_TO_TABLE
	print_line(table,2);
	CPY_TO_TABLE
	print_line(table,1);
	CPY_TO_TABLE
	print_line(table,1);
	CPY_TO_TABLE
	print_line(table,2);
	CPY_TO_TABLE
	print_line(table,1);
	CPY_TO_TABLE
	print_line(table,1);

#undef CPY_TO_TABLE
}

int main()
{
	Board tmp(T1[0]);
	print_sudoku_puzzle(tmp);
}
