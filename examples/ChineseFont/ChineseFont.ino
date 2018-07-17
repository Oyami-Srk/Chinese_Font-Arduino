#include <Wire.h>
#include <CN_SSD1306_Wire.h>
#include "UniFont.h"

#define DISPLAY_WIDTH	128
#define DISPLAY_OBJECT	disp

CN_SSD1306_Wire disp(8);//HardWare I2C
UniFont font;

void uni_print(UniFont font, WORD wCode, int x, int y) {
	int FontSize = font.getY();
	WORD bytesPerLine = 0;
	BYTE *buf = new BYTE[FontSize * (FontSize / PIXELS_PER_BYTE)];
	memset(buf, 0, FontSize*(FontSize / PIXELS_PER_BYTE));

	font.Read(wCode, buf, &bytesPerLine);
	DISPLAY_OBJECT.IIC_SetPos(x, y);
	int i, j, c;
	c = 0;
	for (i = 0; i < FontSize; i++) {
		for (j = 0; j < bytesPerLine; j++) {
			c++;
			DISPLAY_OBJECT.WriteData(buf[i * bytesPerLine + j]);
			if (c == FontSize)
				DISPLAY_OBJECT.IIC_SetPos(x, y + 1);
		}
	}
	delete buf;
	buf = NULL;
}

void uni_print(UniFont font, WORD wCodes[], int x, int y, unsigned int len = 0) {
	int FontSize = font.getY();
	unsigned int i;
	unsigned int c = 0;
	if (0 == len)
		len = 65535;
	//int col = ()
	for (i = 0; i < len; i++) {
		if (wCodes[i] == '\0')
			break;
		if (c >= (int)(DISPLAY_WIDTH / FontSize) || wCodes[i] == '\n') {
			y += 2;
			c = 0;
			if (wCodes[i] == '\n')
				continue;
		}
		uni_print(font, wCodes[i], x + c * FontSize, y);
		c++;
	}
}

void setup() {
	Serial.begin(115200);
	disp.Initial();
	disp.Fill_Screen(0x00, 0x00);
	font.begin("Font.bin");

	WORD c[] = {
		0x4e2d,0x6587,0x5b57,0x5e93,0x6d4b,0x8bd5,0xA,	//0xA = \n
		0x8fde,0x7eed,0x8f93,0x51fa,0x7684,0x81ea,0x52a8,0x6362,0x884c,0x6d4b,0x8bd5,
		0x0	//END OF LINE
	};

	uni_print(font, c, 0, 0);

	delay(7000);
	disp.Fill_Screen(0x00, 0x00);
}

void loop() {}
