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
	m_row3 = ((m_DisplaysTotal << 2) * 3) << 2;
	m_bDMDScreenRAM = (uint8_t *)malloc(m_DisplaysTotal * DMD_RAM_SIZE_BYTES);

	ClearScreen(true);

	// init the scan line/ram pointer to the required start point
	m_bDMDByte = 0;
	m_State = SBuffering;
}

void DMD::Init(SPI_TypeDef *pSpi, IDma *pDma, ITimer *pTimer, IGpio *pSS, IGpio *pPinA,
		IGpio *pPinB, IGpio *pPinOE, uint16_t refreshRate)
{
	m_pSpi = pSpi;
	m_pDma = pDma;
	m_timer.Init(pTimer);
	m_timer.SetExpiry(refreshRate);
	m_refreshRate = refreshRate;
	m_PinSS = pSS;
	m_PinA = pPinA;
	m_PinB = pPinB;
	m_PinOE = pPinOE;

	m_PinA->Clear();
	m_PinB->Clear();
	m_PinOE->Clear();

	m_pDma->SetDirection(DMA_DIR_PeripheralDST);
	// init the scan line/ram pointer to the required start point
	ClearScreen(true);
	m_bDMDByte = 0;
}

//DMD::~DMD()
//{
//   // nothing needed here
//}

/*--------------------------------------------------------------------------------------
 Set or clear a pixel at the x and y location (0,0 is the top left corner)
 --------------------------------------------------------------------------------------*/
void DMD::WritePixel(uint16_t bX, uint16_t bY, uint8_t bGraphicsMode, uint8_t bPixel)
{
	uint16_t uiDMDRAMPointer;

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
			if (bPixel == true) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;// zero bit is pixel on
			else m_bDMDScreenRAM[uiDMDRAMPointer] |= lookup;	// one bit is pixel off
			break;
		case GRAPHICS_INVERSE:
			if (bPixel == false) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;// zero bit is pixel on
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
			if (bPixel == true) m_bDMDScreenRAM[uiDMDRAMPointer] &= ~lookup;// zero bit is pixel on
			break;
		case GRAPHICS_NOR:
			//only clear on pixels
			if ((bPixel == true) && ((m_bDMDScreenRAM[uiDMDRAMPointer] & lookup) == 0))
				m_bDMDScreenRAM[uiDMDRAMPointer] |= lookup;	// one bit is pixel off
			break;
	}

}

void DMD::DrawString(int16_t bX, int16_t bY, const char* bChars, uint8_t length,
		uint8_t bGraphicsMode)
{
	if (bX >= (DMD_PIXELS_ACROSS * m_DisplaysWide) || bY >= DMD_PIXELS_DOWN * m_DisplaysHigh)
		return;
	uint8_t height = *(m_pFont + FONT_HEIGHT);
	if (bY + height < 0) return;

	int16_t strWidth = 0;
	this->DrawLine(bX - 1, bY, bX - 1, bY + height, GRAPHICS_INVERSE);

	for (int16_t i = 0; i < length; i++)
	{
		int16_t charWide = this->DrawChar(bX + strWidth, bY, bChars[i], bGraphicsMode);
		if (charWide > 0)
		{
			strWidth += charWide;
			this->DrawLine(bX + strWidth, bY, bX + strWidth, bY + height, GRAPHICS_INVERSE);
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

void DMD::DrawMarquee(const char* bChars, uint8_t length, int16_t left, int16_t top)
{
	m_marqueeWidth = 0;
	for (int16_t i = 0; i < length; i++)
	{
		m_marqueeText[i] = bChars[i];
		m_marqueeWidth += CharWidth(bChars[i]) + 1;
	}
	m_marqueeHeight = *(m_pFont + FONT_HEIGHT);
	m_marqueeText[length] = '\0';
	m_marqueeOffsetY = top;
	m_marqueeOffsetX = left;
	m_marqueeLength = length;
	DrawString(m_marqueeOffsetX, m_marqueeOffsetY, m_marqueeText, m_marqueeLength, GRAPHICS_NORMAL);
}

bool DMD::StepMarquee(int16_t amountX, int16_t amountY)
{
	bool ret = false;
	m_marqueeOffsetX += amountX;
	m_marqueeOffsetY += amountY;
	if (m_marqueeOffsetX < -m_marqueeWidth)
	{
		m_marqueeOffsetX = DMD_PIXELS_ACROSS * m_DisplaysWide;
		ClearScreen(true);
		ret = true;
	}
	else if (m_marqueeOffsetX > DMD_PIXELS_ACROSS * m_DisplaysWide)
	{
		m_marqueeOffsetX = -m_marqueeWidth;
		ClearScreen(true);
		ret = true;
	}

	if (m_marqueeOffsetY < -m_marqueeHeight)
	{
		m_marqueeOffsetY = DMD_PIXELS_DOWN * m_DisplaysHigh;
		ClearScreen(true);
		ret = true;
	}
	else if (m_marqueeOffsetY > DMD_PIXELS_DOWN * m_DisplaysHigh)
	{
		m_marqueeOffsetY = -m_marqueeHeight;
		ClearScreen(true);
		ret = true;
	}

	// Special case horizontal scrolling to improve speed
	if (amountY == 0 && amountX == -1)
	{
		// Shift entire screen one bit
		for (int16_t i = 0; i < DMD_RAM_SIZE_BYTES * m_DisplaysTotal; i++)
		{
			if ((i % (m_DisplaysWide * 4)) == (m_DisplaysWide * 4) - 1)
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] << 1) + 1;
			}
			else
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] << 1)
						+ ((m_bDMDScreenRAM[i + 1] & 0x80) >> 7);
			}
		}

		// Redraw last char on screen
		int16_t strWidth = m_marqueeOffsetX;
		for (uint8_t i = 0; i < m_marqueeLength; i++)
		{
			int16_t wide = CharWidth(m_marqueeText[i]);
			if (strWidth + wide >= m_DisplaysWide * DMD_PIXELS_ACROSS)
			{
				DrawChar(strWidth, m_marqueeOffsetY, m_marqueeText[i], GRAPHICS_NORMAL);
				return ret;
			}
			strWidth += wide + 1;
		}
	}
	else if (amountY == 0 && amountX == 1)
	{
		// Shift entire screen one bit
		for (int16_t i = (DMD_RAM_SIZE_BYTES * m_DisplaysTotal) - 1; i >= 0; i--)
		{
			if ((i % (m_DisplaysWide * 4)) == 0)
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] >> 1) + 128;
			}
			else
			{
				m_bDMDScreenRAM[i] = (m_bDMDScreenRAM[i] >> 1)
						+ ((m_bDMDScreenRAM[i - 1] & 1) << 7);
			}
		}

		// Redraw last char on screen
		int16_t strWidth = m_marqueeOffsetX;
		for (uint8_t i = 0; i < m_marqueeLength; i++)
		{
			int16_t wide = CharWidth(m_marqueeText[i]);
			if (strWidth + wide >= 0)
			{
				DrawChar(strWidth, m_marqueeOffsetY, m_marqueeText[i], GRAPHICS_NORMAL);
				return ret;
			}
			strWidth += wide + 1;
		}
	}
	else
	{
		DrawString(m_marqueeOffsetX, m_marqueeOffsetY, m_marqueeText, m_marqueeLength,
				GRAPHICS_NORMAL);
	}

	return ret;
}

/*--------------------------------------------------------------------------------------
 Clear the screen in DMD RAM
 --------------------------------------------------------------------------------------*/
void DMD::ClearScreen(uint8_t bNormal)
{
	if (bNormal) // clear all pixels
	memset(m_bDMDScreenRAM, 0xFF, DMD_RAM_SIZE_BYTES * m_DisplaysTotal);
	else // set all pixels
	memset(m_bDMDScreenRAM, 0x00, DMD_RAM_SIZE_BYTES * m_DisplaysTotal);
}

/*--------------------------------------------------------------------------------------
 Draw or clear a line from x1,y1 to x2,y2
 --------------------------------------------------------------------------------------*/
void DMD::DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	int16_t dy = y2 - y1;
	int16_t dx = x2 - x1;
	int16_t stepx, stepy;

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

	WritePixel(x1, y1, bGraphicsMode, true);
	if (dx > dy)
	{
		int16_t fraction = dy - (dx >> 1);	// same as 2*dy - dx
		while (x1 != x2)
		{
			if (fraction >= 0)
			{
				y1 += stepy;
				fraction -= dx;	// same as fraction -= 2*dx
			}
			x1 += stepx;
			fraction += dy;	// same as fraction -= 2*dy
			WritePixel(x1, y1, bGraphicsMode, true);
		}
	}
	else
	{
		int16_t fraction = dx - (dy >> 1);
		while (y1 != y2)
		{
			if (fraction >= 0)
			{
				x1 += stepx;
				fraction -= dy;
			}
			y1 += stepy;
			fraction += dx;
			WritePixel(x1, y1, bGraphicsMode, true);
		}
	}
}

/*--------------------------------------------------------------------------------------
 Draw or clear a circle of radius r at x,y centre
 --------------------------------------------------------------------------------------*/
void DMD::DrawCircle(int16_t xCenter, int16_t yCenter, int16_t radius, uint8_t bGraphicsMode)
{
	int16_t x = 0;
	int16_t y = radius;
	int16_t p = (5 - radius * 4) / 4;

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
		WritePixel(cx, cy + y, bGraphicsMode, true);
		WritePixel(cx, cy - y, bGraphicsMode, true);
		WritePixel(cx + y, cy, bGraphicsMode, true);
		WritePixel(cx - y, cy, bGraphicsMode, true);
	}
	else if (x == y)
	{
		WritePixel(cx + x, cy + y, bGraphicsMode, true);
		WritePixel(cx - x, cy + y, bGraphicsMode, true);
		WritePixel(cx + x, cy - y, bGraphicsMode, true);
		WritePixel(cx - x, cy - y, bGraphicsMode, true);
	}
	else if (x < y)
	{
		WritePixel(cx + x, cy + y, bGraphicsMode, true);
		WritePixel(cx - x, cy + y, bGraphicsMode, true);
		WritePixel(cx + x, cy - y, bGraphicsMode, true);
		WritePixel(cx - x, cy - y, bGraphicsMode, true);
		WritePixel(cx + y, cy + x, bGraphicsMode, true);
		WritePixel(cx - y, cy + x, bGraphicsMode, true);
		WritePixel(cx + y, cy - x, bGraphicsMode, true);
		WritePixel(cx - y, cy - x, bGraphicsMode, true);
	}
}

/*--------------------------------------------------------------------------------------
 Draw or clear a box(rectangle) with a single pixel border
 --------------------------------------------------------------------------------------*/
void DMD::DrawBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	DrawLine(x1, y1, x2, y1, bGraphicsMode);
	DrawLine(x2, y1, x2, y2, bGraphicsMode);
	DrawLine(x2, y2, x1, y2, bGraphicsMode);
	DrawLine(x1, y2, x1, y1, bGraphicsMode);
}

/*--------------------------------------------------------------------------------------
 Draw or clear a filled box(rectangle) with a single pixel border
 --------------------------------------------------------------------------------------*/
void DMD::DrawFilledBox(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bGraphicsMode)
{
	for (int16_t b = x1; b <= x2; b++)
	{
		DrawLine(b, y1, b, y2, bGraphicsMode);
	}
}

/*--------------------------------------------------------------------------------------
 Draw the selected test pattern
 --------------------------------------------------------------------------------------*/
void DMD::DrawTestPattern(uint8_t bPattern)
{
	uint16_t ui;

	int16_t numPixels = m_DisplaysTotal * DMD_PIXELS_ACROSS * DMD_PIXELS_DOWN;
	int16_t pixelsWide = DMD_PIXELS_ACROSS * m_DisplaysWide;
	for (ui = 0; ui < numPixels; ui++)
	{
		switch (bPattern)
		{
			case PATTERN_ALT_0:	// every alternate pixel, first pixel on
				if ((ui & pixelsWide) == 0)
				//even row
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				else
				//odd row
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, !(ui & 1));
				break;
			case PATTERN_ALT_1:	// every alternate pixel, first pixel off
				if ((ui & pixelsWide) == 0)
				//even row
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, !(ui & 1));
				else
				//odd row
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				break;
			case PATTERN_STRIPE_0:	// vertical stripes, first stripe on
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
						GRAPHICS_NORMAL, ui & 1);
				break;
			case PATTERN_STRIPE_1:	// vertical stripes, first stripe off
				WritePixel((ui & (pixelsWide - 1)), ((ui & ~(pixelsWide - 1)) / pixelsWide),
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
void DMD::Execute()
{

	switch (m_State)
	{
		case SBuffering:
		{
			if (!m_timer.HasElapsed()) return;
			m_timer.SetExpiry((uint16_t)(m_refreshRate * m_brightness));
			//SPI transfer pixels to the display hardware shift registers
			int16_t rowsize = m_DisplaysTotal << 2;
			int16_t offset = rowsize * m_bDMDByte;

			uint16_t size = 0;
			for (int16_t i = 0; i < rowsize; i++)
			{
				m_dmaCache[size++] = m_bDMDScreenRAM[offset + i + m_row3];
				m_dmaCache[size++] = m_bDMDScreenRAM[offset + i + m_row2];
				m_dmaCache[size++] = m_bDMDScreenRAM[offset + i + m_row1];
				m_dmaCache[size++] = m_bDMDScreenRAM[offset + i];
			}

			m_pDma->SetAddress((uint32_t)&m_pSpi->DR, (uint32_t)m_dmaCache);
			m_pDma->SetBufferSize(size);
			m_PinSS->Clear(); // chip select low
			m_pDma->Start();
			m_State = STransmitting;
		}
			break;

		case STransmitting:
		{
			if (!m_timer.HasElapsed()) return;
			m_timer.SetExpiry((uint16_t)(m_refreshRate * (1 - m_brightness)));
			if (!m_pDma->IsTransferFinished()) return;
			m_pDma->Stop();
			m_PinOE->Clear(); // turn off the display first
			m_PinSS->Set(); // latch the transfered data
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
			m_PinOE->Set(); // turn on the display again
			m_State = SBuffering;
		}
			break;
	}

}

void DMD::SetBrighness(float brightness)
{
	m_brightness = brightness;
}

void DMD::SetRefreshRate(uint16_t refreshRate)
{
	m_refreshRate = refreshRate;
}

void DMD::SelectFont(const uint8_t * font)
{
	this->m_pFont = font;
}

int DMD::DrawChar(const int16_t bX, const int16_t bY, const unsigned char letter,
		uint8_t bGraphicsMode)
{
	if (bX > (DMD_PIXELS_ACROSS * m_DisplaysWide) || bY > (DMD_PIXELS_DOWN * m_DisplaysHigh))
		return -1;
	unsigned char c = letter;
	uint8_t height = *(m_pFont + FONT_HEIGHT);
	if (c == ' ')
	{
		int16_t charWide = CharWidth(' ');
		this->DrawFilledBox(bX, bY, bX + charWide, bY + height, GRAPHICS_INVERSE);
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
			int16_t offset = (i * 8);
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
						WritePixel(bX + j, bY + offset + k, bGraphicsMode, true);
					}
					else
					{
						WritePixel(bX + j, bY + offset + k, bGraphicsMode, false);
					}
				}
			}
		}
	}
	return width;
}

int16_t DMD::CharWidth(const unsigned char letter)
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

	if (*(m_pFont + FONT_LENGTH) == 0 && *(m_pFont + FONT_LENGTH + 1) == 0)
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
