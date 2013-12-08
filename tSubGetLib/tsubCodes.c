#include <stdlib.h>
/*
   fixHamm8 is used to remove Hamming 8/4 protection,
   fixHamm24 is used to remove Hamming 24/18 protection,
   and fixParity is used to remove odd parity
   protection (as described in section 8 of the
   ETS 300 706 document).
*/
const unsigned char fixHamm8[256]={ 
	0x01, 0xff, 0x01, 0x01, 0xff, 0x00, 0x01, 0xff,
	0xff, 0x02, 0x01, 0xff, 0x0a, 0xff, 0xff, 0x07,
	0xff, 0x00, 0x01, 0xff, 0x00, 0x00, 0xff, 0x00,
	0x06, 0xff, 0xff, 0x0b, 0xff, 0x00, 0x03, 0xff,
	0xff, 0x0c, 0x01, 0xff, 0x04, 0xff, 0xff, 0x07,
	0x06, 0xff, 0xff, 0x07, 0xff, 0x07, 0x07, 0x07,
	0x06, 0xff, 0xff, 0x05, 0xff, 0x00, 0x0d, 0xff,
	0x06, 0x06, 0x06, 0xff, 0x06, 0xff, 0xff, 0x07,
	0xff, 0x02, 0x01, 0xff, 0x04, 0xff, 0xff, 0x09,
	0x02, 0x02, 0xff, 0x02, 0xff, 0x02, 0x03, 0xff,
	0x08, 0xff, 0xff, 0x05, 0xff, 0x00, 0x03, 0xff,
	0xff, 0x02, 0x03, 0xff, 0x03, 0xff, 0x03, 0x03,
	0x04, 0xff, 0xff, 0x05, 0x04, 0x04, 0x04, 0xff,
	0xff, 0x02, 0x0f, 0xff, 0x04, 0xff, 0xff, 0x07,
	0xff, 0x05, 0x05, 0x05, 0x04, 0xff, 0xff, 0x05,
	0x06, 0xff, 0xff, 0x05, 0xff, 0x0e, 0x03, 0xff,
	0xff, 0x0c, 0x01, 0xff, 0x0a, 0xff, 0xff, 0x09,
	0x0a, 0xff, 0xff, 0x0b, 0x0a, 0x0a, 0x0a, 0xff,
	0x08, 0xff, 0xff, 0x0b, 0xff, 0x00, 0x0d, 0xff,
	0xff, 0x0b, 0x0b, 0x0b, 0x0a, 0xff, 0xff, 0x0b,
	0x0c, 0x0c, 0xff, 0x0c, 0xff, 0x0c, 0x0d, 0xff,
	0xff, 0x0c, 0x0f, 0xff, 0x0a, 0xff, 0xff, 0x07,
	0xff, 0x0c, 0x0d, 0xff, 0x0d, 0xff, 0x0d, 0x0d,
	0x06, 0xff, 0xff, 0x0b, 0xff, 0x0e, 0x0d, 0xff,
	0x08, 0xff, 0xff, 0x09, 0xff, 0x09, 0x09, 0x09,
	0xff, 0x02, 0x0f, 0xff, 0x0a, 0xff, 0xff, 0x09,
	0x08, 0x08, 0x08, 0xff, 0x08, 0xff, 0xff, 0x09,
	0x08, 0xff, 0xff, 0x0b, 0xff, 0x0e, 0x03, 0xff,
	0xff, 0x0c, 0x0f, 0xff, 0x04, 0xff, 0xff, 0x09,
	0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0e, 0x0f, 0xff,
	0x08, 0xff, 0xff, 0x05, 0xff, 0x0e, 0x0d, 0xff,
	0xff, 0x0e, 0x0f, 0xff, 0x0e, 0x0e, 0xff, 0x0e };

const unsigned char fixParity[256]={	
	0x80, 0x01, 0x02, 0x83, 0x04, 0x85, 0x86, 0x07, 
	0x08, 0x89, 0x8A, 0x0B, 0x8C, 0x0D, 0x0E, 0x8F, 
	0x10, 0x91, 0x92, 0x13, 0x94, 0x15, 0x16, 0x97, 
	0x98, 0x19, 0x1A, 0x9B, 0x1C, 0x9D, 0x9E, 0x1F, 
	0x20, 0xA1, 0xA2, 0x23, 0xA4, 0x25, 0x26, 0xA7, 
	0xA8, 0x29, 0x2A, 0xAB, 0x2C, 0xAD, 0xAE, 0x2F, 
	0xB0, 0x31, 0x32, 0xB3, 0x34, 0xB5, 0xB6, 0x37, 
	0x38, 0xB9, 0xBA, 0x3B, 0xBC, 0x3D, 0x3E, 0xBF, 
	0x40, 0xC1, 0xC2, 0x43, 0xC4, 0x45, 0x46, 0xC7, 
	0xC8, 0x49, 0x4A, 0xCB, 0x4C, 0xCD, 0xCE, 0x4F, 
	0xD0, 0x51, 0x52, 0xD3, 0x54, 0xD5, 0xD6, 0x57, 
	0x58, 0xD9, 0xDA, 0x5B, 0xDC, 0x5D, 0x5E, 0xDF, 
	0xE0, 0x61, 0x62, 0xE3, 0x64, 0xE5, 0xE6, 0x67, 
	0x68, 0xE9, 0xEA, 0x6B, 0xEC, 0x6D, 0x6E, 0xEF, 
	0x70, 0xF1, 0xF2, 0x73, 0xF4, 0x75, 0x76, 0xF7, 
	0xF8, 0x79, 0x7A, 0xFB, 0x7C, 0xFD, 0xFE, 0x7F, 
	0x00, 0x81, 0x82, 0x03, 0x84, 0x05, 0x06, 0x87, 
	0x88, 0x09, 0x0A, 0x8B, 0x0C, 0x8D, 0x8E, 0x0F, 
	0x90, 0x11, 0x12, 0x93, 0x14, 0x95, 0x96, 0x17, 
	0x18, 0x99, 0x9A, 0x1B, 0x9C, 0x1D, 0x1E, 0x9F, 
	0xA0, 0x21, 0x22, 0xA3, 0x24, 0xA5, 0xA6, 0x27, 
	0x28, 0xA9, 0xAA, 0x2B, 0xAC, 0x2D, 0x2E, 0xAF, 
	0x30, 0xB1, 0xB2, 0x33, 0xB4, 0x35, 0x36, 0xB7, 
	0xB8, 0x39, 0x3A, 0xBB, 0x3C, 0xBD, 0xBE, 0x3F, 
	0xC0, 0x41, 0x42, 0xC3, 0x44, 0xC5, 0xC6, 0x47, 
	0x48, 0xC9, 0xCA, 0x4B, 0xCC, 0x4D, 0x4E, 0xCF, 
	0x50, 0xD1, 0xD2, 0x53, 0xD4, 0x55, 0x56, 0xD7, 
	0xD8, 0x59, 0x5A, 0xDB, 0x5C, 0xDD, 0xDE, 0x5F, 
	0x60, 0xE1, 0xE2, 0x63, 0xE4, 0x65, 0x66, 0xE7, 
	0xE8, 0x69, 0x6A, 0xEB, 0x6C, 0xED, 0xEE, 0x6F, 
	0xF0, 0x71, 0x72, 0xF3, 0x74, 0xF5, 0xF6, 0x77, 
	0x78, 0xF9, 0xFA, 0x7B, 0xFC, 0x7D, 0x7E, 0xFF };

int fixHamm24(unsigned char triplet[3]) {
	int result = triplet[0] | (triplet[1] << 8) | (triplet[2] << 16);
	unsigned char test, i;

	//Tests A-F correspond to bits 0-6 respectively in 'test'.
	for (i = 0, test = 0; i < 23; i++)
		test ^= ((result >> i) & 1) * (i + 33);
	//Only parity bit is tested for bit 24
	test ^= ((result >> 18) & 32);
	//Equivalent: test ^= ((result >> 23) & 1) * 32; 

	if ((test & 31) != 31) { //Not all tests A-E correct
		if (test & 32) { //F correct: Double error
			return -1;
		}
		//Test F incorrect: Single error
		result ^= 1 << (30 - test);
	}

	return	(result & 0x4) >> 2 | 
			(result & 0x70) >> 3 | 
			(result & 0x7F00) >> 4 | 
			(result & 0x7F0000) >> 5;
}
/* 
   Map to convert LangID to correct index of latinNOSubsets.
*/
const int langIdMap[14] = {
	1,0,1,2,3,4,5,6,7,8,9,10,11,12
};

/*
   The Latin National Option Subset (Table 36 of ETS 300 706)
   Values are in unicode (hexadecimal format).
   In short form:
	#,ů,č,ť,ž,ý,í,ř,é,á,ě,ú,š
	£,$,@,←,½,→,↑,#,–,¼,∥,¾,÷
	#,õ,Š,Ä,Ö,Ž,Ü,Õ,š,ä,ö,ž,ü
	é,ï,à,ë,ê,ù,î,#,è,â,ô,û,ç
	#,$,§,Ä,Ö,Ü,^,_,°,ä,ö,ü,ß
	£,$,é,°,ç,→,↑,#,ù,à,ò,è,ì
	#,$,Š,ė,ę,Ž,č,ū,š,ą,ų,ž,į
	#,ń,ą,Ƶ,Ś,Ł,ć,ó,ę,ż,ś,ł,ź
	ç,$,i,á,é,í,ó,ú,¿,ü,ñ,è,à
	#,¤,Ţ,Â,Ş,Ă,Î,ı,ţ,â,ş,ă,î
	#,Ë,Č,Ć,Ž,Đ,Š,ë,č,ć,ž,đ,š
	#,¤,É,Ä,Ö,Å,Ü,_,é,ä,ö,å,ü
	₤,ğ,İ,Ş,Ö,Ç,Ü,Ğ,ı,ş,ö,ç,ü
	Note that the first item in the Turkish subset should be
	the Turkish Lira sign (introduced recently).
*/
const unsigned __int16 latinNOSubsets[13][13] = 
{
	{ // Czech/Slovak
		0x0023, 0x016f, 0x010d, 0x0165, 0x017e, 0x00fd, 0x00ed, 
		0x0159, 0x00e9, 0x00e1, 0x011b, 0x00fa, 0x0161
	},

	{ // English
		0x00a3, 0x0024, 0x0040, 0x2190, 0x00bd, 0x2192, 0x2191, 
		0x0023, 0x2013, 0x00bc, 0x2225, 0x00be, 0x00f7
	},

	{ // Estonian
		0x0023, 0x00f5, 0x0160, 0x00c4, 0x00d6, 0x017d, 0x00dc, 
		0x00d5, 0x0161, 0x00e4, 0x00f6, 0x017e, 0x00fc
	},

	{ // French
		0x00e9, 0x00ef, 0x00e0, 0x00eb, 0x00ea, 0x00f9, 0x00ee, 
		0x0023, 0x00e8, 0x00e2, 0x00f4, 0x00fb, 0x00e7
	},

	{ // German
		0x0023, 0x0024, 0x00a7, 0x00c4, 0x00d6, 0x00dc, 0x005e, 
		0x005f, 0x00b0, 0x00e4, 0x00f6, 0x00fc, 0x00df
	},

	{ // Italian
		0x00a3, 0x0024, 0x00e9, 0x00b0, 0x00e7, 0x2192, 0x2191, 
		0x0023, 0x00f9, 0x00e0, 0x00f2, 0x00e8, 0x00ec
	},

	{ // Lettish/Lithuanian
		0x0023, 0x0024, 0x0160, 0x0117, 0x0119, 0x017d, 0x010d, 
		0x016b, 0x0161, 0x0105, 0x0173, 0x017e, 0x012f
	},

	{ // Polish
		0x0023, 0x0144, 0x0105, 0x01b5, 0x015a, 0x0141, 0x0107, 
		0x00f3, 0x0119, 0x017c, 0x015b, 0x0142, 0x017a
	},

	{ // Portuguese/Spanish
		0x00e7, 0x0024, 0x0069, 0x00e1, 0x00e9, 0x00ed, 0x00f3, 
		0x00fa, 0x00bf, 0x00fc, 0x00f1, 0x00e8, 0x00e0
	},

	{ // Romanian
		0x0023, 0x00a4, 0x0162, 0x00c2, 0x015e, 0x0102, 0x00ce, 
		0x0131, 0x0163, 0x00e2, 0x015f, 0x0103, 0x00ee
	},

	{ // Serbian/Croatian/Slovenian
		0x0023, 0x00cb, 0x010c, 0x0106, 0x017d, 0x0110, 0x0160, 
		0x00eb, 0x010d, 0x0107, 0x017e, 0x0111, 0x0161
	},

	{ // Swedish/Finnish/Hungarian
		0x0023, 0x00a4, 0x00c9, 0x00c4, 0x00d6, 0x00c5, 0x00dc, 
		0x005f, 0x00e9, 0x00e4, 0x00f6, 0x00e5, 0x00fc
	},

	{ //Turkish
		0x20a4, 0x011f, 0x0130, 0x015e, 0x00d6, 0x00c7, 0x00dc, 
		0x011e, 0x0131, 0x015f, 0x00f6, 0x00e7, 0x00fc
	}
};

/*
   Misc codes
*/
const wchar_t *colourSet[8]	= 
{
	L"Black", L"Red", L"Green", L"Yellow",
	L"Blue", L"Magenta", L"Cyan", L"White" 
};