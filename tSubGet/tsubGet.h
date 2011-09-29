/* tsubStreamer main header file. */
#ifndef _TSUBSTREAMER_H_
#define _TSUBSTREAMER_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/*
#define CAPTION_SIZE	50
#define CAPS_PER_COLL	5
*/

#define CAP_LENGTH		51

enum _colours {
	BLACK,RED,GREEN,YELLOW,BLUE,
	MAGENTA,CYAN,WHITE
};

typedef struct _Sample {
	long double sTime;
	BYTE *buffer;
	DWORD cBPos;
	DWORD bufLen;	
} Sample;

typedef struct _Caption {
	unsigned lang[2];
	unsigned tsIndex;
	unsigned colour[2];
	unsigned pos[2];
	unsigned noBreak;
	wchar_t text[CAP_LENGTH];
} Caption;

typedef struct _Tstamp {
	long double startTime;
	long double endTime;
} Tstamp;

typedef struct _DecoderMeta {
	unsigned state;
	unsigned curPage;
	unsigned curLang[2];
	unsigned forceLang[2];
	unsigned forceLangFlag;
	unsigned capIdx;
	unsigned capTextIdx;
	unsigned tsIdx;
	unsigned capSize;
	unsigned tsSize;
	unsigned hasActiveTs;
} DecoderMeta;

typedef struct _Decoder {
	Sample smp;
	DecoderMeta meta;
	Caption *caps;
	Tstamp *ts;
	unsigned pageNumber;
	unsigned capCount; //The count of caps, NOT the index.
} Decoder;

/*
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
*/

int initialiseDecoder(Decoder *d, unsigned pageNumber, unsigned forceLang[2]);
void finaliseDecoder(Decoder *d);
int decodeSample(Decoder *d);

//tsubDecoder.c//
/*int initialiseDecoder(Decoder *d, int pageNumber);
void finaliseDecoder(Decoder *d);
int decodeSample(Decoder *d);
*/
//tsubInput.c//
int parseFile(Decoder *d, wchar_t *filename);
//tsubOutput.c//
void getSubFilename(wchar_t *inputFN, wchar_t *outputFN, int szOut);
int writeOutSubs(Decoder d, wchar_t *subFilename);

#endif