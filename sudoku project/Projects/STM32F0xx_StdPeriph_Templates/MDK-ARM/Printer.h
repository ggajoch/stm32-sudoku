#include "main.h"
/***********************************

				[TODO]:
				
		ADD THERMISTOR FUNCTIONALITY
		
		
************************************/



#define LATCH_PIN GPIO_Pin_10
#define STB1_PIN GPIO_Pin_0
#define STB2_PIN GPIO_Pin_1
#define STB3_PIN GPIO_Pin_2
#define TM_PIN GPIO_Pin_3
#define HEAD_GPIO GPIOB

// SPI - GPIOA {5 - SCK, 7 - MOSI}
#define COIL_A_PIN GPIO_Pin_0
#define COIL_B_PIN GPIO_Pin_1
#define COIL_C_PIN GPIO_Pin_2
#define COIL_D_PIN GPIO_Pin_3
#define COIL_GPIO GPIOA

#define STROBE_TIME_US 5000

#define WAIT_TIME_STEPPER 5000

class Printer_
{
private:
	void latch(FunctionalState new_state);
	void send_byte(uint8_t data);
	void strobe(uint8_t nr_of_stobe);
	int8_t stepper_state;
public:
	Printer_()
	{
		this->stepper_state = 0;
	}
	void init();
	void load_line(uint8_t * data);
	void strobe_line();
	void next_line();
	void next_line(uint8_t nr);
	void prev_line();
	void prev_line(uint8_t nr);
};


extern Printer_ Printer;
