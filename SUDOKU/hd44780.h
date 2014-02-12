#ifndef __HD44780_H
#define __HD44780_H

#define LCD_D4 GPIO_Pin_9
#define LCD_D5 GPIO_Pin_10
#define LCD_D6 GPIO_Pin_11
#define LCD_D7 GPIO_Pin_12
#define LCD_EN GPIO_Pin_8
#define LCD_RS GPIO_Pin_7

#define LCD_ALL LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7 | LCD_EN | LCD_RS

#define LCD_GPIO GPIOE

#define LCD_Y 2		// ilość wierszy wyświetlacza LCD
#define LCD_X 16	// ilość kolumn wyświetlacza LCD

// definicje adresów w DDRAM dla różnych wyświetlaczy
// inne są w wyświetlaczach 2wierszowych i w 4wierszowych
#if ( (LCD_Y == 4) && (LCD_X == 16) )
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54  	// adres 1 znaku 4 wiersza
#else
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50  	// adres 1 znaku 4 wiersza
#endif


// Komendy sterujące
#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80

#define SET_RS 	GPIO_SetBits(LCD_GPIO, LCD_RS)			// stan wysoki na linii RS
#define CLR_RS 	GPIO_ResetBits(LCD_GPIO, LCD_RS)		// stan niski na linii RS

#define SET_E 	GPIO_SetBits(LCD_GPIO, LCD_EN)			// stan wysoki na linii E
#define CLR_E 	GPIO_ResetBits(LCD_GPIO, LCD_EN)			// stan niski na linii E

extern void Delay(__IO uint32_t nTime);
//----------------------------------------------------------------------------------------
//
//		 Wysłanie połówki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
void lcd_sendHalf(uint8_t data);

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data);

//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd);

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data);

//----------------------------------------------------------------------------------------
//
//		 Wysłanie stringa do wyświetlacza LCD z pamięci RAM
//
//		 8 własnych znaków zdefiniowanych w CGRAM
//		 wysyłamy za pomocą kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str(char * str);

//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zależności od rodzaju wyświetlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_locate(uint8_t y, uint8_t x);
//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyświetlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void);

//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WYŚWIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void);

//----------------------------------------------------------------------------------------
//
//		Definicja własnego znaku na LCD z pamięci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamięci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaźnik do tablicy 7 bajtów definiujących znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak);

//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę dziesiętną na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val);

//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę szestnastkową HEX na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(int val);

#endif //__HD44780_H
