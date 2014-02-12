#include <stdio.h>
#include <inttypes.h>


#ifndef _BOARD_STRUCT_H_
#define _BOARD_STRUCT_H_

struct Field
{
	uint8_t puzzle : 4;
	uint8_t solution : 4;
	Field(uint8_t x, uint8_t y)
	{
		puzzle = x;
		solution = y;
	}
	Field(uint8_t x)
	{
		split(x);
	}
	Field()
	{
		puzzle = 0;
		solution = 0;
	}
	uint8_t merge()
	{
		uint8_t res = solution;
		res <<= 4;
		res |= puzzle;
		return res;
	}
	void split(uint8_t a)
	{
		this->puzzle = (0xF & a);
		a >>= 4;
		this->solution = a;
	}
};

struct Board
{
	#undef BOARD_SIZE
	#define BOARD_SIZE 81
	Field fields[BOARD_SIZE];
	Board()
	{
		for(uint8_t i = 0; i < BOARD_SIZE; ++i)
			fields[i] = Field(0,0);
	}
	Board(uint8_t * puzz, uint8_t * sol)
	{
		for(uint8_t i = 0; i < BOARD_SIZE; ++i)
			fields[i] = Field(puzz[i],sol[i]);
	}
	Board(uint8_t * puzz)
	{
		for(uint8_t i = 0; i < BOARD_SIZE; ++i)
			fields[i] = Field(puzz[i]);
	}
	
	void print()
	{
		printf("{");
		for(uint8_t i = 0; i < BOARD_SIZE-1; ++i)
		{
			printf("%d, ", this->fields[i].merge());
		}
		printf("%d", this->fields[BOARD_SIZE-1].merge());
		printf("}");
	}
	#undef BOARD_SIZE
};

#endif

