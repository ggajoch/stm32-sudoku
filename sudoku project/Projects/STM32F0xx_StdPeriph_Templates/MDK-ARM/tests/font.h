#include <inttypes.h>
#include <string.h>
struct FONT_CHAR_INFO
{
  int width; // Character width in bytes.
	int height; // Character height in bits
  int offset; // Offset in bytes into font bitmap.
};



struct FONT_INFO
{
	int height; //Character height
	char first; //First character in table
	char last; //last
	int space_width; //WTF
	const FONT_CHAR_INFO * font_info;
	const uint8_t * font;
	
	uint8_t get_char_piece(uint16_t character, uint16_t line, uint16_t pos)
	{
		FONT_CHAR_INFO fnt = this->font_info[character - this->first];
		
		return this->font[fnt.offset + line * fnt.width + pos];
	}
	
	void add_char_piece(uint8_t * dst, char character, uint16_t line, uint8_t position)
	{
		FONT_CHAR_INFO fnt = this->font_info[character - this->first];
		for(uint8_t i = 0; i < fnt.width; ++i)
		{
			dst[position + i] = this->font[fnt.offset + line * fnt.width + i];
		}
	}
	
	void write_line(unsigned char * dst, const char * str, uint16_t line)
	{
		uint8_t len = strlen(str), position = 0;
		for(uint8_t ch = 0; ch < len; ++ch)
		{
			if( ch == ' ' )
			{
				position += this->space_width;
			}
			else
			{
				FONT_CHAR_INFO fnt = this->font_info[str[ch] - this->first];
				
				for(uint8_t i = 0; i < fnt.width; ++i)
				{
					dst[position + i] = this->font[fnt.offset + line * fnt.width + i];
				}
				position += fnt.width;
			}
		}
	}
};


// Font data for Comic Sans MS 44pt
extern const uint8_t Font[];
extern FONT_INFO FontInfo;
extern const FONT_CHAR_INFO comicSansMS_44ptDescriptors[];

