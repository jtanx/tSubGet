#include <Dshow.h>
#include "tSubGetLib.h"
#include "..\\NullGrabber\\NullGrabber_h.h"
#include "..\\baseLib\\BaseLib.h"

#ifdef _DEBUG
	#pragma comment (lib, "..\\Bin\\Debug\\baseLib.lib")
#else
	#pragma comment (lib, "..\\Bin\\baseLib.lib")
#endif

#define TT_PACKETSIZE	42
#define TT_PACKETSPP	26
#define TT_PAGESIZE		(TT_PACKETSIZE*TT_PACKETSPP)

#define EXT_SIZE		5
#define EXT				L".srt"

const unsigned char fixHamm48[256];
extern const unsigned char fixParity[256];
extern const wchar_t subsets[1][13];

enum colours {
	BLACK,RED,GREEN,YELLOW,BLUE,
	MAGENTA,CYAN,WHITE
};

typedef struct NGCallback {
	const INullGrabberCBVtbl *lpVtbl;
	CaptionsParser *pParent;
} NGCallback;

typedef struct FileReader{
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent *pEvent;
	IMediaSeeking *pStatus;
	IBaseFilter *pSourceF;
	IBaseFilter *pNullGrabberF;
	INullGrabber *pNullGrabber;
	NGCallback NullGrabberCB;
	__int64 duration;
	__int64 currentPos;
	int state;
} FileReader;

struct CaptionsParser {
	ParserOpts po;
	FileReader fr;
	Queue cc;
	HANDLE hAbort;
};

typedef struct Sample {
	unsigned char *pBuf;
	__int64 time;
	long bufSize;
} Sample;

typedef struct Caption {
	StringBase text;
	int fgColour;
	int bgColour;
	int posX;
	int posY;
	int sizeX;
	int sizeY;
	int noBreak;
	int startFlag;
} Caption;

typedef struct CaptionCluster {
	__int64 timeStart;
	__int64 timeEnd;
	unsigned hasActiveCap;
	Queue caps;
} CaptionCluster;

typedef struct RTime {
	__int64 h;
	__int64 m;
	__int64 s;
	__int64 ms;
} RTime;

int ccStart(Queue ccq, __int64 timeStart);
void ccEnd(Queue ccq, __int64 timeEnd);
int capAdd(CaptionsParser *p, unsigned posX, unsigned posY, unsigned val);

int readerInit(CaptionsParser *p);
void readerClose(CaptionsParser *p);

int parseSample(CaptionsParser *p, Sample smp);

void ngCallbackInit(CaptionsParser *p);