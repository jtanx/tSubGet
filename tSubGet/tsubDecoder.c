#include "tsubStreamer.h"
#include "tsubDecoder.h"

static int initialiseCollection(Decoder *d){
	if (d->cCollSize == 0){
		d->coll = (Collection*)malloc(sizeof(Collection)*BLOCK_SIZE);
		d->cCollSize = BLOCK_SIZE;
		if (!d->coll)
			return FALSE;
	}
	else if (d->cCollPos == d->cCollSize){
		d->cCollSize += BLOCK_SIZE;
		d->coll = (Collection*)realloc(d->coll,sizeof(Collection)*d->cCollSize);
		if (!d->coll)
			return FALSE;
	}
	if (!d->hasActiveColl){
		d->hasActiveColl = TRUE;
		memset(&(CurrentColl(d)),0,sizeof(Collection));
		CurrentColl(d).startTime = d->cSmp.cTime;
	}

	return TRUE;
}

//This also needs improving. I can't think of another way to do this though.
static int isDuplicateCollection (Decoder *d){
	if (d->cCollPos > 0){
		if (PreviousColl(d).cCapPos == CurrentColl(d).cCapPos){
			int i;
			for (i = 0; i < CurrentColl(d).cCapPos; i++){
				if (wcscmp(PreviousColl(d).cap[i].text,CurrentColl(d).cap[i].text))
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

__inline static void finaliseCollection(Decoder *d){
	//If it's duplicate, go back one collection, and change its end time
	//to reflect now.
	if (isDuplicateCollection(d))
		d->cCollPos--;
	
	CurrentColl(d).endTime = d->cSmp.cTime;
	d->cCollPos++;
	
	d->hasActiveColl = FALSE;
}

static int initialiseCap(Decoder *d, int currentPacket){
	if (!initialiseCollection(d)) //To zero the current collection, if needed.
			return FALSE;

	if (CurrentColl(d).cCapPos == CAPS_PER_COLL){ //Link it to the next group.
		CurrentColl(d).isLinked = TRUE;
		finaliseCollection(d);
		if (!initialiseCollection(d))
			return FALSE;
	}
	CurrentCap(d).fgCol = WHITE;
	CurrentCap(d).pos[0] = currentPacket;
	d->state = DECODER_IDLE;
	return TRUE;
}

__inline static void finaliseCap(Decoder *d){
	CurrentCap(d).text[CurrentColl(d).cCapTextPos] = L'\0';
	CurrentColl(d).cCapTextPos = 0;
	CurrentColl(d).cCapPos++;
}

static int parseControlCode(Decoder *d, unsigned char val, int currentPacket){
	unsigned char bgCol = CurrentCap(d).bgCol;
	unsigned char fgCol = CurrentCap(d).fgCol;

	if (val >= 0 && val <= 0x7)
		fgCol = val;
	switch (val){
		case 0x1C:
			bgCol = BLACK; break;
		case 0x1D:
			bgCol = fgCol; break;
		case TT_SPACE:
			CurrentCap(d).pos[1]++; break;
		case 0x0B:{
			if (d->state != DECODER_GETTEXT)
				d->state = DECODER_GETTEXT;
		} break;
		case 0x0A:{ //EOL
			if (CurrentCap(d).text[CurrentColl(d).cCapTextPos-1] == TT_SPACE)
				CurrentCap(d).noBreak = TRUE;
			d->state = DECODER_FINISH;
		} break;
	}//End switch
	if (d->state != DECODER_IDLE && CurrentColl(d).cCapTextPos > 0){
		if (CurrentCap(d).fgCol != fgCol ||
			CurrentCap(d).bgCol != bgCol){
			finaliseCap(d);
			if (!initialiseCap(d,currentPacket))
				return FALSE;
			d->state = DECODER_GETTEXT;
		}
	}

	CurrentCap(d).fgCol = fgCol;
	CurrentCap(d).bgCol = bgCol;
	return TRUE;
}

//Definitely needs improving...
__inline static wchar_t translateChar(Decoder *d, unsigned char val){
	wchar_t retVal = (wchar_t)val; //This is not exactly right, but....
	switch (val){
		//Latin option subset.
		case 0x23: retVal = 0xA3; break;
		case 0x5B: retVal = 0x2C2; break;
		case 0x5C: retVal = 0xBD; break;
		case 0x5D: retVal =	0x2C3; break;
		case 0x5E: retVal = 0x2C4; break;
		case 0x5F: retVal = 0x23; break;
		case 0x60: retVal = 0x2013; break;
		case 0x7B: retVal = 0xBC; break;
		case 0x7C: retVal = 0x2225; break;
		case 0x7D: retVal = 0xBE; break;
		case 0x7E: retVal = 0xF7; break;
	}
	return retVal;
}

static int decodeSubtitlePage(Decoder *d){
	int i, j;

	//Finalise the previous collection.
	if (d->hasActiveColl)
		finaliseCollection(d);

	d->cSmp.cBPos = TT_PACKETSIZE + 2;
	for (i = 0; i < TT_PACKETSPP - 1; i++, d->cSmp.cBPos += TT_PACKETSIZE){
		unsigned char mag = fixHamm48[d->cSmp.buffer[d->cSmp.cBPos-2]] & 0x7;
		if (mag != d->pnum.mag) continue;

		if (!initialiseCap(d,i))
			return DECODER_ERROR;
		for (j = 0; j < TT_PACKETSIZE - 2 && d->state != DECODER_FINISH; j++){
			unsigned char val = fixParity[d->cSmp.buffer[d->cSmp.cBPos+j]];	

			if (val >= 0 && val <= 0x1F || 
				(d->state == DECODER_IDLE && val == TT_SPACE)){
				if (!parseControlCode(d,val,i))
					return DECODER_ERROR;
			} else {
				if (CurrentColl(d).cCapTextPos == CAPTION_SIZE-1){
					unsigned char tempVals[2] = {CurrentCap(d).bgCol,
												 CurrentCap(d).fgCol};
					CurrentCap(d).noBreak = TRUE;
					finaliseCap(d);
					initialiseCap(d,i);
					CurrentCap(d).bgCol = tempVals[0]; //TODO: What about position data?
					CurrentCap(d).fgCol = tempVals[1];
					d->state = DECODER_GETTEXT;
				}
				CurrentCap(d).text[CurrentColl(d).cCapTextPos++] = translateChar(d,val);
			}
		}
		finaliseCap(d);
	}
	return DECODER_OK;
}

int initialiseDecoder(Decoder *d, int pageNumber){
	if (pageNumber < 100 || pageNumber > 999)
		return FALSE;

	memset(d,0,sizeof(Decoder));
	d->pnum.mag = pageNumber/100;
	d->pnum.tens = (pageNumber%100)/10;
	d->pnum.units = pageNumber%10;
	if (d->pnum.mag == 8) d->pnum.mag = 0;

	return TRUE;
}

void finaliseDecoder(Decoder *d){
	if (d->coll){
		free (d->coll);
		d->coll = NULL;
	}
}

__inline static int isEqualPageNumbers(PageNumber p1, PageNumber p2){
	if (p1.mag == p2.mag &&
		p1.tens == p2.tens &&
		p1.units == p2.units)
		return TRUE;
	return FALSE;
}

int decodeSample(Decoder *d){
	int i;
	unsigned char rawHeader[10]; //Sample buffer is read only.
	PageNumber currentPnum;

	if (d->cSmp.sLength < TT_PAGESIZE)
		return FALSE;

	for (i = 0; i < 10; i++){
		rawHeader[i] = fixHamm48[d->cSmp.buffer[i]];
		if (rawHeader[i] & 0x80)
			return TRUE; //Just skip errors, (not fatal)
	}

	currentPnum.mag = rawHeader[0] & 0x7;
	currentPnum.tens = rawHeader[3];
	currentPnum.units = rawHeader[2];
	//TODO: Add proper language support.
	
	if (!isEqualPageNumbers(currentPnum,d->pnum))
		return TRUE;
	
	if (decodeSubtitlePage(d) == DECODER_ERROR)
		return FALSE;
	return TRUE;
}