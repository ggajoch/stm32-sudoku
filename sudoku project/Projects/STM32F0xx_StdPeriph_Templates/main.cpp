/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    22-November-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "Printer.h"
#include "font.h"
#include "sudoku.h"

#include "tests/boardStruct.h"

#include "tests/T1.h"
  
void delay(uint32_t x)
{
	volatile uint32_t c = x;
	while(c > 0)
	{
			--c;
	}
}

void RCC_Conf()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}

void LED_Conf()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

void LED_On()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}

void LED_Off()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

volatile uint32_t systick_nr;
void delay_us(uint32_t x)
{
	systick_nr = (x/100);
	SysTick->VAL = 0;
	while(systick_nr > 0) {}
}

#define FOR(i,a,b) for(uint8_t i = a; i <= b; ++i)
#define REP(i,a) for(uint8_t i = 0; i < a; ++i)
#define REPL(i,a,l) for(uint8_t i = a; i < (a+l); ++i)


uint8_t line[60];

#define CLEAR_LINE for(uint8_t xxx = 0; xxx < 60; ++xxx) line[xxx] = 0x00;	
#define FOR_LINE(i) for(uint8_t i = 0; i < 60; ++i)

const uint8_t char_positions[] = {1,7,13,19,25,31,37,43,49};
const uint8_t thick_lines_nr = 2, thick_line_positions[] = {18,36};
const uint8_t thin_lines_nr = 6, thin_line_positions[] = {6,12,24,30,42,48};


void print_line(uint8_t * val, uint8_t thickness)
{
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
		Printer.load_line(line);
		Printer.strobe_line();
		Printer.next_line(1);
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
		Printer.load_line(line);
		Printer.strobe_line();
		Printer.next_line(1);
	}
}


void print_sudoku(uint8_t table[9][9])
{
	print_line(table[0],0);
	print_line(table[1],1);
	print_line(table[2],1);
	print_line(table[3],2);
	print_line(table[4],1);
	print_line(table[5],1);
	print_line(table[6],2);
	print_line(table[7],1);
	print_line(table[8],1);
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


void print_sudoku(uint8_t * puzzle)
{
	uint8_t table[9];
	
	FOR(i,0,8) table[i] = puzzle[i];	
	print_line(table,0);
	
	FOR(i,0,8) table[i] = puzzle[9+i];
	print_line(table,1);
	
	FOR(i,0,8) table[i] = puzzle[18+i];
	print_line(table,1);
	
	FOR(i,0,8) table[i] = puzzle[27+i];
	print_line(table,2);
	
	FOR(i,0,8) table[i] = puzzle[36+i];
	print_line(table,1);
	
	FOR(i,0,8) table[i] = puzzle[45+i];
	print_line(table,1);
	
	FOR(i,0,8) table[i] = puzzle[54+i];
	print_line(table,2);
	
	FOR(i,0,8) table[i] = puzzle[63+i];
	print_line(table,1);
	
	FOR(i,0,8) table[i] = puzzle[72+i];
	print_line(table,1);
}
SudokuBoard SudokuObject;
int main(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 10000); //100us resolution
	RCC_Conf();
	
	Printer.init();
	
	LED_Conf();
	
	LED_On();
	delay_us(1000000);
	LED_Off();
	

	Board tmp;

	REP(i,3)
	{
		tmp = Board(T1[i]);
		print_sudoku_puzzle(tmp);
		REP(j,5)
			Printer.next_line();
		delay_us(5000000);
	}
	
	
	
	while(1) {}
		
		
	
}
