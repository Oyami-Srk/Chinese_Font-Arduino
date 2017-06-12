#include "UniFont.h"

#define	GET_FONT_WIDTH(charinfo)		(charinfo >> 26)
#define	GET_FONT_OFFADDR(charinfo)		(charinfo & 0x3ffffff)
#define	DB2UC(c1,c2)					(c1 | (((U16)c2)<<8))
#define	IS_CJK(codepage)				(codepage & 0xf)	//mono or not

#ifdef WIN32
#define PRINT_STR(x) printf("%s",x)
#define PRINT_INT(x) printf("%d",x)
#define PRINT_HEX(x) printf("%x",x)

#define FONTREAD(FONT, pDATA, SIZE) \
	if(NULL != FONT)\
		fread(pDATA, SIZE, 1, FONT)

#define FONTSEEK(FONT, OFFSET)\
	if(NULL != FONT)\
		fseek(FONT, OFFSET, SEEK_SET)
#endif
#ifdef ARDUINO
#define PRINT_STR(x) Serial.print(x)
#define PRINT_INT(x) Serial.print(x)
#define PRINT_HEX(x) Serial.print(x)

#define FONTREAD(FONT, pDATA, SIZE)\
	FONT.read(pDATA, SIZE)

#define FONTSEEK(FONT, OFFSET)\
	FONT.seekSet(OFFSET)
#endif

#ifdef ARDUINO
SdFat SD;
#endif

/*For Debug*/
typedef unsigned char *byte_pointer;

void show_b(byte_pointer start, int len) {
	char buf[4] = { 0 };
	for (int i = 0; i < len; i++) {
		sprintf(buf, " %.2x", start[i]);
		PRINT_STR(buf);
	}
	PRINT_STR("\n");
}
/*Bugs never die!*/

UniFont::UniFont() {}

void UniFont::begin(char *FilePath) {
	memset(&fl_header, 0, sizeof(FL_HEADER));
	int result = 0;
#ifdef WIN32
	if ((Font = fopen(FilePath, "rb")) != NULL)
		result = 1;
#endif

#ifdef ARDUINO
	//Memo: SDmodule needs 5V power
	if (!SD.begin(10)) {	//10 - CS pin
		PRINT_STR("SD initialization failed!\n");
		result = 0;
	} else {
		PRINT_STR("SD initialization done.\n");
		//Font = SD.open(FilePath);		//For offical SD method
		Font.open(FilePath, O_RDONLY);	//For SdFat better than before

#ifdef _DEBUG
		PRINT_STR("Is Open: ");
		PRINT_INT(Font.isOpen());
		PRINT_STR("\n");
#endif // _DEBUG

		if (Font.available() != 0)	//Both offical SD method and SdFat can use
			result = 1;
	}
#endif
	//OpenCode Here
	if (!result) {
		PRINT_STR("Cannot Open File: ");
		PRINT_STR(FilePath);
		PRINT_STR("\n");
		return;
	}
	result = 0;

#ifndef _UNIFONT_SECTION_FIXED
	FONTREAD(Font, &fl_header, sizeof(FL_HEADER) - 4);	//Always align with 4bytes long
#else
	FONTREAD(Font, &fl_header, sizeof(FL_HEADER) - sizeof(PFL_SECTION_INF) + sizeof(FL_SECTION_INF) * _UNIFONT_SECTION_NUM);
#endif

	if ((fl_header.magic[0] != 'U' && fl_header.magic[0] != 'M')
		|| fl_header.magic[1] != 'F' || fl_header.magic[2] != 'L') {
		PRINT_STR("Cann't support file format!\n");
		return;
	}
	if ('U' == fl_header.magic[0]) {
#ifndef _UNIFONT_SECTION_FIXED
		if (fl_header.pSection != NULL) {
			free(fl_header.pSection);
			fl_header.pSection = NULL;
		}	
		//In Arduino, it's better not to use malloc or now beacuse of lack memory...but you know......
		fl_header.pSection = (FL_SECTION_INF *)malloc(fl_header.nSection * sizeof(FL_SECTION_INF));
		//fl_header.pSection = new FL_SECTION_INF[fl_header.nSection];

		if (fl_header.pSection == NULL) {
			PRINT_STR("Malloc fail!\n");
			return;
		}

		FONTREAD(Font, fl_header.pSection, fl_header.nSection * sizeof(FL_SECTION_INF));
#endif

#ifdef _DEBUG
		PRINT_STR("First Section: \n");
		PRINT_STR("First: ");
		PRINT_INT(fl_header.pSection[0].First);
		PRINT_STR("\nLast: ");
		PRINT_INT(fl_header.pSection[_UNIFONT_SECTION_NUM].Last);
		PRINT_STR("\nOffAddr: ");
		PRINT_INT(fl_header.pSection[0].OffAddr);
		PRINT_STR("\n");
#endif // _DEBUG
	}
	PRINT_STR("UniFont initialization done.\n");
}

void UniFont::end() {
#ifndef _UNIFONT_SECTION_FIXED
	if ('U' == fl_header.magic[0]) {
		if (fl_header.pSection != NULL) {
			free(fl_header.pSection);
			fl_header.pSection = NULL;
		}
	}
#endif

#ifdef WIN32
	if (Font != NULL)
		fclose(Font);
#endif
#ifdef ARDUINO
	if (Font.available())
		Font.close();
#endif
	delete this;
}

UniFont::~UniFont() {
#ifndef _UNIFONT_SECTION_FIXED
	if ('U' == fl_header.magic[0]) {
		if (fl_header.pSection != NULL) {
			free(fl_header.pSection);
			fl_header.pSection = NULL;
		}
	}
#endif

#ifdef WIN32
	if (Font != NULL)
		fclose(Font);
#endif
#ifdef ARDUINO
	if (Font.available())
		Font.close();
#endif
}

int UniFont::Read(WORD wCode, BYTE * pData, WORD * Count) {
#ifdef _DEBUG
	PRINT_STR("Code=");
	show_b((byte_pointer)&wCode, sizeof(WORD));
	PRINT_STR("First Section: \n");
	PRINT_STR("First: ");
	PRINT_INT(fl_header.pSection[0].First);
	PRINT_STR("\nLast: ");
	PRINT_INT(fl_header.pSection[0].Last);
	PRINT_STR("\nOffAddr: ");
	PRINT_INT(fl_header.pSection[0].OffAddr);
	PRINT_STR("\n");
#endif // _DEBUG

	if ('U' == fl_header.magic[0]) {
		//g_dwCharInfo = read_char_info_unicode(wCode);
		DWORD offset;
		int i = 0;
		DWORD dwCharInfo = 0;

#ifndef _UNIFONT_SECTION_FIXED
		for (i = 0; i < fl_header.nSection; i++) {
#else
		for (i = 0; i < _UNIFONT_SECTION_NUM; i++) {
#endif
			if (wCode >= fl_header.pSection[i].First && wCode <= fl_header.pSection[i].Last)
				break;
		}
		if (i >= fl_header.nSection) {
			PRINT_STR("Cannot Find out Code\n");
			return 0;
		}
// #else
// 		if (!(wCode >= fl_header.pSection[i].First && wCode <= fl_header.pSection[i].Last)) {
// 			PRINT_STR("Cannot Find out Code\n");
// 			return 0;
// 		}
// #endif
		
#ifdef WIN32
		offset = fl_header.pSection[i].OffAddr + ((wCode - fl_header.pSection[i].First) * FONT_INDEX_TAB_SIZE);
#endif

#ifdef ARDUINO	//Arduino cannot comput in once
		offset = ((wCode - fl_header.pSection[i].First));
		offset *= FONT_INDEX_TAB_SIZE;
		offset += fl_header.pSection[i].OffAddr;
#endif

		FONTSEEK(Font, offset);
		FONTREAD(Font, &dwCharInfo, sizeof(DWORD));
		g_dwCharInfo = dwCharInfo;
	}
	else {	//I dont know whether it can work. 101% cant
		PFL_HEADER pfl_header = &fl_header;
		DWORD offset = 0;
		int   i = 0;
		DWORD dwCharInfo = 0;

		if (!pfl_header->nSection)  // CJK (Monospace). File form: Header+Data.
		{
			long lIdx = -1;

			BYTE R = (wCode >> 8) & 0xFF;   //Region Code
			BYTE C = wCode & 0xFF;   //Bit Code

			switch (pfl_header->wCpFlag) {
			case CP932:
				if (R >= 0x81 && R <= 0x9F) {
					if (C >= 0x40 && C <= 0x7E)
						lIdx = (R - 0x81) * 188 + (C - 0x40);  //188 = (0x7E-0x40+1)+(0xFC-0x80+1); 			
					else if (C >= 0x80 && C <= 0xFC)
						lIdx = (R - 0x81) * 188 + (C - 0x80) + 63;  // 63 = 0x7E-0x40+1;			
				} else if (R >= 0xE0 && R <= 0xFC) {
					if (C >= 0x40 && C <= 0x7E)
						lIdx = 5828 + (R - 0xE0) * 188 + (C - 0x40);  // 5828 = 188 * (0x9F-0x81+1);
					else if (C >= 0x80 && C <= 0xFC)
						lIdx = 5828 + (R - 0xE0) * 188 + (C - 0x80) + 63;
				}
				break;

			case CP936:
				if ((R >= 0xA1 && R <= 0xFE) && (C >= 0xA1 && C <= 0xFE))
					lIdx = (R - 0xa1) * 94 + (C - 0xa1);  //94 = (0xFE-0xA1+1); 
				break;

			case CP949:
				if (R >= 0x81) {
					if (C >= 0x41 && C <= 0x7E)
						lIdx = ((R - 0x81) * 188 + (C - 0x41));   // 188 = (0x7E-0x41+1)+(0xFE-0x81+1);
					else if (C >= 0x81 && C <= 0xFE)
						lIdx = ((R - 0x81) * 188 + (C - 0x81) + 62);  // 62 = (0x7E-0x41+1);
				}
				break;

			case CP950:
				if (R >= 0xA1 && R <= 0xFE) {
					if (C >= 0x40 && C <= 0x7E)
						lIdx = ((R - 0xa1) * 157 + (C - 0x40));   // 157 = (0x7E-0x40+1)+(0xFE-0xA1+1);
					else if (C >= 0xA1 && C <= 0xFE)
						lIdx = ((R - 0xa1) * 157 + (C - 0xa1) + 63);  // 63 = (0x7E-0x40+1);
				}
				break;
			default:
				break;
			}
			//long lIdx = find_code_with_mbcs(wCode, pfl_header->wCpFlag);
			if (lIdx != -1) {
				dwCharInfo = sizeof(FL_HEADER) - sizeof(pfl_header->pSection) + lIdx * ((pfl_header->YSize + 7) / PIXELS_PER_BYTE * pfl_header->YSize);
				dwCharInfo |= (pfl_header->YSize << 26);
			}
		} else   // LatinChar (Nonmonospace).   File form: Header+Index+Data.
		{
			if (wCode <= 0xff) {
#ifdef WIN32
				offset = sizeof(FL_HEADER) - sizeof(pfl_header->pSection) + wCode * FONT_INDEX_TAB_SIZE;
#endif

#ifdef ARDUINO	//Arduino cannot comput in once
				offset = wCode;
				offset *= FONT_INDEX_TAB_SIZE;
				offset += (sizeof(FL_HEADER) - sizeof(pfl_header->pSection));

#endif
				FONTSEEK(Font, offset);
				FONTREAD(Font, &dwCharInfo, sizeof(DWORD));
			}
		}
		g_dwCharInfo = dwCharInfo;
	}

	int nWidth = GET_FONT_WIDTH(g_dwCharInfo);

#ifdef _DEBUG
	PRINT_STR("Char Info: ");
	show_b((byte_pointer)&wCode, sizeof(WORD));
	//PRINT_INT(wCode);
	PRINT_STR("Width: ");
	PRINT_INT(nWidth);
	PRINT_STR("\n");
#endif

	*Count = (WORD)((GET_FONT_WIDTH(g_dwCharInfo)) + 7) / PIXELS_PER_BYTE;

	if (g_dwCharInfo > 0) {
		DWORD nDataLen = *Count * fl_header.YSize;
		DWORD  dwOffset = GET_FONT_OFFADDR(g_dwCharInfo);    //Get Addr of ary (Low 26)

		FONTSEEK(Font, dwOffset);
		FONTREAD(Font, pData, nDataLen);
		return 1;
	}

	return 0;
}

BYTE UniFont::getY() {
	return fl_header.YSize;
}
