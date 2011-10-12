#include "tSubCaption.h"
static int capDuplicate(CaptionCluster *cc, ArrayInfo *ccI);
static int appendSpace(CaptionCluster *cc, ArrayInfo *ccI);
static void convertMsToRTime(__int64 ms, RTime *rt);
static const wchar_t *colours[8] = {L"black",L"red",L"green",L"yellow",
									L"blue",L"magenta",L"cyan",L"white"};

int capAdd(CaptionCluster *cc, ArrayInfo *ccI, unsigned posX, unsigned posY, unsigned lang, unsigned char val){
	int ret;
	Caption *currentCap;

	if (!cc || !ccI || ccI->index == ccI->count)
		return PARSER_E_PARAMS;
	ret = capStart(cc,ccI); 
	if (ret != PARSER_OK)
		return ret;

	currentCap = &cc[ccI->index].caps[cc[ccI->index].capsI.index];
	if (val >= 0 && val <= 0x1F){
		if (val == 0x0B)//Start flag
			currentCap->startFlag = TRUE;
		else if (val == 0x0A) //End flag
			capEnd(cc,ccI);
		else{
			if (currentCap->startFlag && currentCap->textI.index > 0){
				ret = appendSpace(cc,ccI);
				if (ret != PARSER_OK)
					return ret;

				capEnd(cc,ccI);
				ret = capDuplicate(cc,ccI);
				if (ret != PARSER_OK)
					return ret;
				currentCap = &cc[ccI->index].caps[cc[ccI->index].capsI.index];
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
		currentCap->posX = posX;
		currentCap->posY = posY;
	} else if (currentCap->startFlag){
		wchar_t parsedVal;

		if (currentCap->textI.index == currentCap->textI.count-1){
			currentCap->textI.count += CAPTION_TEXT_SIZE;
			currentCap->text = realloc(currentCap->text,sizeof(wchar_t)*currentCap->textI.count);
			if (!currentCap->text)
				return PARSER_E_MEM;
		}

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
		currentCap->text[currentCap->textI.index++] = parsedVal;
	}

	return PARSER_OK;
}

int ccSingleWriteOut(CaptionCluster *cc, unsigned index, unsigned colouredOutput, __int64 delay, FILE *fp){
	RTime timeStart, timeEnd;
	unsigned i;
	if (!cc || !fp)
		return PARSER_E_PARAMS;
	
	convertMsToRTime(cc[index].timeStart+delay,&timeStart);
	convertMsToRTime(cc[index].timeEnd+delay,&timeEnd);
	
	fwprintf(fp,L"%d\n",index+1);
	fwprintf(fp,L"%.2lld:%.2lld:%.2lld,%.3lld --> %.2lld:%.2lld:%.2lld,%.3lld\n",
				timeStart.h,timeStart.m,timeStart.s,timeStart.ms,
				timeEnd.h,timeEnd.m,timeEnd.s,timeEnd.ms);
	for (i = 0; i < cc[index].capsI.index; i++){
		if (colouredOutput && cc[index].caps[i].fgColour != WHITE)
			fwprintf(fp,L"<font color=\"%s\">",colours[cc[index].caps[i].fgColour]);
		fwprintf(fp,L"%s",cc[index].caps[i].text);
		if (colouredOutput && cc[index].caps[i].fgColour != WHITE)
			fwprintf(fp,L"</font>");
		if (!cc[index].caps[i].noBreak || i == cc[index].capsI.index-1)
			fwprintf(fp,L"\n");
	}
	fwprintf(fp,L"\n");

	return PARSER_OK;
}

/////////////////////////////BEGIN HELPER ROUTINES/////////////////////////////
static int capDuplicate(CaptionCluster *cc, ArrayInfo *ccI){
	int ret;
	Caption *previous, *current;

	if (cc[ccI->index].hasActiveCap || cc[ccI->index].capsI.index <= 0)
		return PARSER_E_PARAMS;
	ret = capStart(cc,ccI);
	if (ret != PARSER_OK)
		return ret;
	previous = &cc[ccI->index].caps[cc[ccI->index].capsI.index-1];
	current = &cc[ccI->index].caps[cc[ccI->index].capsI.index];
	
	current->bgColour = previous->bgColour;
	current->fgColour = previous->fgColour;
	current->sizeX = previous->sizeX;
	current->sizeY = previous->sizeY;
	current->startFlag = previous->startFlag;

	return PARSER_OK;
}

static int appendSpace(CaptionCluster *cc, ArrayInfo *ccI){
	Caption *currentCap = &cc[ccI->index].caps[cc[ccI->index].capsI.index];
	
	if (currentCap->textI.index > 0){
		if (currentCap->text[currentCap->textI.index-1] != L' '){
			if (currentCap->textI.index == currentCap->textI.count){
				currentCap->textI.count += CAPTION_TEXT_SIZE;
				currentCap->text = realloc(currentCap->text,sizeof(wchar_t)*currentCap->textI.count);
				if (!currentCap->text)
					return PARSER_E_MEM;
			}
			currentCap->text[currentCap->textI.index++] = L' ';
		}
	}
	return PARSER_OK;
}

static void convertMsToRTime(__int64 ms, RTime *rt){
	if (ms < 0) ms = 0;
	rt->h = ms/3600000;
	rt->m = (ms%3600000)/60000;
	rt->s = (ms%60000)/1000;
	rt->ms = ms%1000;
}
/////////////////////////////END HELPER ROUTINES/////////////////////////////