/*
	"RetroVGen" : Retro RGB Video Signal Generator
	Hiroaki GOTO as GORRY / http://GORRY.hauN.org/
	2020/03/22 Version 20200322a
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

const byte cScreenWidth = 56;
const byte cScreenHeight = 32;
byte sScreenBuffer[cScreenHeight][cScreenWidth];

const ScreenParam* sScr;
uint16_t sVDispEndLine;
uint16_t sVSyncStartLine;
uint16_t sVSyncEndLine;

volatile int16_t vLine;
volatile int8_t vVsyncCount;

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
	sVSyncEndLine = sScr->mVerticalTotalLines - sScr->mVerticalBackPorch;
	sVSyncStartLine = sVSyncEndLine - sScr->mVerticalSyncLines;
	sVDispEndLine = sVSyncStartLine - sScr->mVerticalFrontPorch;

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
			*(p++) = (vVsyncCount / 10) + '0';
			*(p++) = (vVsyncCount % 10) + '0';
			*(p++) = 0xff;
		}
	}

	vVsyncCount = 0;
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
// draw a single scan line
//==============================================================

void doOneScanLine()
{
	register byte i;
	register uint16_t l;
	const register byte* linePtr;
	register byte* p;

	// VBack Porch
	if (vLine >= sVDispEndLine) {
		if (vLine == sVSyncStartLine) {
			digitalWrite(hPinVSync, 0);
		} else if (vLine == sVSyncEndLine) {
			digitalWrite(hPinVSync, 1);
		}
		goto next;
	}

	// pre-load pointer for speed
	l = (vLine >> sScr->mLineDoubler);
	linePtr = &screen_font[l & 0x07][0];
	l >>= 3;
	p = &(sScreenBuffer[l][0]);

	if (l >= sScr->mVerticalChars) {
		goto next;
	}

	// turn transmitter on
	UCSR0B = _BV(TXEN0);	// transmit enable (starts transmitting white)

	// how many pixels to send
	i = sScr->mHorizontalChars;

	// blit pixel data to screen
	do {
		register byte* q = linePtr + (*p);
		register byte c = pgm_read_byte(q);
		UDR0 = c;
		(void)pgm_read_byte(q);	// wait
		p++;
	} while (--i);

	// wait till done
	while (!(UCSR0A & _BV(TXC0))) {
		// blank
	}

	// disable transmit
	UCSR0B = 0;		// drop back to black

	// Next Line
next:;
	vLine++;
	if (vLine >= sScr->mVerticalTotalLines) {
		vLine = 0;
		vVsyncCount = (vVsyncCount + 1) % 60;
		byte c = ((vVsyncCount >= 4) ? ' ' : 0xff);
		{
			const byte windoww = 4;
			const byte windowh = 3;
			byte* p = &sScreenBuffer[6 + ((sScr->mVerticalChars - windowh) >> 1)][0 + ((sScr->mHorizontalChars - windoww) >> 1)];
			*(p++) = c;
			*(p++) = (vVsyncCount / 10) + '0';
			*(p++) = (vVsyncCount % 10) + '0';
			*(p++) = c;
		}
		digitalWrite(hPinLED, (c != ' '));
	}


}	// end of doOneScanLine


//==============================================================
// called by interrupt service routine when incoming data arrives
//==============================================================

// ISR: Hsync pulse
ISR (TIMER1_OVF_vect) {
}	// end of TIMER1_OVF_vect


//==============================================================
// main loop
//==============================================================

void loop()
{
	// sleep to ensure we start up in a predictable way
	sleep_mode();
	doOneScanLine();
}	// end of loop

// [EOF]
