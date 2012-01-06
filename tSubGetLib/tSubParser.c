#include "tSubInternal.h"

static int fileExists(wchar_t *file);
static int parsePage(CaptionsParser *p, Sample smp);
static void getDefaultOutputFile(wchar_t *inBuf, wchar_t *outBuf, int bufSize);
static void convertMsToRTime(__int64 ms, RTime *rt);
static const wchar_t *colours[8] = {L"black",L"red",L"green",L"yellow",
									L"blue",L"magenta",L"cyan",L"white"};

int tsgInit(CaptionsParser **p, ParserOpts *po){
	int ret;
	
	if (!p || !po || !fileExists(po->fileIn))
		return PARSER_E_PARAMS;
	else if (po->pageNumber > 0x7FF || po->langId != LANGID_DEFAULT)
		return PARSER_E_PARAMS;
	
	//Determine the correct output filename and determine if it exists.
	if (po->fileOut[0] == '\0')
		getDefaultOutputFile(po->fileIn,po->fileOut,MAX_PATH);
	if (!po->overwriteOutput && fileExists(po->fileOut))
		return PARSER_E_OUT_EXISTS;
	
	//Allocate buffer for CaptionsParser, since everything seems ok.
	*p = calloc(1, sizeof(CaptionsParser));
	if (!p)
		return PARSER_E_MEM;
	(*p)->po = *po;

	//Check that the input will 'work' or can be parsed
	ret = readerInit(*p);
	if (ret != PARSER_OK){
		tsgClose(p);
		return ret;
	}
	
	//Create the event signals
	(*p)->hAbort = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (!(*p)->hAbort){
		tsgClose(p);
		return PARSER_E_COM;
	}

	//Initialise the caption-cluster structure
	(*p)->cc = qbCreate(sizeof(CaptionCluster));
	if (!(*p)->cc){
		tsgClose(p);
		return PARSER_E_MEM;
	}

	return PARSER_OK;
}

int tsgWriteout(CaptionsParser *p){
	FILE *fp;
	unsigned i;
	CaptionCluster *cc;
	Caption *cap;
	RTime timeStart, timeEnd;

	if (!p) 
		return PARSER_E_PARAMS;
	else  if (!qbPeek(p->cc, 0, TRUE, &cc) || !cc)
		return PARSER_E_NOCAPS;
	else if (_wfopen_s(&fp, p->po.fileOut, L"w"))
		return PARSER_E_OUT_DENIED;

	for (i = 0; cc; i++){
		convertMsToRTime(cc->timeStart, &timeStart);
		convertMsToRTime(cc->timeEnd, &timeEnd);
		fwprintf(fp, L"%d\n", i+1);
		fwprintf(fp,L"%.2lld:%.2lld:%.2lld,%.3lld --> %.2lld:%.2lld:%.2lld,%.3lld\n",
				timeStart.h,timeStart.m,timeStart.s,timeStart.ms,
				timeEnd.h,timeEnd.m,timeEnd.s,timeEnd.ms);
		
		while (qbPeek(cc->caps, 0, TRUE, &cap) && cap){
			if (p->po.addColourTags && cap->fgColour != WHITE)
				fwprintf(fp, L"<font color=\"%s\">", colours[cap->fgColour]);
			fwprintf(fp, L"%s", sbGetString(cap->text));
			if (p->po.addColourTags && cap->fgColour != WHITE)
				fwprintf(fp, L"</font>");
			if (!cap->noBreak || !qbPeek(cc->caps, 0, TRUE, NULL))
				fwprintf(fp, L"\n");
			
			sbFree(cap->text);
			if (!qbFreeSingle(cc->caps, TRUE))
				return PARSER_E_MEM;
		}
		fwprintf (fp, L"\n");
		qbClose(&cc->caps);
		
		if (!qbFreeSingle(p->cc, TRUE))
			return PARSER_E_MEM;
		qbPeek(p->cc, 0, TRUE, &cc);
	}

	fclose(fp);
	return PARSER_OK;
}

void tsgClose(CaptionsParser **p){
	if (*p){
		CaptionCluster *cc;

		readerClose(*p);
		while (qbPeek((*p)->cc, 0, TRUE, &cc)){
			Caption *c;
			if (!cc) break;
			
			while (qbPeek(cc->caps, 0, TRUE, &c)){
				if (!c) break;
				sbFree(c->text);
				qbFreeSingle(cc->caps, TRUE);
			}
			qbClose(&cc->caps);
			qbFreeSingle((*p)->cc, TRUE);
		}
		qbClose(&(*p)->cc);

		CloseHandle((*p)->hAbort);
	}
	free(*p);
	*p = NULL;
}

void tsgSignalAbort(CaptionsParser *p){
	SetEvent(p->hAbort);
}

void tsgGetError(int errCode, wchar_t *buf, int bufSize){
	wchar_t *ptr;

	if (!buf) return;
	switch (errCode){
		case PARSER_OK:
			ptr = L"No error"; break;
		case PARSER_E_NOCAPS:
			ptr = L"No subtitles detected"; break;
		case PARSER_E_OUT_EXISTS:
			ptr = L"The output file exists"; break;
		case PARSER_E_PARAMS:
			ptr = L"An invalid parameter was passed"; break;
		case PARSER_E_FNF_IN:
			ptr = L"The specified input file was not found"; break;
		case PARSER_E_IN:
			ptr = L"The input file could not be processed"; break;
		case PARSER_E_OUT_DENIED:
			ptr = L"The output file could not be opened for writing"; break;
		case PARSER_E_MEM:
			ptr = L"One of the memory allocation routines failed"; break;
		case PARSER_E_COM:
			ptr = L"A COM error occurred"; break;
		case PARSER_E_ABORT:
			ptr = L"The parsing process was aborted"; break;
		default:
			ptr = L"An unknown error occurred";
	}

	wcsncpy_s(buf,bufSize,ptr,_TRUNCATE);
}

int parseSample(CaptionsParser *p, Sample smp){
	int i, ret;
	unsigned char rawHeader[10];
	unsigned packetNum, curPage;

	if (smp.bufSize < TT_PAGESIZE)
		return PARSER_OK; //Not fatal.
	for (i = 0; i < 10; i++){
		rawHeader[i] = fixHamm48[smp.pBuf[i]];
		if (rawHeader[i] & 0x80) //Erroneous packet
			return PARSER_OK;
	}
	
	packetNum = rawHeader[1] | rawHeader[0] >> 3;
	curPage = ((rawHeader[0] & 0x7) << 8)|(rawHeader[3] << 4)|(rawHeader[2]);
	if (packetNum != 0 || curPage != p->po.pageNumber)
		return PARSER_OK;

	ret = parsePage(p,smp);
	return ret;
}

/////////////////////////////BEGIN HELPER ROUTINES/////////////////////////////

static int fileExists(wchar_t *file){
	FILE *fp;
	if (!_wfopen_s(&fp, file, L"r")){
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

static int parsePage(CaptionsParser *p, Sample smp){
	int i, j, ret;
	int bufPos = TT_PACKETSIZE;

	ccEnd(p->cc, smp.time);
	ccStart(p->cc, smp.time);
	
	for (i = 0; i < TT_PACKETSPP - 1; i++, bufPos += TT_PACKETSIZE){
		unsigned char mag = fixHamm48[smp.pBuf[bufPos]] & 0x7;
		if (mag & 0x80 || mag != p->po.pageNumber >> 8) continue;
		
		for (j = 2; j < TT_PACKETSIZE; j++){
			unsigned char val = fixParity[smp.pBuf[bufPos+j]];
			if (val & 0x80) continue;

			ret = capAdd(p, j, i, val);
			if (ret != PARSER_OK)
				return ret;
		}
	}
	return PARSER_OK;
}

static void getDefaultOutputFile(wchar_t *inBuf, wchar_t *outBuf, int bufSize){
	wchar_t *ptr;

	wcsncpy_s(outBuf,bufSize-EXT_SIZE,inBuf,_TRUNCATE);
	ptr = wcsrchr(outBuf,L'.');
	if (ptr) *ptr = L'\0';
	wcsncat_s(outBuf,bufSize,EXT,_TRUNCATE);
}

static void convertMsToRTime(__int64 ms, RTime *rt){
	if (ms < 0) ms = 0;
	rt->h = ms/3600000;
	rt->m = (ms%3600000)/60000;
	rt->s = (ms%60000)/1000;
	rt->ms = ms%1000;
}
/////////////////////////////END HELPER ROUTINES/////////////////////////////