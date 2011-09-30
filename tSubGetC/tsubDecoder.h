extern unsigned char fixHamm48[256];
extern unsigned char fixParity[256];
extern const wchar_t subsets[1][13];

#define TT_PACKETSIZE	42
#define TT_PACKETSPP	26
#define TT_PAGESIZE		(TT_PACKETSIZE*TT_PACKETSPP)
#define TT_SPACE		0x20

#define ALLOC_BLOCK		50

enum _decoderStates {
	DECODER_IDLE,
	DECODER_WORKING,
	DECODER_FINISH,
	DECODER_ERROR
};

