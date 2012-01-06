#include "tSubInternal.h"
static int ccIsDuplicate(CaptionCluster *ccA, CaptionCluster *ccB);
static int capDuplicate(CaptionCluster *cc);

int ccStart(Queue ccq, __int64 timeStart){
	CaptionCluster *cc;
	if (!qbAdd(ccq, FALSE, &cc))
		return PARSER_E_MEM;
	else if (!(cc->caps = qbCreate(sizeof(Caption))))
		return PARSER_E_MEM;

	cc->timeStart = timeStart/10000;
	return PARSER_OK;
}

void ccEnd(Queue ccq, __int64 timeEnd){
	CaptionCluster *ccCurrent, *ccPrev;
	Caption *c;

	if (!qbPeek(ccq, 0, FALSE, &ccCurrent))
		return;
	capEnd(ccCurrent);

	//Condition 1: No caps in cluster. Condition 2: Duplicate cluster.
	qbPeek(ccCurrent->caps, 0, TRUE, &c);
	qbPeek(ccq, 1, FALSE, &ccPrev);
	if (!c || (ccPrev && ccIsDuplicate(ccPrev, ccCurrent))){
		while (qbPeek(ccCurrent->caps, 0, TRUE, &c)){
			sbFree(c->text);
			qbFreeSingle(ccCurrent->caps, TRUE);
		}

		qbClose(&ccCurrent->caps);
		qbFreeSingle(ccq, FALSE);
		ccCurrent = ccPrev;
		if (!ccPrev) return; //Happens in condition 1 with no previous cap.
	}
	
	timeEnd /= 10000;
	if (ccCurrent->timeStart > timeEnd || ccCurrent->timeStart < 0)
		ccCurrent->timeStart = 0;
	ccCurrent->timeEnd = timeEnd;
}

int capStart(CaptionCluster *cc){
	if (!cc->hasActiveCap){
		Caption *c;
		if (!qbAdd(cc->caps, FALSE, &c))
			return PARSER_E_MEM;
		if (!(c->text = sbCreate(TT_PACKETSIZE, -1)))
			return PARSER_E_MEM;

		c->fgColour = WHITE;
		cc->hasActiveCap = TRUE;
	}
	return PARSER_OK;
}

void capEnd(CaptionCluster *cc){
	if (cc->hasActiveCap){
		Caption *currentCap, *previousCap;
		
		if (!qbPeek(cc->caps, 0, FALSE, &currentCap))
			return;
		qbPeek(cc->caps, 1, FALSE, &previousCap);
		
		if (sbGetCharCount(currentCap->text) == 0){
			sbFree(currentCap->text);
			qbFreeSingle(cc->caps, FALSE);
		} else if (previousCap && currentCap->posY == previousCap->posY)
			previousCap->noBreak = TRUE;
		
		cc->hasActiveCap = FALSE;
	}
}

int capAdd(CaptionsParser *p, unsigned posX, unsigned posY, unsigned val){
	int ret;
	CaptionCluster *cc;
	Caption *currentCap;

	if (!p || !qbPeek(p->cc, 0, FALSE, &cc))
		return PARSER_E_PARAMS;
	else if ((ret = capStart(cc)) != PARSER_OK)
		return ret;
	else if (!qbPeek(cc->caps, 0, FALSE, &currentCap))
		return PARSER_E_MEM;

	//Control flags
	if (val >= 0 && val <= 0x1F){
		if (val == 0x0B){//Start flag
			currentCap->startFlag = TRUE;
			currentCap->posX = posX;
			currentCap->posY = posY;
		} else if (val == 0x0A) //End flag
			capEnd(cc);
		else{
			if (currentCap->startFlag && sbGetCharCount(currentCap->text) > 0){
				if (!sbAddChar(currentCap->text, L' '))
					return PARSER_E_MEM;

				if ((ret = capDuplicate(cc)) != PARSER_OK)
					return ret;
				else if (!qbPeek(cc->caps, 0, FALSE, &currentCap))
					return PARSER_E_MEM;
				
				currentCap->posX = posX;
				currentCap->posY = posY;
			}

			if (val >= 0 && val <= 0x7)
				currentCap->fgColour = val;
			else 
				switch(val){
					case 0x0C:{
						currentCap->sizeX = 0;
						currentCap->sizeY = 0;
					} break;
					case 0x0D: currentCap->sizeY = 1; break;
					case 0x0E: currentCap->sizeX = 1; break;
					case 0x0F:{
						currentCap->sizeX = 1;
						currentCap->sizeY = 1;
					} break;
					case 0x1C: currentCap->bgColour = BLACK; break;
					case 0x1D: currentCap->bgColour = currentCap->fgColour; break;
				}
		}
	} else if (currentCap->startFlag){
		wchar_t parsedVal;

		switch (val){
			case 0x23: case 0x24:
				parsedVal = subsets[0][val-0x23]; break;
			case 0x40:
				parsedVal = subsets[0][2]; break;
			case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
				parsedVal = subsets[0][val-0x58]; break;
			case 0x60:
				parsedVal = subsets[0][8]; break;
			case 0x7B: case 0x7C: case 0x7D: case 0x7E:
				parsedVal = subsets[0][val-0x74]; break;
			default:
				parsedVal = (wchar_t)val;
		}
		if (!sbAddChar(currentCap->text, parsedVal))
			return PARSER_E_MEM;
	}

	return PARSER_OK;
}

/////////////////////////////BEGIN HELPER ROUTINES/////////////////////////////

static int ccIsDuplicate(CaptionCluster *ccA, CaptionCluster *ccB){
	Caption *cA = NULL, *cB = NULL;
	int i = 0;

	//Both must be evaluated (NOT &&)
	while(qbPeek(ccA->caps, i, FALSE, &cA) & qbPeek(ccB->caps, i, FALSE, &cB)){
		if (wcscmp(sbGetString(cA->text), sbGetString(cB->text)))
			return FALSE;
		i++;
	}

	//One has more caps than the other.
	if (cA || cB)
		return FALSE;
	return TRUE;
}

static int capDuplicate(CaptionCluster *cc){
	int ret;
	Caption *previous, *current;

	if (!qbPeek(cc->caps, 0, FALSE, &previous))
		return PARSER_E_PARAMS;
	
	capEnd(cc);
	ret = capStart(cc);
	if (ret != PARSER_OK || !qbPeek(cc->caps, 0, FALSE, &current))
		return ret;
	
	current->bgColour = previous->bgColour;
	current->fgColour = previous->fgColour;
	current->sizeX = previous->sizeX;
	current->sizeY = previous->sizeY;
	current->startFlag = previous->startFlag;

	return PARSER_OK;
}


/////////////////////////////END HELPER ROUTINES/////////////////////////////