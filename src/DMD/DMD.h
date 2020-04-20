/*--------------------------------------------------------------------------------------

 DMD.h   - Function and support library for the Freetronics DMD, a 512 LED matrix display
 panel arranged in a 32 x 16 layout.

 Copyright (C) 2011 Marc Alexander (info <at> freetronics <dot> com)

 Note that the DMD library uses the SPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.


 LED Panel Layout in RAM
 32 pixels (4 bytes)
 top left  ----------------------------------------
 |                                      |
 Screen 1 |        512 pixels (64 bytes)         | 16 pixels
 |                                      |
 ---------------------------------------- bottom right

 ---
 
 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

 --------------------------------------------------------------------------------------*/
#ifndef DMD_H_
#define DMD_H_

#include <Hal.h>

typedef uint8_t (*FontCallback)(const uint8_t*);

//The main class of DMD library functions
class DMD
{
public:
	//Instantiate the DMD
	DMD(uint8_t panelsWide, uint8_t panelsHigh);
	//virtual ~DMD();

	void Init(ISpi *pSpi,IGpio *pPinA,IGpio *pPinB,IGpio *pPinOE);

	//Set or clear a pixel at the x and y location (0,0 is the top left corner)
	void writePixel(uint16_t bX, uint16_t bY, uint8_t bGraphicsMode, uint8_t bPixel);

	//Draw a string
	void drawString(int16_t bX, int16_t bY, const char* bChars, uint8_t length,
			uint8_t bGraphicsMode);

	//Select a text font
	void selectFont(const uint8_t* font);

	//Draw a single character
	int drawChar(const int16_t bX, const int16_t bY, const unsigned char letter,
			uint8_t bGraphicsMode);

	//Find the width of a character
	int charWidth(const unsigned char letter);

	//Draw a scrolling string
	void drawMarquee(const char* bChars, uint8_t length, int16_t left, int16_t top);

	//Move the maquee accross by amount
	bool stepMarquee(int16_t amountX, int16_t amountY);

	//Clear the screen in DMD RAM
	void clearScreen(uint8_t bNormal);

	//Draw or clear a line from x1,y1 to x2,y2
	void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode);

	//Draw or clear a circle of radius r at x,y centre
	void drawCircle(int16_t xCenter, int16_t yCenter, int16_t radius, uint8_t bGraphicsMode);

	//Draw or clear a box(rectangle) with a single pixel border
	void drawBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode);

	//Draw or clear a filled box(rectangle) with a single pixel border
	void drawFilledBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode);

	//Draw the selected test pattern
	void drawTestPattern(uint8_t bPattern);

	//Scan the dot matrix LED panel display, from the RAM mirror out to the display hardware.
	//Call 4 times to scan the whole display which is made up of 4 interleaved rows within the 16 total rows.
	//Insert the calls to this function into the main loop for the highest call rate, or from a timer interrupt
	void scanDisplayBySPI();

private:
	void drawCircleSub(int16_t cx, int16_t cy, int16_t x, int16_t y, uint8_t bGraphicsMode);

	//Mirror of DMD pixels in RAM, ready to be clocked out by the main loop or high speed timer calls
	uint8_t *m_bDMDScreenRAM;

	//Marquee values
	char m_marqueeText[256];
	uint8_t m_marqueeLength;
	int16_t m_marqueeWidth;
	int16_t m_marqueeHeight;
	int16_t m_marqueeOffsetX;
	int16_t m_marqueeOffsetY;

	//Pointer to current font
	const uint8_t* m_pFont;

	//Display information
	uint8_t m_DisplaysWide;
	uint8_t m_DisplaysHigh;
	uint8_t m_DisplaysTotal;
	int16_t m_row1, m_row2, row3;

	//scanning pointer into bDMDScreenRAM, setup init @ 48 for the first valid scan
	volatile uint8_t m_bDMDByte;

	//Pixel/graphics writing modes (bGraphicsMode)
	static const uint8_t GRAPHICS_NORMAL = 0;
	static const uint8_t GRAPHICS_INVERSE = 1;
	static const uint8_t GRAPHICS_TOGGLE = 2;
	static const uint8_t GRAPHICS_OR = 3;
	static const uint8_t GRAPHICS_NOR = 4;

	//drawTestPattern Patterns
	static const uint8_t PATTERN_ALT_0 = 0;
	static const uint8_t PATTERN_ALT_1 = 1;
	static const uint8_t PATTERN_STRIPE_0 = 2;
	static const uint8_t PATTERN_STRIPE_1 = 3;

	//display screen (and subscreen) sizing
	static const uint8_t DMD_PIXELS_ACROSS = 32;      //pixels across x axis (base 2 size expected)
	static const uint8_t DMD_PIXELS_DOWN = 16;      //pixels down y axis
	static const uint8_t DMD_BITSPERPIXEL = 1;//1 bit per pixel, use more bits to allow for pwm screen brightness control
	static const uint8_t DMD_RAM_SIZE_BYTES = ((DMD_PIXELS_ACROSS * DMD_BITSPERPIXEL / 8)
			* DMD_PIXELS_DOWN);

	// (32x * 1 / 8) = 4 bytes, * 16y = 64 bytes per screen here.
	//lookup table for DMD::writePixel to make the pixel indexing routine faster
	static constexpr uint8_t bPixelLookupTable[8] = { 0x80,   //0, bit 7
			0x40,   //1, bit 6
			0x20,   //2. bit 5
			0x10,   //3, bit 4
			0x08,   //4, bit 3
			0x04,   //5, bit 2
			0x02,   //6, bit 1
			0x01    //7, bit 0
			};

	// Font Indices
#define FONT_LENGTH             0
#define FONT_FIXED_WIDTH        2
#define FONT_HEIGHT             3
#define FONT_FIRST_CHAR         4
#define FONT_CHAR_COUNT         5
#define FONT_WIDTH_TABLE        6

	// my code porting
private:
	IGpio *m_PinA;
	IGpio *m_PinB;
	IGpio *m_PinOE;
	IGpio *m_PinSS;
	ISpi *m_pSpi;

	//DMD I/O pin macros
	void LIGHT_DMD_ROW_01_05_09_13()
	{
//		digitalWrite( PIN_DMD_B, LOW);
//		digitalWrite( PIN_DMD_A, LOW);
		m_PinB->Clear();
		m_PinA->Clear();
	}
	void LIGHT_DMD_ROW_02_06_10_14()
	{
//		digitalWrite( PIN_DMD_B, LOW);
//		digitalWrite( PIN_DMD_A, HIGH);
		m_PinB->Clear();
		m_PinA->Set();
	}
	void LIGHT_DMD_ROW_03_07_11_15()
	{
//		digitalWrite( PIN_DMD_B, HIGH);
//		digitalWrite( PIN_DMD_A, LOW);
		m_PinB->Set();
		m_PinA->Clear();
	}
	void LIGHT_DMD_ROW_04_08_12_16()
	{
//		digitalWrite( PIN_DMD_B, HIGH);
//		digitalWrite( PIN_DMD_A, HIGH);
		m_PinB->Set();
		m_PinA->Set();
	}
	void LATCH_DMD_SHIFT_REG_TO_OUTPUT()
	{
//		digitalWrite( PIN_DMD_SCLK, HIGH);
//		digitalWrite( PIN_DMD_SCLK, LOW);
		m_PinSS->Set();
		m_PinSS->Clear();
	}
	void OE_DMD_ROWS_OFF()
	{
//		digitalWrite( PIN_DMD_nOE, LOW);
		m_PinOE->Clear();
	}
	void OE_DMD_ROWS_ON()
	{
//		digitalWrite( PIN_DMD_nOE, HIGH);
		m_PinOE->Set();
	}

};

#endif /* DMD_H_ */
