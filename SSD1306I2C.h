#ifndef __SSD1306I2C_H__
#define __SSD1306I2C_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"

class SSD1306I2C {
public:
	SSD1306I2C(int rst = 0);
	
	void Init();
	void WriteCommand(unsigned char cmd);
	void WriteData(unsigned char dat);
	void goTo(unsigned short x, unsigned short y);
	void Fill(unsigned char dat1, unsigned char dat2);

private:
	int _rst = 0;
};

#endif