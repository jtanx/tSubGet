#include "tSubInternal.h"
static int parsePage(CaptionsParser *p, Sample smp);
static int fileExists(wchar_t *file);
static void getDefaultOutputFile(ParserOpts *po);

int tsgInit(CaptionsParser **p, ParserOpts *po){
	int ret;
	
	if (!p || !po || !fileExists(po->fileIn))
		return PARSER_E_PARAMS;
	else if (po->pageNumber > 0x7FF || !IsLangId(po->langId))
		return PARSER_E_PARAMS;
	
	//Determine the correct output filename and determine if it exists.
	if (po->fileOut[0] == '\0')
		getDefaultOutputFile(po);
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
		convertMsToRTime(cc->timeStart + p->po.delay, &timeStart);
		convertMsToRTime(cc->timeEnd + p->po.delay, &timeEnd);
		fwprintf(fp, L"%d\n", i+1);
		fwprintf(fp,L"%.2lld:%.2lld:%.2lld,%.3lld --> %.2lld:%.2lld:%.2lld,%.3lld\n",
				timeStart.h,timeStart.m,timeStart.s,timeStart.ms,
				timeEnd.h,timeEnd.m,timeEnd.s,timeEnd.ms);
		
		while (qbPeek(cc->caps, 0, TRUE, &cap) && cap){
			if (p->po.addColourTags){
				if (p->po.fmt.fgColour[cap->fgColour] >= 0)
					fwprintf(fp, L"<font color=\"#%06.6X\">", 
						p->po.fmt.fgColour[cap->fgColour]);
				else if (cap->fgColour != WHITE)
					fwprintf(fp, L"<font color=\"%s\">", 
						colourSet[cap->fgColour]);
			}

			fwprintf(fp, L"%s", sbGetString(cap->text));
			if (p->po.addColourTags){
				if (p->po.fmt.fgColour[cap->fgColour] >= 0 || cap->fgColour != WHITE)
					fwprintf(fp, L"</font>");
			}

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
	CaptionCluster *cc;
	if (!*p) return;

	readerClose(*p);
	while (qbPeek((*p)->cc, 0, TRUE, &cc)){
		Caption *c;
		
		while (qbPeek(cc->caps, 0, TRUE, &c)){
			sbFree(c->text);
			qbFreeSingle(cc->caps, TRUE);
		}
		qbClose(&cc->caps);
		qbFreeSingle((*p)->cc, TRUE);
	}
	qbClose(&(*p)->cc);

	CloseHandle((*p)->hAbort);
	
	free(*p);
	*p = NULL;
}

void tsgSignalAbort(CaptionsParser *p){
	if (p)
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
			ptr = L"A memory allocation routine failed"; break;
		case PARSER_E_COM:
			ptr = L"A COM error occurred"; break;
		case PARSER_E_ABORT:
			ptr = L"The parsing process was aborted"; break;
		default:
			ptr = L"An unknown error occurred";
	}

	wcsncpy_s(buf,bufSize,ptr,_TRUNCATE);
}

void tsgGetLangStr(LangID lang, wchar_t *buf, size_t bufSize){
	wchar_t *ptr;
	if (!buf) return;

	switch (lang){
		case LANGID_DEFAULT:
			ptr = L"(Default) Latin/English"; break;
		case LANGID_CZECHSLOVAK:
			ptr = L"Latin/Czech, Slovak"; break;
		case LANGID_ENGLISH:
			ptr = L"Latin/English"; break;
		case LANGID_ESTONIAN:
			ptr = L"Latin/Estonian"; break;
		case LANGID_FRENCH:
			ptr = L"Latin/French"; break;
		case LANGID_GERMAN:
			ptr = L"Latin/German"; break;
		case LANGID_ITALIAN:
			ptr = L"Latin/Italian"; break;
		case LANGID_LETTISHLITHUANIAN:
			ptr = L"Latin/Lettish, Lithuanian"; break;
		case LANGID_POLISH:
			ptr = L"Latin/Polish"; break;
		case LANGID_PORTUGUESESPANISH:
			ptr = L"Latin/Portuguese, Spanish"; break;
		case LANGID_ROMANIAN:
			ptr = L"Latin/Romanian"; break;
		case LANGID_SERBIANCROATIANSLOVENIAN:
			ptr = L"Latin/Serbian, Croatian, Slovenian"; break;
		case LANGID_SWEDISHFINNISHHUNGARIAN:
			ptr = L"Latin/Swedish, Finnish, Hungarian"; break;
		case LANGID_TURKISH:
			ptr = L"Latin/Turkish"; break;
		default:
			ptr=  L"Unknown (!!)"; break;
	}

	wcsncpy_s(buf, bufSize, ptr, _TRUNCATE);
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
static int parsePage(CaptionsParser *p, Sample smp){
	int i, j, ret;
	int bufPos = TT_PACKETSIZE;

	//NB: Third parameter determines if this page is an 'erase' page. ('Byte 9, bit 8')
	ccEnd(p->cc, smp.time, (fixHamm48[smp.pBuf[5]] & 1 << 3));
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

static int fileExists(wchar_t *file){
	FILE *fp;
	if (!_wfopen_s(&fp, file, L"r")){
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

static void getDefaultOutputFile(ParserOpts *po){
	wchar_t *ptr;

	ptr = wcsrchr(po->fileIn, L'\\');
	ptr = !ptr ? po->fileIn : ptr + 1;


	if (po->folderOut[0] != L'\0')
		_snwprintf_s(po->fileOut, MAX_PATH-EXT_SIZE, _TRUNCATE, L"%s\\%s", po->folderOut, ptr);
	else
		wcsncpy_s(po->fileOut, MAX_PATH-EXT_SIZE, po->fileIn, _TRUNCATE);
	ptr = wcsrchr(po->fileOut,L'.');
	if (ptr) *ptr = L'\0';
	wcsncat_s(po->fileOut,MAX_PATH,EXT,_TRUNCATE);
}

void convertMsToRTime(__int64 ms, RTime *rt){
	if (ms < 0) ms = 0;

	rt->h = ms/3600000;
	rt->m = (ms%3600000)/60000;
	rt->s = (ms%60000)/1000;
	rt->ms = ms%1000;
}
/////////////////////////////END HELPER ROUTINES/////////////////////////////