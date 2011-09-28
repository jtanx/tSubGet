/* Header file for tsubDecoder.c */

#define TT_PACKETSIZE	42
#define TT_PACKETSPP	26
#define TT_PAGESIZE		(TT_PACKETSIZE*TT_PACKETSPP)
#define TT_SPACE		0x20
#define BLOCK_SIZE		50

#define CurrentColl(d) (d)->coll[(d)->cCollPos]
#define PreviousColl(d) (d)->coll[(d)->cCollPos-1]
#define CurrentCap(d) CurrentColl(d).cap[CurrentColl(d).cCapPos]

enum _decoderStates {
	DECODER_IDLE,
	DECODER_GETTEXT,
	DECODER_FINISH,
	DECODER_ERROR,
	DECODER_OK
};

extern unsigned char fixHamm48[256];
extern unsigned char fixParity[256];

