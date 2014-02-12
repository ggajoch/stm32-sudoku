/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "sudoku.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStruct;
USART_ClockInitTypeDef USART_ClockInitStruct; 
RCC_ClocksTypeDef RCC_Clocks;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
static __IO uint32_t TimingDelay2;
uint16_t dane;
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
void Delay_ms(__IO uint32_t nTime);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	SysTick_Config(SystemCoreClock / 1000000);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//port A dla RS232

  /* GPIO - konfiguracja */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//PA2 - TxD, PA3 - RxD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* funkcje alternatywne - usart2 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/* zegarek dla usart2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/*USART_ClockInitStruct.USART_Clock=USART_Clock_Enable;
	USART_ClockInitStruct.USART_CPOL=USART_CPOL_High;
	USART_ClockInitStruct.USART_CPHA=USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit=USART_LastBit_Disable;
	USART_ClockInit(USART2, &USART_ClockInitStruct);*/
	
	/*konfiguracja*/
	USART_InitStruct.USART_BaudRate=9600;//prędkość
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//8 bit danych
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//1 stop bit
	USART_InitStruct.USART_Parity=USART_Parity_No;//parity = none
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//włącz Rx i Tx
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//brak kontroli przepływu
	USART_Init(USART2, &USART_InitStruct);
	//włączamy usart
	USART_Cmd(USART2, ENABLE);
	
	//USART2->BRR=4375;
	RCC_GetClocksFreq(&RCC_Clocks);
	usart_str("Witaj\r\n");
	/*while(1){
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)==RESET);
			USART_ClearFlag(USART2, USART_FLAG_RXNE);
			dane=USART_ReceiveData(USART2);
			USART_SendData(USART2, dane);
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET);
	}*/
	
	SudokuBoard::Difficulty difficulty = SudokuBoard::EASY;

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
										char bufor[5];
										sprintf(bufor,"%d",1000-number_to_benchmark);
										usart_str(bufor);
									  usart_str("\r\n");
                }
            }

            if (havePuzzle){

                // Print the puzzle itself.
                ss->printPuzzle();

                ss->printSolution();
            }
        }

        //delete ss;
				
				while(1);
	
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 us.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void Delay_ms(__IO uint32_t nTime)
{
  TimingDelay2 = nTime;

  while(TimingDelay2 != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
	static uint16_t num=0;
	num++;
	if(num==999){
		if (TimingDelay2 != 0x00)
		{ 
			TimingDelay2--;
		}
		num=0;
	}
	
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

void usart_str(char * str)
{
	register char znak;
	while ( (znak=*(str++)) ){
		USART_SendData(USART2, znak);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET);
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
