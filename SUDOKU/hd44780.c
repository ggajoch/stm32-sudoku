#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "hd44780.h"

//----------------------------------------------------------------------------------------
//
//		 Wysłanie połówki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
void lcd_sendHalf(uint8_t data){
	/*if (data&(1<<0)) GPIO_SetBits(); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);*/
	GPIO_WriteBit(LCD_GPIO, LCD_D4, (BitAction)(data&(1<<0)));
	GPIO_WriteBit(LCD_GPIO, LCD_D5, (BitAction)(data&(1<<1)));
	GPIO_WriteBit(LCD_GPIO, LCD_D6, (BitAction)(data&(1<<2)));
	GPIO_WriteBit(LCD_GPIO, LCD_D7, (BitAction)(data&(1<<3)));
}

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	// Ustawienie pinów portu LCD D4..D7 jako wyjścia
	//data_dir_out();

	SET_E;
	lcd_sendHalf(_data >> 4);			// wysłanie starszej części bajtu danych D7..D4
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);				// wysłanie młodszej części bajtu danych D3..D0
	CLR_E;

	Delay(120);

}

//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	_lcd_write_byte(cmd);
}

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS;
	_lcd_write_byte(data);
}

//----------------------------------------------------------------------------------------
//
//		 Wysłanie stringa do wyświetlacza LCD z pamięci RAM
//
//		 8 własnych znaków zdefiniowanych w CGRAM
//		 wysyłamy za pomocą kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str(char * str)
{
	register char znak;
	while ( (znak=*(str++)) )
		lcd_write_data( ( znak>=0x80 && znak<=0x87 ) ? (znak & 0x07) : znak);
}

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
void lcd_locate(uint8_t y, uint8_t x)
{
	switch(y)
	{
		case 0: y = LCD_LINE1; break;

#if (LCD_Y>1)
	    case 1: y = LCD_LINE2; break; // adres 1 znaku 2 wiersza
#endif
#if (LCD_Y>2)
    	case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
#endif
#if (LCD_Y>3)
    	case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
#endif
	}

	lcd_write_cmd( (0x80 + y + x) );
}

//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyświetlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void)
{
	lcd_write_cmd( LCDC_CLS );

	Delay(4900);
}

//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WYŚWIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	// inicjowanie pinów portów ustalonych do podłączenia z wyświetlaczem LCD
	// ustawienie wszystkich jako wyjścia
	/* GPIOB dla LCD - zegar */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//port B dla LCD

  /* GPIOB dla LCD - konfiguracja */
  GPIO_InitStructure.GPIO_Pin = LCD_ALL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_GPIO, &GPIO_InitStructure);
	
	GPIO_ResetBits(LCD_GPIO, LCD_ALL);
	
	Delay(15000);

	// jeszcze nie można używać Busy Flag
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	Delay(4100);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	Delay(100);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC4B);
	Delay(100);

	// już można używać Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	// wyłączenie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	// włączenie wyświetlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// przesuwanie kursora w prawo bez przesuwania zawartości ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );

	// kasowanie ekranu
	lcd_cls();
}

//----------------------------------------------------------------------------------------
//
//		Definicja własnego znaku na LCD z pamięci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamięci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaźnik do tablicy 7 bajtów definiujących znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = *(def_znak++);
		lcd_write_data(c);
	}
}

//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę dziesiętną na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val)
{
	char bufor[17];
	sprintf(bufor, "%i", val);
	lcd_str( (char*)bufor );
}

//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę szestnastkową HEX na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(int val)
{
	char bufor[17];
	sprintf(bufor, "%X", val);
	lcd_str( (char*)bufor );
}
