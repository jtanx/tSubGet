#include "tSubParser.h"
static int parsePage(CaptionsParser *p, Sample smp);
static void getDefaultOutputFile(wchar_t *inBuf, wchar_t *outBuf, int bufSize);

int parserInit(CaptionsParser *p){
	FILE *fp;
	int ret;
	
	if (!p)
		return PARSER_E_PARAMS;

	//Check the input file exists
	if (_wfopen_s(&fp,p->fileIn,L"r"))
		return PARSER_E_FNF_IN;
	fclose(fp);

	//Check the output file doesn't exist
	if (p->fileOut[0] == '\0')
		getDefaultOutputFile(p->fileIn,p->fileOut,MAX_PATH);
	if (!p->overwriteOutput && !_wfopen_s(&fp,p->fileOut,L"r")){
		fclose(fp);
		return PARSER_W_OUT_EXISTS;
	}

	//Check that the input will 'work' or can be parsed
	ret = readerInit(p);
	if (ret != PARSER_OK)
		return ret;
	
	//Create the event signals
	p->hAbort = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (!p->hAbort)
		return PARSER_E_COM;

	//Initialise the caption-cluster structure
	memset(&p->ccI,0,sizeof(ArrayInfo));
	ret = ccInit(&p->cc,&p->ccI);
	if (ret != PARSER_OK)
		parserClose(p);
	return ret;
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
	if (packetNum != 0) //Currently only support Pkt/0 types
		return PARSER_OK;
	curPage = ((rawHeader[0] & 0x7) << 8)|(rawHeader[3] << 4)|(rawHeader[2]);
	if (curPage != p->pageNum)
		return PARSER_OK;
	ret = parsePage(p,smp);
	
	return ret;
}

int parserWriteout(CaptionsParser *p){
	unsigned i, ret;
	if (!p) return PARSER_E_PARAMS;
	else if (p->ccI.index == 0) return PARSER_W_NOCAPS;

	if (_wfopen_s(&p->fpOut,p->fileOut,L"w"))
		return PARSER_E_OUT_DENIED;	

	for (i = 0; i < p->ccI.index; i++){
		ret = ccSingleWriteOut(p->cc,i,p->colouredOutput,p->delay,p->fpOut);
		if (ret != PARSER_OK)
			return ret;
	}
	return PARSER_OK;
}

void parserClose(CaptionsParser *p){
	if (p){
		if (p->fpOut){
			fclose(p->fpOut);
			p->fpOut = NULL;
		}
		readerClose(p);
		ccClose(p->cc,p->ccI);
		CloseHandle(p->hAbort);
	}
}

void parserSignalAbort(CaptionsParser *p){
	SetEvent(p->hAbort);
}

void parserGetError(int errCode, wchar_t *buf, int bufSize){
	wchar_t *ptr;

	if (!buf) return;
	switch (errCode){
		case PARSER_OK:
			ptr = L"No error"; break;
		case PARSER_W_NOCAPS:
			ptr = L"No subtitles detected"; break;
		case PARSER_W_OUT_EXISTS:
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
/////////////////////////////BEGIN HELPER ROUTINES/////////////////////////////
static int parsePage(CaptionsParser *p, Sample smp){
	int i, j, ret;
	int bufPos = TT_PACKETSIZE;

	ccEnd(p->cc,&p->ccI,smp.time);
	ccStart(&p->cc,&p->ccI,smp.time);
	for (i = 0; i < TT_PACKETSPP - 1; i++, bufPos += TT_PACKETSIZE){
		unsigned char mag = fixHamm48[smp.pBuf[bufPos]] & 0x7;
		if (mag & 0x80 || mag != p->pageNum >> 8) continue;
		
		for (j = 2; j < TT_PACKETSIZE; j++){
			unsigned char val = fixParity[smp.pBuf[bufPos+j]];
			if (val & 0x80) continue;

			ret = capAdd(p->cc,&p->ccI,j,i,0,val);
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
/////////////////////////////END HELPER ROUTINES/////////////////////////////