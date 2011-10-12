#include "tSubGetLib.h"

#define TT_PACKETSIZE	42
#define TT_PACKETSPP	26
#define TT_PAGESIZE		(TT_PACKETSIZE*TT_PACKETSPP)

#define EXT_SIZE		5
#define EXT				L".srt"

const unsigned char fixHamm48[256];
extern const unsigned char fixParity[256];

int readerInit(CaptionsParser *p);
void readerClose(CaptionsParser *p);

int ccInit(CaptionCluster **cc, ArrayInfo *ccI);
void ccClose(CaptionCluster *cc, ArrayInfo ccI);
int ccStart(CaptionCluster **cc, ArrayInfo *ccI, __int64 timeStart);
void ccEnd(CaptionCluster *cc, ArrayInfo *ccI, __int64 timeEnd);
int ccSingleWriteOut(CaptionCluster *cc, unsigned index, unsigned colouredOutput, __int64 delay, FILE *fp);
int capAdd(CaptionCluster *cc, ArrayInfo *ccI, unsigned posX, unsigned posY, unsigned lang, unsigned char val);