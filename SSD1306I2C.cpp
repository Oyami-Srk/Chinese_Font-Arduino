#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"
#include "SSD1306I2C.h"



SSD1306I2C::SSD1306I2C(int rst) {
	_rst = rst;
}

void SSD1306I2C::Init() {
	Wire.begin();
	WriteCommand(0xAE);//display off

	WriteCommand(0x00);//set lower column address
	WriteCommand(0x10);//set higher column address

	WriteCommand(0x40);//set display start line

	WriteCommand(0xB0);//set page address

	WriteCommand(0x81);//对比度设置
	WriteCommand(0xCF);//0~255（对比度值……效果不是特别明显）

	WriteCommand(0xA1);//set segment remap

	WriteCommand(0xA6);//normal / reverse

	WriteCommand(0xA8);//multiplex ratio
	WriteCommand(0x3F);//duty = 1/64

	WriteCommand(0xC8);//Com scan direction

	WriteCommand(0xD3);//set display offset
	WriteCommand(0x00);

	WriteCommand(0xD5);//set osc division
	WriteCommand(0x80);

	WriteCommand(0xD9);//set pre-charge period
	WriteCommand(0xF1);

	WriteCommand(0xDA);//set COM pins
	WriteCommand(0x12);

	WriteCommand(0xDB);//set vcomh
	WriteCommand(0x40);

	WriteCommand(0x8D);//set charge pump enable
	WriteCommand(0x14);

	WriteCommand(0xAF);//display ON
}

void SSD1306I2C::WriteCommand(unsigned char cmd) {
	Wire.beginTransmission(0x78 >> 1);//0x78 >> 1
	Wire.write(0x00);//0x00
	Wire.write(cmd);
	Wire.endTransmission();
}

void SSD1306I2C::WriteData(unsigned char dat) {
	Wire.beginTransmission(0x78 >> 1);//0x78 >> 1
	Wire.write(0x40);//0x40
	Wire.write(dat);
	Wire.endTransmission();
}

void SSD1306I2C::goTo(unsigned short x, unsigned short y) {
	Wire.beginTransmission(0x78 >> 1);//0x78 >> 1
	Wire.write(0x00);//0x00
	Wire.write(0xb0 + y);
	Wire.write(0x00);//0x00
	Wire.write(((x & 0xf0) >> 4) | 0x10);
	Wire.write(0x00);//0x00
	Wire.write((x & 0x0f) | 0x01);
	Wire.endTransmission();
}

void SSD1306I2C::Fill(unsigned char dat1, unsigned char dat2) {

}
