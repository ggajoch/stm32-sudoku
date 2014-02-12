#include "Printer.h"

Printer_ Printer;
/*

	void load_line(char * data);
	void strobe_line();
	void next_line();
	void prev_line();
*/


void Printer_::latch(FunctionalState new_state)
{
	if( new_state == ENABLE )
	{
		GPIO_ResetBits(HEAD_GPIO, LATCH_PIN);
	}
	else
	{
		GPIO_SetBits(HEAD_GPIO, LATCH_PIN);
	}
}

void Printer_::send_byte(uint8_t data)
{
	SPI_SendData8(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //transfer is fast enought 
	delay_us(100);
}

void Printer_::load_line(uint8_t * data)
{
	this->latch(DISABLE);
	// 432 bits = 54 bytes
	
	for(uint8_t i = 0; i < 54; ++i)
	{
		this->send_byte(*data);
		++data;
	}
	this->latch(ENABLE);
}


void Printer_::strobe(uint8_t nr_of_strobe)
{
	switch(nr_of_strobe)
	{
		case 1:
			GPIO_ResetBits(HEAD_GPIO, STB1_PIN);
			break;
		case 2:
			GPIO_ResetBits(HEAD_GPIO, STB2_PIN);
			break;
		case 3:
			GPIO_ResetBits(HEAD_GPIO, STB3_PIN);
			break;	
	}
	delay_us(STROBE_TIME_US);
	switch(nr_of_strobe)
	{
		case 1:
			GPIO_SetBits(HEAD_GPIO, STB1_PIN);
			break;
		case 2:
			GPIO_SetBits(HEAD_GPIO, STB2_PIN);
			break;
		case 3:
			GPIO_SetBits(HEAD_GPIO, STB3_PIN);
			break;	
	}
}


void Printer_::init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
// GPIO Pins for stepper coils
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = (COIL_A_PIN | COIL_B_PIN | COIL_C_PIN | COIL_D_PIN);
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COIL_GPIO, &GPIO_InitStruct);
		
// GPIO Pins for HeadEnd
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = (LATCH_PIN | STB1_PIN | STB2_PIN | STB3_PIN);
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HEAD_GPIO, &GPIO_InitStruct);
	GPIO_SetBits(HEAD_GPIO, STB1_PIN);
	GPIO_SetBits(HEAD_GPIO, STB2_PIN);
	GPIO_SetBits(HEAD_GPIO, STB3_PIN);
	GPIO_ResetBits(HEAD_GPIO, LATCH_PIN);	
	
// GPIO Pins for SPI
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7), GPIO_AF_0);
	
// SPI Init
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;   
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;   
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;   
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitStructure);   
		
	SPI_Cmd(SPI1, ENABLE);
	
	this->latch(ENABLE);
}

void Printer_::strobe_line()
{
	this->strobe(1);
	this->strobe(2);
	this->strobe(3);
}

void Printer_::next_line()
{
	if( stepper_state == 0 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_A_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_D_PIN);
	}
	else if( stepper_state == 1 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_B_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_A_PIN);
	}
	else if( stepper_state == 2 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_C_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_B_PIN);
	}
	else if( stepper_state == 3 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_D_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_C_PIN);
	}
	if( stepper_state == 3 )
		stepper_state = 0;
	else
		++stepper_state;
		
	delay_us(WAIT_TIME_STEPPER);
}

void Printer_::next_line(uint8_t nr)
{
	while(nr--)
	{
		this->prev_line();
	}
}

void Printer_::prev_line()
{
	if( stepper_state == 0 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_C_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_D_PIN);
	}
	else if( stepper_state == 1 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_D_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_A_PIN);
	}
	else if( stepper_state == 2 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_A_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_B_PIN);
	}
	else if( stepper_state == 3 )
	{
		GPIO_SetBits(COIL_GPIO, COIL_B_PIN);
		GPIO_ResetBits(COIL_GPIO, COIL_C_PIN);
	}
	
	if( stepper_state == 0 )
		stepper_state = 4;
	else
		--stepper_state;
		
	delay_us(WAIT_TIME_STEPPER);
}


void Printer_::prev_line(uint8_t nr)
{
	while(nr--)
	{
		this->next_line();
	}
}



