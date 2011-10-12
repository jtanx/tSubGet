#include "tSubGetLib.h"

#define CAPTION_BLOCK		3
#define CAPTION_TEXT_SIZE	50
#define CLUSTER_BLOCK		25

enum _colours {
	BLACK,RED,GREEN,YELLOW,BLUE,
	MAGENTA,CYAN,WHITE
};

typedef struct _Caption {
	wchar_t *text;
	ArrayInfo textI;
	int fgColour;
	int bgColour;
	int posX;
	int posY;
	int sizeX;
	int sizeY;
	int noBreak;
	int startFlag;
} Caption;

typedef struct _CaptionCluster {
	__int64 timeStart;
	__int64 timeEnd;
	unsigned hasActiveCap;
	Caption *caps;
	ArrayInfo capsI;
} CaptionCluster;

typedef struct _RTime {
	__int64 h;
	__int64 m;
	__int64 s;
	__int64 ms;
} RTime;

extern const wchar_t subsets[1][13];

int capStart(CaptionCluster *cc, ArrayInfo *ccI);
void capEnd(CaptionCluster *cc, ArrayInfo *ccI);
