#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dshow.h>
#include "tSubGetLibV2.h"
#include "..\\NullGrabber\\NullGrabberC.h"

#define TT_PACKETSIZE	42
//#define TT_PACKETSPP	26
//#define TT_PAGESIZE		(TT_PACKETSIZE*TT_PACKETSPP)

typedef struct ArrayBuilder ArrayBuilder;
typedef struct ArrayBuilder StringBuilder;

enum CaptionFlags {
	CAPTION_BLACK	= 0x01,
	CAPTION_RED		= 0x02,
	CAPTION_GREEN	= 0x04,
	CAPTION_YELLOW	= 0x08,
	CAPTION_BLUE	= 0x10,
	CAPTION_MAGENTA	= 0x20,
	CAPTION_CYAN	= 0x40,
	CAPTION_WHITE	= 0x80,
	CAPTION_LARGE_X	= 0x100,
	CAPTION_LARGE_Y	= 0x200,
	CAPTION_NOBREAK	= 0x400
};

typedef struct Caption {
	StringBuilder *text;
	int flags;
	int posX;
	int posY;
} Caption;

typedef struct CaptionBlock {
	__int64 start;
	__int64 end;
	ArrayBuilder *caps;
} CaptionsBlock;

struct TSG_Parser {
	unsigned pageNumber;
	TSG_LangID langId;
	NGC_Info *fileReader;
	TSG_Info infoCallback;
	ArrayBuilder *captionsBlock;
};

//tSubCodes.c
extern const unsigned char fixHamm8[256];
extern const unsigned char fixParity[256];
int fixHamm24(unsigned char triplet[3]);

//toolset
BOOL ab_init(ArrayBuilder **ab, size_t initialCount, size_t unitSize);
BOOL ab_add(ArrayBuilder *ab, void *data);
BOOL ab_add_multiple(ArrayBuilder *ab, void *data, size_t count);
size_t ab_length(ArrayBuilder *ab) ;
void *ab_finalise(ArrayBuilder **ab, size_t *count);

BOOL sb_init(StringBuilder **sb, size_t initialCount) ;
BOOL sb_add(StringBuilder *sb, unsigned __int32 codePoint);
char *sb_peek(StringBuilder *sb);
size_t sb_length(StringBuilder *sb) ;
void sb_free(StringBuilder **sb) ;

//tSubParser.c
HRESULT parseSampleV2(unsigned char *buffer, long length, __int64 timestamp, void *userData); 