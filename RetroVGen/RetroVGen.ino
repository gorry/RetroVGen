/*
	"RetroVGen" : Retro RGB Video Signal Generator
	Hiroaki GOTO as GORRY / http://GORRY.hauN.org/
	2023/01/05 Version 20230105a
*/

/*
	Connections:

	D1	: Pixel output (150 ohms in series to each one of R, G, B)	 --> Pins 1, 2, 3 on DB15 socket
	D3	: Vertical Sync (68 ohms in series) --> Pin 14 on DB15 socket
	D8	: LED Flash sync with VSYNC
	D9	: mode sw 4
	D10 : Horizontal Sync (68 ohms in series) --> Pin 13 on DB15 socket
	D11 : mode sw 1
	D12 : mode sw 2
	D13 : mode sw 3

	Gnd : --> Pins 5, 6, 7, 8, 10 on DB15 socket
*/

#include "TimerHelpers.h"
#include <avr/pgmspace.h>
#include "screenFont.h"
#include <avr/sleep.h>
#include <Wire.h>

#include "original.h"
#include "videoparam.h"

#define BETA_ARDUINO ARDUINO < 100

const byte hPinHSync = 10;	// <------- HSYNC
const byte hPinVSync = 3;	// <------- VSYNC
const byte hPinPixel = 1;	// <------- Pixel data
const byte hPinMSPIM_SCK = 4;	// <-- we aren't using it directly
const byte hPinSW1 = 11;
const byte hPinSW2 = 12;
const byte hPinSW3 = 13;
const byte hPinSW4 = 9;
const byte hPinLED = 8;

const byte cScreenFontWidth = 8;
const byte cScreenFontHeight = 8;

const byte cScreenWidth = 50;
const byte cScreenHeight = 32;
byte sScreenBuffer[cScreenHeight][cScreenWidth];

const ScreenParam* sScr;

int16_t sVDispLines;
int16_t sVDispBlankLines;

volatile int16_t vLine;
volatile int8_t vVsyncCount1;
volatile int8_t vVsyncCount2;
volatile byte vHsyncMode;
volatile int16_t vHsyncCount;
volatile byte *vScreenBufferPtr;
volatile const byte* vScreenFontLine;

//==============================================================
// initialize
//==============================================================

void setup()
{
	ScreenParam screenParam;

	vLine = 0;

	// initial screen
	for (byte y = 0; y < cScreenHeight; y++) {
		for (byte x = 0; x < cScreenWidth; x++) {
			byte n = x;
			if (n < y) n = y;
			n = n % 10;
			sScreenBuffer[y][x] = '0' + (char)n;
		}
	}

	// mode switch
	pinMode(hPinSW1, INPUT_PULLUP);
	pinMode(hPinSW2, INPUT_PULLUP);
	pinMode(hPinSW3, INPUT_PULLUP);
	pinMode(hPinSW4, INPUT_PULLUP);
	int modesw =
		(digitalRead(hPinSW1) ? 0 : 1) |
		(digitalRead(hPinSW2) ? 0 : 2) |
		(digitalRead(hPinSW3) ? 0 : 4) |
		(digitalRead(hPinSW4) ? 0 : 8);
	memcpy_P(&screenParam, &sScreenParam[modesw], sizeof(screenParam));
	sScr = &screenParam;

	vHsyncMode = 0;
	vVsyncCount1 = 0;
	vVsyncCount2 = 0;

	// Calc Disp Lines and Disp Blank Lines
	{
		int16_t lines = sScr->mVerticalTotalLines - sScr->mVerticalSyncLines - sScr->mVerticalFrontPorch - sScr->mVerticalBackPorch;
		int16_t displines = sScr->mVerticalChars * (8 << sScr->mLineDoubler);
		sVDispBlankLines = lines - displines;
		if (sVDispBlankLines < 0) sVDispBlankLines = 0;
		sVDispLines = lines - sVDispBlankLines;
		if (sVDispLines < 0) sVDispLines = 0;
	}

	// screen mode print
	{
		const byte windoww = 18;
		const byte windowh = 4;
		for (byte y = 0; y < windowh; y++) {
			byte* p = &sScreenBuffer[y + ((sScr->mVerticalChars - windowh) >> 1)][((sScr->mHorizontalChars - windoww) >> 1)];
			memset(p, ' ', windoww);
		}
	}
	{
		const int windoww = 16;
		const int windowh = 2;
		memcpy(&sScreenBuffer[0 + ((sScr->mVerticalChars - windowh) >> 1)][0 + ((sScr->mHorizontalChars - windoww) >> 1)], sScr->mMsg1, windoww);
		memcpy(&sScreenBuffer[1 + ((sScr->mVerticalChars - windowh) >> 1)][0 + ((sScr->mHorizontalChars - windoww) >> 1)], sScr->mMsg2, windoww);
	}

	// vsync counter print
	{
		const byte windoww = 4;
		const byte windowh = 3;
		for (byte y = 0; y < windowh; y++) {
			byte* p = &sScreenBuffer[5 + y + ((sScr->mVerticalChars - windowh) >> 1)][((sScr->mHorizontalChars - windoww) >> 1)];
			memset(p, ' ', windoww);
		}
		{
			byte* p = &sScreenBuffer[6 + ((sScr->mVerticalChars - windowh) >> 1)][0 + ((sScr->mHorizontalChars - windoww) >> 1)];
			*(p++) = 0xff;
			*(p++) = vVsyncCount2 + '0';
			*(p++) = vVsyncCount1 + '0';
			*(p++) = 0xff;
		}
	}

	pinMode(hPinLED, OUTPUT);

	// disable Timer 0
	TIMSK0 = 0;						// no interrupts on Timer 0
	OCR0A = 0;						// and turn it off
	OCR0B = 0;

	// Timer 1 - horizontal sync pulses
	// Interrupt: TIMER1_OVF_vect
	pinMode(hPinHSync, OUTPUT);
	Timer1::setMode(15, Timer1::PRESCALE_1, Timer1::CLEAR_B_ON_COMPARE);
	OCR1A = sScr->mOCR1A;
	OCR1B = sScr->mOCR1A - sScr->mOCR1B;
	TIFR1 = _BV(TOV1);				// clear overflow flag
	TIMSK1 = _BV(TOIE1);			// interrupt on overflow on timer 1

	// Timer 2 - horizontal sync pulses
	// Interrupt: TIMER2_OVF_vect
	pinMode (hPinVSync, OUTPUT);
	digitalWrite(hPinVSync, 1);
	TIMSK2 = 0;						// no interrupts on Timer 0
	OCR2A = 0;						// and turn it off
	OCR2B = 0;

	// Set up USART in SPI mode (MSPIM)
	// baud rate must be zero before enabling the transmitter
	UBRR0 = 0;						// USART Baud Rate Register
	pinMode(hPinMSPIM_SCK, OUTPUT);		// set XCK pin as output to enable master mode
	UCSR0B = 0;
	UCSR0C = _BV(UMSEL00) | _BV(UMSEL01) | _BV(UCPHA0) | _BV(UCPOL0);	// Master SPI mode

	// prepare to sleep between horizontal sync pulses
	set_sleep_mode(SLEEP_MODE_IDLE);
}

//==============================================================
// called by interrupt service routine when incoming data arrives
//==============================================================

// ISR: Hsync pulse
ISR (TIMER1_OVF_vect) {
}	// end of TIMER1_OVF_vect


//==============================================================
// main loop
//==============================================================

#define GetScreenBufferPtr() \
	line = sVDispLines - vHsyncCount;				\
	l = (line >> sScr->mLineDoubler);				\
	vScreenFontLine = &screen_font[l & 0x07][0];	\
	vScreenBufferPtr = &(sScreenBuffer[l>>3][0]);	\
	\

void loop()
{
	register uint16_t line;
	register uint16_t l;
	register byte* p;

	switch (vHsyncMode) {
	case 0: // Before Start
		// VSYNC Start
		digitalWrite(hPinVSync, 1);

		// Update Display Counter and VSYNC LED
		{
			byte c = (((vVsyncCount2 > 0) || (vVsyncCount1 >= 4)) ? ' ' : 0xff);
			{
				const byte windoww = 4;
				const byte windowh = 3;
				byte* p = &sScreenBuffer[6 + ((sScr->mVerticalChars - windowh) >> 1)][0 + ((sScr->mHorizontalChars - windoww) >> 1)];
				*(p++) = c;
				*(p++) = vVsyncCount2 + '0';
				*(p++) = vVsyncCount1 + '0';
				*(p++) = c;
			}
			digitalWrite(hPinLED, (c != ' '));
		}

		vHsyncMode = 1;
		vHsyncCount = sScr->mVerticalSyncLines;
		break;

	case 1: // Sync Line
		if (--vHsyncCount == 0) {
			// VSYNC End
			digitalWrite(hPinVSync, 0);

			vHsyncMode = 2;
			vHsyncCount = sScr->mVerticalBackPorch;
		}
		break;

	case 2: // Back Porch
		if (--vHsyncCount == 0) {
			vHsyncMode = 3;
			vHsyncCount = sVDispLines;

			GetScreenBufferPtr();
		}
		break;

	case 3: // Disp Line

		// blit pixel data to screen
		{
			register byte i = sScr->mHorizontalChars;
			register byte *p = vScreenBufferPtr;
			register byte* fontline = vScreenFontLine;

			register byte* q = fontline + (*p);
			register byte c = pgm_read_byte(q);

			// turn transmitter on
			UCSR0B = _BV(TXEN0);	// transmit enable (starts transmitting white)

			do {
				UDR0 = c;
				(void)pgm_read_byte(q);	// wait
				p++;

				q = fontline + (*p);
				c = pgm_read_byte(q);
			} while (--i);

			// wait transmitter done
			while (!(UCSR0A & _BV(TXC0))) {
				// blank
			}

			(void)pgm_read_byte(q);	// wait

			// disable transmit
			UCSR0B = 0;		// drop back to black
		}

		if (--vHsyncCount == 0) {
			if (sVDispBlankLines > 0) {
				vHsyncMode = 4;
				vHsyncCount = sVDispBlankLines;
			} else {
				vHsyncMode = 5;
				vHsyncCount = sScr->mVerticalFrontPorch;
			}
		} else {
			GetScreenBufferPtr();
		}

		break;

	case 4: // Disp Blank Line
		if (--vHsyncCount == 0) {
			vHsyncMode = 5;
			vHsyncCount = sScr->mVerticalFrontPorch;
		}
		break;

	case 5: // Front Porch
		if (--vHsyncCount == 0) {
			// Update VSYNC Counter
			vVsyncCount1++;
			if (vVsyncCount1 >= 10) {
				vVsyncCount1 = 0;
				vVsyncCount2++;
				if (vVsyncCount2 >= 6) {
					vVsyncCount2 = 0;
				}
			}

			vHsyncMode = 0;
			vHsyncCount = 1;
		}
		break;
	}

	sleep_mode();
}	// end of loop

// [EOF]
