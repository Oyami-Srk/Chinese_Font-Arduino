#ifndef __UNIFONT_H__
#define __UNIFONT_H__

//#define _DEBUG	//For Debug... 
#define _UNIFONT_SECTION_FIXED	//Deal for magic bugs in Arduino
#define _UNIFONT_SECTION_NUM	1

typedef unsigned long       DWORD;
typedef unsigned long       BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#define PIXELS_PER_BYTE					8
#define	FONT_INDEX_TAB_SIZE				4    //CharInfo = 4Byte  (b0~b25: Start Address of CharArray, b26~b31: Width)
#define		SET_BIT(n)				(1<<n)

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tagFlSectionInfo {
	WORD  First;
	WORD  Last;
	DWORD OffAddr;
} FL_SECTION_INF, *PFL_SECTION_INF;

typedef struct tagFontLibHeader {
	BYTE    magic[4];    //'U'(or ¡¯M¡¯), 'F', 'L', X---Unicode(or MBCS) Font Library, X: Version. 4bit. 0x12=Ver 1.2
	DWORD 	Size;
	BYTE    nSection; // Number of Section of Data£¬Mainly useful for Unicode.
	BYTE    YSize;
	WORD    wCpFlag;    // codepageflag
	WORD	nTotalChars;
	BYTE    reserved[2];      // reservedByte for any platform    
#ifdef _UNIFONT_SECTION_FIXED
	FL_SECTION_INF pSection[_UNIFONT_SECTION_NUM];
#else
	PFL_SECTION_INF  pSection;
#ifdef ARDUINO
	BYTE    reserved_arduino[2];	//Just for Arduino because of sizeof pointer
#endif
#endif
} FL_HEADER, *PFL_HEADER;

/*For Debug*/
typedef unsigned char *byte_pointer;
void show_b(byte_pointer start, int len);
/*-=======-*/

class UniFont {
public:
	UniFont();
	~UniFont();

	void begin(char *FilePath);
	void end();
	int Read(WORD wCode, BYTE *pData, WORD *Count);
	BYTE getY();

private:
	FL_HEADER fl_header;
	DWORD g_dwCharInfo = 0;
#ifdef WIN32
	FILE *Font;
#endif

#ifdef ARDUINO
	SdFile Font;
#endif

private:
	enum {
		CP932 = SET_BIT(0),
		CP936 = SET_BIT(1),
		CP949 = SET_BIT(2),
		CP950 = SET_BIT(3),
		CP874 = SET_BIT(4),
		CP1250 = SET_BIT(5),
		CP1251 = SET_BIT(6),
		CP1252 = SET_BIT(7),
		CP1253 = SET_BIT(8),
		CP1254 = SET_BIT(9),
		CP1255 = SET_BIT(10),
		CP1256 = SET_BIT(11),
		CP1257 = SET_BIT(12),
		CP1258 = SET_BIT(13),
	};
};

#endif