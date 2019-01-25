// "RetroVGen" : Retro RGB Video Signal Generator

typedef struct tagScreenParam
{
	uint8_t mHorizontalChars;
	uint8_t mVerticalChars;
	uint8_t mLineDoubler;
	uint8_t mDummy03;
	uint16_t mOCR1A;
	uint16_t mOCR1B;
	uint16_t mVerticalTotalLines;
	uint16_t mVerticalFrontPorch;
	uint16_t mVerticalSyncLines;
	uint16_t mVerticalBackPorch;
	char mMsg1[16];
	char mMsg2[16];
} ScreenParam;

PROGMEM static const ScreenParam sScreenParam[16] = {

	{
		 22,	// Horizontal Chars
		 30,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		507,	// H Total: 1000 / 31.50[kHz] / 0.0625[uS] = 508 (-1)
		 60,	// H Sync : 508 * (96[px]/800[px]) = 61 (-1)
		525,	// V Total
		 10,	// V Front Porch
		  2,	// V Sync Line
		 33,	// V Back Porch
		"0: VGA 31kHz",
		"31.50kHz/60.00Hz",
	},
	{
		 40,	// Horizontal Chars
		 28,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1018,	// H Total: 1000 / 15.70[kHz] / 0.0625[uS] = 1019 (-1)
		 73,	// H Sync : 4.6[uS] / 0.0625[uS] = 74 (-1)
		262,	// V Total
		 10,	// V Front Porch
		  2,	// V Sync Time
		 10,	// V Back Porch
		"1: NTSC 15kHz",
		"15.70kHz/59.94Hz",
	},
	{
		 40,	// Horizontal Chars
		 25,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 71,	// H Sync : 4.47[uS] / 0.0625[uS] = 72 (-1)
		258,	// V Total
		 24,	// V Front Porch
		  3,	// V Sync Time
		 31,	// V Back Porch
		"2: X1 15kHz",
		"15.98kHz/61.94Hz",
	},
	{
		 50,	// Horizontal Chars
		 32,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 52,	// H Sync : 3.30[uS] / 0.0625[uS] = 53 (-1)
		260,	// V Total
		  3,	// V Front Porch
		  3,	// V Sync Time
		 14,	// V Back Porch
		"3: X68000 15kHz",
		"15.98kHz/61.46Hz",
	},
	{
		 40,	// Horizontal Chars
		 25,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 71,	// H Sync : 4.47[uS] / 0.0625[uS] = 72 (-1)
		261,	// V Total
		 15,	// V Front Porch
		  8,	// V Sync Time
		 38,	// V Back Porch
		"4: PC-9801 15kHz",
		"15.98kHz/61.23Hz",
	},
	{
		 28,	// Horizontal Chars
		 25,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		643,	// H Total: 1000 / 24.86[kHz] / 0.0625[uS] = 644 (-1)
		 47,	// H Sync : 2.98[uS] / 0.0625[uS] = 48 (-1)
		448,	// V Total
		 16,	// V Front Porch
		  8,	// V Sync Time
		 24,	// V Back Porch
		"5: X1turbo 24kHz",
		"24.86kHz/55.49Hz",
	},
	{
		 28,	// Horizontal Chars
		 25,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		643,	// H Total: 1000 / 24.83[kHz] / 0.0625[uS] = 644 (-1)
		 48,	// H Sync : 3.04[uS] / 0.0625[uS] = 49 (-1)
		440,	// V Total
		  7,	// V Front Porch
		  8,	// V Sync Time
		 25,	// V Back Porch
		"6: PC-9801 24kHz",
		"24.83kHz/56.42Hz",
	},
	{
		 22,	// Horizontal Chars
		 32,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		507,	// H Total: 1000 / 31.50[kHz] / 0.0625[uS] = 508 (-1)
		 52,	// H Sync : 3.30[uS] / 0.0625[uS] = 53 (-1)
		568,	// V Total
		 15,	// V Front Porch
		  6,	// V Sync Line
		 35,	// V Back Porch
		"7: X68000 31kHz",
		"31.50kHz/55.46Hz",
	},

	{
		 22,	// Horizontal Chars
		 30,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		507,	// H Total: 1000 / 31.50[kHz] / 0.0625[uS] = 508 (-1)
		 60,	// H Sync : 508 * (96[px]/800[px]) = 61 (-1)
		525,	// V Total
		 10,	// V Front Porch
		  2,	// V Sync Line
		 33,	// V Back Porch
		"8: VGA 31kHz",
		"31.50kHz/60.00Hz",
	},
	{
		 40,	// Horizontal Chars
		 30,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1023,	// H Total: 1000 / 15.63[kHz] / 0.0625[uS] = 1024 (-1)
		 73,	// H Sync : 4.6[uS] / 0.0625[uS] = 74 (-1)
		312,	// V Total
		 10,	// V Front Porch
		  4,	// V Sync Time
		 10,	// V Back Porch
		"9: PAL 15kHz",
		"15.63kHz/50.08Hz",
	},
	{
		 40,	// Horizontal Chars
		 25,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 71,	// H Sync : 4.47[uS] / 0.0625[uS] = 72 (-1)
		258,	// V Total
		 24,	// V Front Porch
		  3,	// V Sync Time
		 31,	// V Back Porch
		"A: X1 15kHz",
		"15.98kHz/61.94Hz",
	},
	{
		 50,	// Horizontal Chars
		 32,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 52,	// H Sync : 3.30[uS] / 0.0625[uS] = 53 (-1)
		260,	// V Total
		  3,	// V Front Porch
		  3,	// V Sync Time
		 14,	// V Back Porch
		"B: X68000 15kHz",
		"15.98kHz/61.46Hz",
	},
	{
		 40,	// Horizontal Chars
		 25,	// Vertical Chars
		  0,	// Line Doubler
		  0,
	   1000,	// H Total: 1000 / 15.98[kHz] / 0.0625[uS] = 1001 (-1)
		 71,	// H Sync : 4.47[uS] / 0.0625[uS] = 72 (-1)
		261,	// V Total
		 15,	// V Front Porch
		  8,	// V Sync Time
		 38,	// V Back Porch
		"C: PC-9801 15kHz",
		"15.98kHz/61.23Hz",
	},
	{
		 28,	// Horizontal Chars
		 25,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		643,	// H Total: 1000 / 24.86[kHz] / 0.0625[uS] = 644 (-1)
		 47,	// H Sync : 2.98[uS] / 0.0625[uS] = 48 (-1)
		448,	// V Total
		 16,	// V Front Porch
		  8,	// V Sync Time
		 24,	// V Back Porch
		"D: X1turbo 24kHz",
		"24.86kHz/55.49Hz",
	},
	{
		 28,	// Horizontal Chars
		 25,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		643,	// H Total: 1000 / 24.83[kHz] / 0.0625[uS] = 644 (-1)
		 48,	// H Sync : 3.04[uS] / 0.0625[uS] = 49 (-1)
		440,	// V Total
		  7,	// V Front Porch
		  8,	// V Sync Time
		 25,	// V Back Porch
		"E: 24kHz PC-9801",
		"24.83kHz/56.42Hz",
	},
	{
		 22,	// Horizontal Chars
		 32,	// Vertical Chars
		  1,	// Line Doubler
		  0,
		507,	// H Total: 1000 / 31.50[kHz] / 0.0625[uS] = 508 (-1)
		 52,	// H Sync : 3.30[uS] / 0.0625[uS] = 53 (-1)
		568,	// V Total
		 15,	// V Front Porch
		  6,	// V Sync Line
		 35,	// V Back Porch
		"F: X68000 31kHz",
		"31.50kHz/55.46Hz",
	},


};

