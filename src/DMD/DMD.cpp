/*--------------------------------------------------------------------------------------

 DMD.cpp - Function and support library for the Freetronics DMD, a 512 LED matrix display
 panel arranged in a 32 x 16 layout.

 Copyright (C) 2011 Marc Alexander (info <at> freetronics <dot> com)

 Note that the DMD library uses the SPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.

 ---

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

 --------------------------------------------------------------------------------------*/
#include "DMD.h"

/*--------------------------------------------------------------------------------------
 Setup and instantiation of DMD library
 Note this currently uses the SPI port for the fastest performance to the DMD, be
 careful of possible conflicts with other SPI port devices
 --------------------------------------------------------------------------------------*/
DMD::DMD(uint8_t panelsWide, uint8_t panelsHigh)
{
	m_DisplaysWide = panelsWide;
	m_DisplaysHigh = panelsHigh;
	m_DisplaysTotal = m_DisplaysWide * m_DisplaysHigh;
	m_row1 = m_DisplaysTotal << 4;
	m_row2 = m_DisplaysTotal << 5;
	row3 = ((m_DisplaysTotal << 2) * 3) << 2;
	m_bDMDScreenRAM = (uint8_t *)malloc(m_DisplaysTotal * DMD_RAM_SIZE_BYTES);

	clearScreen(true);

	// init the scan line/ram pointer to the required start point
	m_bDMDByte = 0;
}
void DMD::Init(ISpi *pSpi,IGpio *pPinA,IGpio *pPinB,IGpio *pPinOE)
{
	m_pSpi = pSpi;
	m_PinA = pPinA;
	m_PinB = pPinB;
	m_PinOE = pPinOE;

	m_PinA->Clear();
	m_PinB->Clear();
	m_PinOE->Clear();
}

//DMD::~DMD()
//{
//   // nothing needed here
//}

/*--------------------------------------------------------------------------------------
 Set or clear a pixel at the x and y location (0,0 is the top left corner)
 --------------------------------------------------------------------------------------*/
void DMD::writePixel(uint16_t bX, uint16_t bY, uint8_t bGraphicsMode, uint8_t bPixel)
{
	unsigned int uiDMDRAMPointer;

	if (bX >= (DMD_PIXELS_ACROSS * m_DisplaysWide) || bY >= (DMD_PIXELS_DOWN * m_DisplaysHigh))
	{
		return;
	}
	uint8_t panel = (bX / DMD_PIXELS_ACROSS) + (m_DisplaysWide * (bY / DMD_PIXELS_DOWN));
	bX = (bX % DMD_PIXELS_ACROSS) + (panel << 5);
	bY = bY % DMD_PIXELS_DOWN;
	//set pointer to DMD RAM byte to be modified
	uiDMDRAMPointer = bX / 8 + bY * (m_DisplaysTotal << 2);

	uint8_t lookup = bPixelLookupTable[bX & 0x07];

	switch (bGraphicsMode)
	{
		case GRAPHICS_NORMAL:
			if (bPixel == true) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;	// zero bit is pixel on
			else m_bDMDScreenRAM[uiDMDRAMPointer] |= lookup;	// one bit is pixel off
			break;
		case GRAPHICS_INVERSE:
			if (bPixel == false) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;	// zero bit is pixel on
			else m_bDMDScreenRAM[uiDMDRAMPointer] |= lookup;	// one bit is pixel off
			break;
		case GRAPHICS_TOGGLE:
			if (bPixel == true)
			{
				if ((m_bDMDScreenRAM[uiDMDRAMPointer] & lookup) == 0) m_bDMDScreenRAM[uiDMDRAMPointer] |=
						lookup;	// one bit is pixel off
				else m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;	// one bit is pixel off
			}
			break;
		case GRAPHICS_OR:
			//only set pixels on
			if (bPixel == true) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;	// zero bit is pixel on
			break;
		case GRAPHICS_NOR:
			//only clear on pixels
			if ((bPixel == true) && ((m_bDMDScreenRAM[uiDMDRAMPointer] & lookup) == 0))
				m_bDMDScreenRAM[uiDMDRAMPointer] |= lookup;	// one bit is pixel off
			break;
	}

}

void DMD::drawString(int16_t bX, int16_t bY, const char* bChars, uint8_t length,
		uint8_t bGraphicsMode)
{
	if (bX >= (DMD_PIXELS_ACROSS * m_DisplaysWide) || bY >= DMD_PIXELS_DOWN * m_DisplaysHigh) return;
	uint8_t height = *(m_pFont + FONT_HEIGHT);
	if (bY + height < 0) return;

	int strWidth = 0;
	this->drawLine(bX - 1, bY, bX - 1, bY + height, GRAPHICS_INVERSE);

	for (int i = 0; i < length; i++)
	{
		int charWide = this->drawChar(bX + strWidth, bY, bChars[i], bGraphicsMode);
		if (charWide > 0)
		{
			strWidth += charWide;
			this->drawLine(bX + strWidth, bY, bX + strWidth, bY + height, GRAPHICS_INVERSE);
			strWidth++;
		}
		else if (charWide < 0)
		{
			return;
		}
		if ((bX + strWidth) >= DMD_PIXELS_ACROSS * m_DisplaysWide
				|| bY >= DMD_PIXELS_DOWN * m_DisplaysHigh) return;
	}
}

void DMD::drawMarquee(const char* bChars, uint8_t length, int16_t left, int16_t top)
{
	m_marqueeWidth = 0;
	for (int i = 0; i < length; i++)
	{
		m_marqueeText[i] = bChars[i];
		m_marqueeWidth += charWidth(bChars[i]) + 1;
	}
	m_marqueeHeight = *(m_pFont + FONT_HEIGHT);
	m_marqueeText[length] = '\0';
	m_marqueeOffsetY = top;
	m_marqueeOffsetX = left;
	m_marqueeLength = length;
	drawString(m_marqueeOffsetX, m_marqueeOffsetY, m_marqueeText, m_marqueeLength,
	GRAPHICS_NORMAL);
}

bool DMD::stepMarquee(int16_t amountX, int16_t amountY)
{
	bool ret = false;
	m_marqueeOffsetX += amountX;
	m_marqueeOffsetY += amountY;
	if (m_marqueeOffsetX < -m_marqueeWidth)
	{
		m_marqueeOffsetX = DMD_PIXELS_ACROSS * m_DisplaysWide;
		clearScreen(true);
		ret = true;
	}
	else if (m_marqueeOffsetX > DMD_PIXELS_ACROSS * m_DisplaysWide)
	{
		m_marqueeOffsetX = -m_marqueeWidth;
		clearScreen(true);
		ret = true;
	}

	if (m_marqueeOffsetY < -m_marqueeHeight)
	{
		m_marqueeOffsetY = DMD_PIXELS_DOWN * m_DisplaysHigh;
		clearScreen(true);
		ret = true;
	}
	else if (m_marqueeOffsetY > DMD_PIXELS_DOWN * m_DisplaysHigh)
	{
		m_marqueeOffsetY = -m_marqueeHeight;
		clearScreen(true);
		ret = true;
	}

	// Special case horizontal scrolling to improve speed
	if (amountY == 0 && amountX == -1)
	{
		// Shift entire screen one bit
		for (int i = 0; i < DMD_RAM_SIZE_BYTES * m_DisplaysTotal; i++)
		{
			if ((i % (m_DisplaysWide * 4)) == (m_DisplaysWide * 4) - 1)
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] << 1) + 1;
			}
			else
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] << 1) + ((m_bDMDScreenRAM[i + 1] & 0x80) >> 7);
			}
		}

		// Redraw last char on screen
		int strWidth = m_marqueeOffsetX;
		for (uint8_t i = 0; i < m_marqueeLength; i++)
		{
			int wide = charWidth(m_marqueeText[i]);
			if (strWidth + wide >= m_DisplaysWide * DMD_PIXELS_ACROSS)
			{
				drawChar(strWidth, m_marqueeOffsetY, m_marqueeText[i], GRAPHICS_NORMAL);
				return ret;
			}
			strWidth += wide + 1;
		}
	}
	else if (amountY == 0 && amountX == 1)
	{
		// Shift entire screen one bit
		for (int i = (DMD_RAM_SIZE_BYTES * m_DisplaysTotal) - 1; i >= 0; i--)
		{
			if ((i % (m_DisplaysWide * 4)) == 0)
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] >> 1) + 128;
			}
			else
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] >> 1) + ((m_bDMDScreenRAM[i - 1] & 1) << 7);
			}
		}

		// Redraw last char on screen
		int strWidth = m_marqueeOffsetX;
		for (uint8_t i = 0; i < m_marqueeLength; i++)
		{
			int wide = charWidth(m_marqueeText[i]);
			if (strWidth + wide >= 0)
			{
				drawChar(strWidth, m_marqueeOffsetY, m_marqueeText[i], GRAPHICS_NORMAL);
				return ret;
			}
			strWidth += wide + 1;
		}
	}
	else
	{
		drawString(m_marqueeOffsetX, m_marqueeOffsetY, m_marqueeText, m_marqueeLength,
		GRAPHICS_NORMAL);
	}

	return ret;
}

/*--------------------------------------------------------------------------------------
 Clear the screen in DMD RAM
 --------------------------------------------------------------------------------------*/
void DMD::clearScreen(uint8_t bNormal)
{
	if (bNormal) // clear all pixels
	memset(m_bDMDScreenRAM, 0xFF, DMD_RAM_SIZE_BYTES * m_DisplaysTotal);
	else // set all pixels
	memset(m_bDMDScreenRAM, 0x00, DMD_RAM_SIZE_BYTES * m_DisplaysTotal);
}

/*--------------------------------------------------------------------------------------
 Draw or clear a line from x1,y1 to x2,y2
 --------------------------------------------------------------------------------------*/
void DMD::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	int dy = y2 - y1;
	int dx = x2 - x1;
	int stepx, stepy;

	if (dy < 0)
	{
		dy = -dy;
		stepy = -1;
	}
	else
	{
		stepy = 1;
	}
	if (dx < 0)
	{
		dx = -dx;
		stepx = -1;
	}
	else
	{
		stepx = 1;
	}
	dy <<= 1;			// dy is now 2*dy
	dx <<= 1;			// dx is now 2*dx

	writePixel(x1, y1, bGraphicsMode, true);
	if (dx > dy)
	{
		int fraction = dy - (dx >> 1);	// same as 2*dy - dx
		while (x1 != x2)
		{
			if (fraction >= 0)
			{
				y1 += stepy;
				fraction -= dx;	// same as fraction -= 2*dx
			}
			x1 += stepx;
			fraction += dy;	// same as fraction -= 2*dy
			writePixel(x1, y1, bGraphicsMode, true);
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		while (y1 != y2)
		{
			if (fraction >= 0)
			{
				x1 += stepx;
				fraction -= dy;
			}
			y1 += stepy;
			fraction += dx;
			writePixel(x1, y1, bGraphicsMode, true);
		}
	}
}

/*--------------------------------------------------------------------------------------
 Draw or clear a circle of radius r at x,y centre
 --------------------------------------------------------------------------------------*/
void DMD::drawCircle(int16_t xCenter, int16_t yCenter, int16_t radius, uint8_t bGraphicsMode)
{
	int x = 0;
	int y = radius;
	int p = (5 - radius * 4) / 4;

	drawCircleSub(xCenter, yCenter, x, y, bGraphicsMode);
	while (x < y)
	{
		x++;
		if (p < 0)
		{
			p += 2 * x + 1;
		}
		else
		{
			y--;
			p += 2 * (x - y) + 1;
		}
		drawCircleSub(xCenter, yCenter, x, y, bGraphicsMode);
	}
}

void DMD::drawCircleSub(int16_t cx, int16_t cy, int16_t x, int16_t y, uint8_t bGraphicsMode)
{

	if (x == 0)
	{
		writePixel(cx, cy + y, bGraphicsMode, true);
		writePixel(cx, cy - y, bGraphicsMode, true);
		writePixel(cx + y, cy, bGraphicsMode, true);
		writePixel(cx - y, cy, bGraphicsMode, true);
	}
	else if (x == y)
	{
		writePixel(cx + x, cy + y, bGraphicsMode, true);
		writePixel(cx - x, cy + y, bGraphicsMode, true);
		writePixel(cx + x, cy - y, bGraphicsMode, true);
		writePixel(cx - x, cy - y, bGraphicsMode, true);
	}
	else if (x < y)
	{
		writePixel(cx + x, cy + y, bGraphicsMode, true);
		writePixel(cx - x, cy + y, bGraphicsMode, true);
		writePixel(cx + x, cy - y, bGraphicsMode, true);
		writePixel(cx - x, cy - y, bGraphicsMode, true);
		writePixel(cx + y, cy + x, bGraphicsMode, true);
		writePixel(cx - y, cy + x, bGraphicsMode, true);
		writePixel(cx + y, cy - x, bGraphicsMode, true);
		writePixel(cx - y, cy - x, bGraphicsMode, true);
	}
}

/*--------------------------------------------------------------------------------------
 Draw or clear a box(rectangle) with a single pixel border
 --------------------------------------------------------------------------------------*/
void DMD::drawBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	drawLine(x1, y1, x2, y1, bGraphicsMode);
	drawLine(x2, y1, x2, y2, bGraphicsMode);
	drawLine(x2, y2, x1, y2, bGraphicsMode);
	drawLine(x1, y2, x1, y1, bGraphicsMode);
}

/*--------------------------------------------------------------------------------------
 Draw or clear a filled box(rectangle) with a single pixel border
 --------------------------------------------------------------------------------------*/
void DMD::drawFilledBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	for (int b = x1; b <= x2; b++)
	{
		drawLine(b, y1, b, y2, bGraphicsMode);
	}
}

/*--------------------------------------------------------------------------------------
 Draw the selected test pattern
 --------------------------------------------------------------------------------------*/
void DMD::drawTestPattern(uint8_t bPattern)
{
	uint16_t ui;

	int numPixels = m_DisplaysTotal * DMD_PIXELS_ACROSS * DMD_PIXELS_DOWN;
	int pixelsWide = DMD_PIXELS_ACROSS * m_DisplaysWide;
	for (ui = 0; ui < numPixels; ui++)
	{
		switch (bPattern)
		{
			case PATTERN_ALT_0:	// every alternate pixel, first pixel on
				if ((ui & pixelsWide) == 0)
				//even row
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				else
				//odd row
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, !(ui & 1));
				break;
			case PATTERN_ALT_1:	// every alternate pixel, first pixel off
				if ((ui & pixelsWide) == 0)
				//even row
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, !(ui & 1));
				else
				//odd row
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				break;
			case PATTERN_STRIPE_0:	// vertical stripes, first stripe on
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				break;
			case PATTERN_STRIPE_1:	// vertical stripes, first stripe off
				writePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, !(ui & 1));
				break;
		}
	}
}

/*--------------------------------------------------------------------------------------
 Scan the dot matrix LED panel display, from the RAM mirror out to the display hardware.
 Call 4 times to scan the whole display which is made up of 4 interleaved rows within the 16 total rows.
 Insert the calls to this function into the main loop for the highest call rate, or from a timer interrupt
 --------------------------------------------------------------------------------------*/
void DMD::scanDisplayBySPI()
{
		//SPI transfer pixels to the display hardware shift registers
		int rowsize = m_DisplaysTotal << 2;
		int offset = rowsize * m_bDMDByte;
		for (int i = 0; i < rowsize; i++)
		{
//			SPI.transfer(bDMDScreenRAM[offset + i + row3]);
//			SPI.transfer(bDMDScreenRAM[offset + i + row2]);
//			SPI.transfer(bDMDScreenRAM[offset + i + row1]);
//			SPI.transfer(bDMDScreenRAM[offset + i]);
		}

		OE_DMD_ROWS_OFF();
		LATCH_DMD_SHIFT_REG_TO_OUTPUT();
		switch (m_bDMDByte)
		{
			case 0:			// row 1, 5, 9, 13 were clocked out
				LIGHT_DMD_ROW_01_05_09_13();
				m_bDMDByte = 1;
				break;
			case 1:			// row 2, 6, 10, 14 were clocked out
				LIGHT_DMD_ROW_02_06_10_14();
				m_bDMDByte = 2;
				break;
			case 2:			// row 3, 7, 11, 15 were clocked out
				LIGHT_DMD_ROW_03_07_11_15();
				m_bDMDByte = 3;
				break;
			case 3:			// row 4, 8, 12, 16 were clocked out
				LIGHT_DMD_ROW_04_08_12_16();
				m_bDMDByte = 0;
				break;
		}
		OE_DMD_ROWS_ON();

}

void DMD::selectFont(const uint8_t * font)
{
	this->m_pFont = font;
}

int DMD::drawChar(const int16_t bX, const int16_t bY, const unsigned char letter,
		uint8_t bGraphicsMode)
{
	if (bX > (DMD_PIXELS_ACROSS * m_DisplaysWide) || bY > (DMD_PIXELS_DOWN * m_DisplaysHigh)) return -1;
	unsigned char c = letter;
	uint8_t height = *(m_pFont + FONT_HEIGHT);
	if (c == ' ')
	{
		int charWide = charWidth(' ');
		this->drawFilledBox(bX, bY, bX + charWide, bY + height, GRAPHICS_INVERSE);
		return charWide;
	}
	uint8_t width = 0;
	uint8_t bytes = (height + 7) / 8;

	uint8_t firstChar = *(m_pFont + FONT_FIRST_CHAR);
	uint8_t charCount = *(m_pFont + FONT_CHAR_COUNT);

	uint16_t index = 0;

	if (c < firstChar || c >= (firstChar + charCount)) return 0;
	c -= firstChar;

	if (*(m_pFont + FONT_LENGTH) == 0 && *(m_pFont + FONT_LENGTH + 1) == 0)
	{
		// zero length is flag indicating fixed width font (array does not contain width data entries)
		width = *(m_pFont + FONT_FIXED_WIDTH);
		index = c * bytes * width + FONT_WIDTH_TABLE;
	}
	else
	{
		// variable width font, read width data, to get the index
		for (uint8_t i = 0; i < c; i++)
		{
			index += *(m_pFont + FONT_WIDTH_TABLE + i);
		}
		index = index * bytes + charCount + FONT_WIDTH_TABLE;
		width = *(m_pFont + FONT_WIDTH_TABLE + c);
	}
	if (bX < -width || bY < -height) return width;

	// last but not least, draw the character
	for (uint8_t j = 0; j < width; j++)
	{ // Width
		for (uint8_t i = bytes - 1; i < 254; i--)
		{ // Vertical Bytes
			uint8_t data = *(m_pFont + index + j + (i * width));
			int offset = (i * 8);
			if ((i == bytes - 1) && bytes > 1)
			{
				offset = height - 8;
			}
			for (uint8_t k = 0; k < 8; k++)
			{ // Vertical bits
				if ((offset + k >= i * 8) && (offset + k <= height))
				{
					if (data & (1 << k))
					{
						writePixel(bX + j, bY + offset + k, bGraphicsMode, true);
					}
					else
					{
						writePixel(bX + j, bY + offset + k, bGraphicsMode, false);
					}
				}
			}
		}
	}
	return width;
}

int DMD::charWidth(const unsigned char letter)
{
	unsigned char c = letter;
	// Space is often not included in font so use width of 'n'
	if (c == ' ') c = 'n';
	uint8_t width = 0;

	uint8_t firstChar = *(m_pFont + FONT_FIRST_CHAR);
	uint8_t charCount = *(m_pFont + FONT_CHAR_COUNT);

	if (c < firstChar || c >= (firstChar + charCount))
	{
		return 0;
	}
	c -= firstChar;

	if (*(m_pFont + FONT_LENGTH) == 0
			&& *(m_pFont + FONT_LENGTH + 1) == 0)
	{
		// zero length is flag indicating fixed width font (array does not contain width data entries)
		width = *(m_pFont + FONT_FIXED_WIDTH);
	}
	else
	{
		// variable width font, read width data
		width = *(m_pFont + FONT_WIDTH_TABLE + c);
	}
	return width;
}
