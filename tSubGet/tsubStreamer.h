/* tsubStreamer main header file. */
#ifndef _TSUBSTREAMER_H_
#define _TSUBSTREAMER_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define CAPTION_SIZE	50
#define CAPS_PER_COLL	5

enum _colours {
	BLACK,RED,GREEN,YELLOW,BLUE,
	MAGENTA,CYAN,WHITE
};

typedef struct _Sample {
	double cTime;
	BYTE *buffer;
	DWORD cBPos;
	DWORD sLength;
} Sample;

typedef struct _PageNumber {
	unsigned char mag;
	unsigned char tens;
	unsigned char units;
} PageNumber;

typedef struct _Caption {
	unsigned char fgCol;
	unsigned char bgCol;
	int pos[2];
	wchar_t text[CAPTION_SIZE];
	int noBreak;
} Caption;

typedef struct _Collection {
	double startTime;
	double endTime;
	int isLinked; 
	Caption cap[CAPS_PER_COLL];
	int cCapTextPos;
	int cCapPos;
} Collection;

typedef struct _Decoder {
	Sample cSmp;
	Collection *coll;
	int cCollSize;
	int cCollPos;
	int hasActiveColl;
	int state;
	PageNumber pnum;
} Decoder;

//tsubDecoder.c//
int initialiseDecoder(Decoder *d, int pageNumber);
void finaliseDecoder(Decoder *d);
int decodeSample(Decoder *d);
//tsubIO.c//
int parseFile(Decoder *d, wchar_t *filename);
void getSubFilename(wchar_t *inputFN, wchar_t *outputFN, int szOut);
int writeOutSubs(Decoder d, wchar_t *subFilename);

#endif