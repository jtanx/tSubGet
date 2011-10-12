#include "tSubCaption.h"
//Helper functions
static int capDuplicate(CaptionCluster *cc, ArrayInfo *ccI);
static void capSingleReset(Caption *c);
static void ccSingleReset(CaptionCluster *cc);
static int ccIsDuplicate(CaptionCluster ccA, CaptionCluster ccB);

/////////////////////////////BEGIN MEM-ALLOC ROUTINES/////////////////////////////
static int capInit(Caption **c, ArrayInfo *capsI){
	int i;

	if (!c || !capsI)
		return PARSER_E_PARAMS;
	*c = calloc(CAPTION_BLOCK,sizeof(Caption));
	if (!*c)
		return PARSER_E_MEM;
	for (i = 0; i < CAPTION_BLOCK; i++){
		(*c)[i].text = calloc(CAPTION_TEXT_SIZE,sizeof(wchar_t));
		(*c)[i].textI.count = CAPTION_TEXT_SIZE;
		(*c)[i].fgColour = WHITE;

		if (!(*c)[i].text)
			return PARSER_E_MEM;
	}
	capsI->count = CAPTION_BLOCK;

	return PARSER_OK;
}

static int capRealloc(Caption **c, ArrayInfo *capsI){
	unsigned i;

	if (!c || !*c || !capsI)
		return PARSER_E_PARAMS;	
	capsI->count += CAPTION_BLOCK;
	*c = realloc(*c,sizeof(Caption)*capsI->count);
	if (!*c)
		return PARSER_E_MEM;

	for (i = capsI->count-CAPTION_BLOCK; i < capsI->count; i++){
		memset(&(*c)[i],0,sizeof(Caption));
		(*c)[i].text = calloc(CAPTION_TEXT_SIZE,sizeof(wchar_t));
		(*c)[i].textI.count = CAPTION_TEXT_SIZE;
		(*c)[i].fgColour = WHITE;

		if (!(*c)[i].text)
			return PARSER_E_MEM;
	}
	return PARSER_OK;
}

//Closes a group of captions within a cluster
static void capClose(Caption *c, ArrayInfo capsI){
	unsigned i;
	if (!c) return;

	for (i = 0; i < capsI.count; i++)
		if (c[i].text) free(c[i].text);
	free(c);
}

int ccInit(CaptionCluster **cc, ArrayInfo *ccI){
	int i, ret;

	if (!cc || !ccI)
		return PARSER_E_PARAMS;
	*cc = calloc(CLUSTER_BLOCK,sizeof(CaptionCluster));
	if (!*cc)
		return PARSER_E_MEM;

	for (i = 0; i < CLUSTER_BLOCK; i++){
		ret = capInit(&(*cc)[i].caps,&(*cc)[i].capsI);
		if (ret != PARSER_OK)
			return ret;
	}

	ccI->count = CLUSTER_BLOCK;
	return PARSER_OK;	
}

static int ccRealloc(CaptionCluster **cc, ArrayInfo *ccI){
	unsigned i, ret;

	if (!cc || !*cc || !ccI)
		return PARSER_E_PARAMS;
	ccI->count += CLUSTER_BLOCK;
	*cc = realloc(*cc,sizeof(CaptionCluster)*ccI->count);
	if (!*cc)
		return PARSER_E_MEM;
	
	for (i = ccI->count - CLUSTER_BLOCK; i < ccI->count; i++){
		memset(&(*cc)[i],0,sizeof(CaptionCluster));
		ret = capInit(&(*cc)[i].caps,&(*cc)[i].capsI);
		if (ret != PARSER_OK)
			return ret;
	}
	return PARSER_OK;
}

//Closes the whole cluster
void ccClose(CaptionCluster *cc, ArrayInfo ccI){
	unsigned i;
	for (i = 0; i < ccI.count; i++)
		if (cc[i].caps)
			capClose(cc[i].caps,cc[i].capsI);
	free(cc);
}
/////////////////////////////END MEM-ALLOC ROUTINES/////////////////////////////

/////////////////////////////BEGIN INIT ROUTINES/////////////////////////////
int ccStart(CaptionCluster **cc, ArrayInfo *ccI, __int64 timeStart){
	if (ccI->index == ccI->count){
		int ret = ccRealloc(cc,ccI);
		if (ret != PARSER_OK)
			return ret;
	}
	(*cc)[ccI->index].timeStart = timeStart/10000;

	return PARSER_OK;
}

void ccEnd(CaptionCluster *cc, ArrayInfo *ccI, __int64 timeEnd){
	capEnd(cc,ccI);

	if (cc[ccI->index].capsI.index == 0)
		return;
	else if (ccI->index > 0 && ccIsDuplicate(cc[ccI->index-1],cc[ccI->index]))
		ccSingleReset(&cc[ccI->index--]);
	
	timeEnd /= 10000;
	if (cc[ccI->index].timeStart > timeEnd)
		cc[ccI->index].timeStart = 0;
	cc[ccI->index++].timeEnd = timeEnd;
}

int capStart(CaptionCluster *cc, ArrayInfo *ccI){
	if (!cc[ccI->index].hasActiveCap){
		ArrayInfo *capsI = &cc[ccI->index].capsI;
		if (capsI->index == capsI->count){
			int ret = capRealloc(&cc[ccI->index].caps,capsI);
			if (ret != PARSER_OK)
				return ret;
		}
		cc[ccI->index].hasActiveCap = TRUE;
	}
	return PARSER_OK;
}

void capEnd(CaptionCluster *cc, ArrayInfo *ccI){
	if (cc[ccI->index].hasActiveCap){
		Caption *currentCap = &cc[ccI->index].caps[cc[ccI->index].capsI.index];
		Caption *previousCap = cc[ccI->index].capsI.index > 0 ? &cc[ccI->index].caps[cc[ccI->index].capsI.index-1] : NULL;
		if (currentCap->textI.index == 0)
			capSingleReset(currentCap);
		else{
			if (previousCap && currentCap->posY == previousCap->posY)
				previousCap->noBreak = TRUE;
			cc[ccI->index].capsI.index++;
		}

		currentCap->text[currentCap->textI.index] = L'\0';
		cc[ccI->index].hasActiveCap = FALSE;
	}
}
/////////////////////////////END INIT ROUTINES/////////////////////////////

/////////////////////////////BEGIN HELPER ROUTINES/////////////////////////////
static void capSingleReset(Caption *c){
	wchar_t *textptr = c->text;
	ArrayInfo textInfo = c->textI;
		
	memset(c,0,sizeof(Caption));
	c->text = textptr;
	c->textI.count = textInfo.count;
	c->fgColour = WHITE;
}

static void ccSingleReset(CaptionCluster *cc){
	unsigned i;

	for (i = 0; i < cc->capsI.index; i++)
		capSingleReset(&cc->caps[i]);

	cc->capsI.index = 0;
	cc->timeStart = 0;
	cc->timeEnd = 0;
}

static int ccIsDuplicate(CaptionCluster ccA, CaptionCluster ccB){
	int i;
	if (ccA.capsI.index != ccB.capsI.index)
		return FALSE;
	//If they don't match, it's more likely that non-matching text
	//is at the back of the array, not the front.
	for (i = (int)ccA.capsI.index-1; i >= 0; i--){
		if (wcscmp(ccA.caps[i].text,ccB.caps[i].text))
			return FALSE;
	}
	return TRUE;
}

/////////////////////////////END HELPER ROUTINES/////////////////////////////