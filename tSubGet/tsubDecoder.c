#include "tsubGet.h"
#include "tsubDecoder.h"

static int initialiseTstamp(Decoder *d){
	if (d->meta.tsIdx == d->meta.tsSize){
		d->meta.tsSize += ALLOC_BLOCK;
		d->ts = realloc(d->ts,sizeof(Tstamp)*d->meta.tsSize);
		if (!d->ts)
			return FALSE;
	}
	d->meta.hasActiveTs = TRUE;
	d->ts[d->meta.tsIdx].startTime = d->smp.sTime;
	d->ts[d->meta.tsIdx].endTime = 0;
	return TRUE;
}

__inline static int finaliseTstamp(Decoder *d){
	d->ts[d->meta.tsIdx++].endTime = d->smp.sTime;
	d->meta.hasActiveTs = FALSE;

	return TRUE;
}

static int initialiseCap(Decoder *d, unsigned vPos){
	if (d->meta.capIdx == d->meta.capSize){
		d->meta.capSize += ALLOC_BLOCK;
		d->caps = realloc(d->caps, sizeof(Caption)*d->meta.capSize);
		if (!d->caps)
			return FALSE;
	}
	if (!d->meta.hasActiveTs){
		if (!initialiseTstamp(d))
			return FALSE;
	}
	memset(&(d->caps[d->meta.capIdx]),0,sizeof(Caption));
	d->caps[d->meta.capIdx].pos[1] = vPos;
	d->caps[d->meta.capIdx].colour[0] = WHITE;
	d->caps[d->meta.capIdx].tsIndex = d->meta.tsIdx;
	d->caps[d->meta.capIdx].lang[0] = d->meta.curLang[0];
	d->caps[d->meta.capIdx].lang[1] = d->meta.curLang[1];

	d->meta.capTextIdx = 0;
	d->meta.state = DECODER_IDLE;
	return TRUE;
}

__inline static int finaliseCap(Decoder *d){
	wchar_t dec = d->caps[d->meta.capIdx].text[d->meta.capTextIdx-1] ;

	switch (dec){
		case TT_SPACE:d->caps[d->meta.capIdx].noBreak = TRUE; break;
	}
	d->caps[d->meta.capIdx].text[d->meta.capTextIdx] = L'\0';
	d->meta.capIdx++;
	d->capCount++;

	return TRUE;
}

__inline static int parseControlCode(Decoder *d, unsigned char val, int vPos){
	unsigned colour[2] = {d->caps[d->meta.capIdx].colour[0],
						  d->caps[d->meta.capIdx].colour[1]};
	if (val >= 0 && val <= 0x7)
		colour[0] = val;
	else switch (val){
		case 0x1C: colour[1] = BLACK; break; //BG colour becomes black
		case 0x1D: colour[1] = colour[0]; break; //BG colour becomes FG colour
		case 0x0B: d->meta.state = DECODER_WORKING; break; //Start of text block
		case 0x0A: d->meta.state = DECODER_FINISH; break; //End of text block
	}
	if (d->meta.state != DECODER_IDLE && d->meta.capTextIdx > 0){
		if (d->caps[d->meta.capIdx].colour[0] != colour[0] ||
			d->caps[d->meta.capIdx].colour[1] != colour[1]){
			finaliseCap(d);
			if (!initialiseCap(d,vPos))
				return FALSE;
			
			d->meta.state = DECODER_WORKING;
		}
	}
	
	d->caps[d->meta.capIdx].colour[0] = colour[0];
	d->caps[d->meta.capIdx].colour[1] = colour[1];
	return TRUE;
}

__inline static int parseValue(Decoder *d, unsigned char val, int vPos){
	if (d->meta.capTextIdx == CAP_LENGTH){
		unsigned colour[2] = {d->caps[d->meta.capIdx].colour[0],
							  d->caps[d->meta.capIdx].colour[1]};
		unsigned hPos = d->caps[d->meta.capIdx].pos[0];
		d->caps[d->meta.capIdx].noBreak = TRUE;

		finaliseCap(d);
		initialiseCap(d,vPos);
		d->caps[d->meta.capIdx].colour[0] = colour[0];
		d->caps[d->meta.capIdx].colour[1] = colour[1];
		d->caps[d->meta.capIdx].pos[0] = hPos;
		d->meta.state = DECODER_WORKING;
	}
	mbtowc(&(d->caps[d->meta.capIdx].text[d->meta.capTextIdx]),
		   &val,1);
	d->meta.capTextIdx++;

	return TRUE;
}

static int decodeSubtitlePage(Decoder *d){
	int i, j;

	if (d->meta.hasActiveTs)
		finaliseTstamp(d);
	
	d->smp.cBPos  = TT_PACKETSIZE;
	for (i = 0; i < TT_PACKETSPP - 1; i++, d->smp.cBPos += TT_PACKETSIZE){
		unsigned char mag = fixHamm48[d->smp.buffer[d->smp.cBPos]] & 0x7;
		if (mag & 0x80 || mag != d->pageNumber >> 8) continue;

		if (!initialiseCap(d,i))
			return DECODER_ERROR;
		for (j = 2; j < TT_PACKETSIZE && d->meta.state != DECODER_FINISH; j++){
			unsigned char val = fixParity[d->smp.buffer[d->smp.cBPos+j]];
			
			if (d->meta.state == DECODER_IDLE && val == TT_SPACE)
				d->caps[d->meta.capIdx].pos[0]++;
			else if (val >= 0x0 && val <= 0x1F){
				if (!parseControlCode(d,val,i))
					return DECODER_ERROR;
			}
			else{
				if (!parseValue(d,val,i))
					return DECODER_ERROR;
			}
		}
		finaliseCap(d);
	}//End outer for loop
	return d->meta.state;
}

int initialiseDecoder(Decoder *d, unsigned pageNumber, unsigned forceLang[2]){
	memset(d,0,sizeof(Decoder));

	d->pageNumber = pageNumber;
	d->caps = malloc(sizeof(Caption)*ALLOC_BLOCK);
	d->ts = malloc(sizeof(Tstamp)*ALLOC_BLOCK);
	d->meta.capSize = ALLOC_BLOCK;
	d->meta.tsSize = ALLOC_BLOCK;
	if (forceLang[0] != 0 || forceLang[1] != 0){
		d->meta.forceLangFlag = TRUE;
		d->meta.forceLang[0] = forceLang[0];
		d->meta.forceLang[1] = forceLang[1];
	}
	
	if (!d->caps || !d->ts)
		return FALSE;
	return TRUE;
}

void finaliseDecoder(Decoder *d){
	if (d->caps) free (d->caps);
	if (d->ts) free (d->ts);
}

int decodeSample(Decoder *d){
	int i;
	unsigned char rawHeader[10];
	unsigned packetNum;

	if (d->smp.bufLen < TT_PAGESIZE)
		return FALSE;

	for (i = 0; i < 10; i++){
		rawHeader[i] = fixHamm48[d->smp.buffer[i]];
		if (rawHeader[i] & 0x80) //Erroneous packet
			return TRUE;
	}

	packetNum = rawHeader[1] | rawHeader[0] >> 3;
	if (packetNum != 0) //Currently only support Pkt/0 types
		return TRUE;
	//TODO: Add proper language support. Needs to be able to decode Pkt 28/Fmt 1
	//to do so though, or just ask user...
	d->meta.curPage = ((rawHeader[0] & 0x7) << 8)|(rawHeader[3] << 4)|(rawHeader[2]);
	
	if (d->meta.curPage != d->pageNumber)
		return TRUE;
	else if (decodeSubtitlePage(d) == DECODER_ERROR)
		return FALSE;

	return TRUE;
}