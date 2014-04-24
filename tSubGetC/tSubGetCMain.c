#include "tSubGetC.h"

#ifdef _DEBUG
int isStandalone() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE), &csbi)){
        printf( "GetConsoleScreenBufferInfo failed: %lu\n", GetLastError());
        return FALSE;
    }

    // if cursor position is (0,0) then we were launched in a separate console
    return ((!csbi.dwCursorPosition.X) && (!csbi.dwCursorPosition.Y));
}

static void noClose() {
	system("pause");
}
#endif

static void printHelp(wchar_t *exec){
	int i;
	wprintf (L"Usage:\n%s [-d delay] [-p page] [-o out] [-c] [-s col,cs] [-y] [-l langid] fileIn\n", exec);
	wprintf (L"\t[-d delay]\tDelay in ms to add to each caption. Can be "
			 L"positive\n\t\t\tor negative.\n");
	wprintf (L"\t[-p page]\tSpecifies the teletext page on which to search for\n"
			 L"\t\t\tcaptions. Defaults to page 801 (Australian default)\n");
	wprintf (L"\t[-o out]\tSpecifies the output file. Defaults to the"
			 L"\n\t\t\toriginal filename with the extension removed and"
			 L"\n\t\t\t'.srt' appended.\n");
	wprintf (L"\t[-c]\t\tAdds colour tags to the output, if present.\n");
	wprintf (L"\t[-s col,cs]\tOverrides the default colour used. The colour must be\n");
	wprintf (L"\t\t\tin hexadecimal format. Example: -s red,FF2211\n");
	wprintf (L"\n\t\t\tAvailable colours:\n\t\t\t");
	for (i = 0; i < NUM_COLOURS; i++) {
		wprintf (L"%s ", colourSet[i]);
	}
	wprintf(L"\n");
	wprintf (L"\t[-y]\t\tOverwrites the output without prompt.\n");
	wprintf (L"\t[-l langid]\tSelects the required language id. For a list of IDs,"
			 L"\n\t\t\tenter 'help' instead of an id.\n");
	wprintf (L"\n");
}

static int parseArgs(ParserOpts *po, int *pos, int argc, wchar_t *argv[]){
	if (argv[*pos][0] == L'-'){
		switch (argv[*pos][1]){
			case L'c': po->addColourTags = TRUE; break;

			case L'y': po->overwriteOutput = TRUE; break;
			
			case L's': {
				wchar_t *failPtr;
				wchar_t *val, *ptr;
				int i;
				if (*pos == argc - 1) return FALSE;
				
				val = argv[++(*pos)];
				if ((ptr = wcschr(val, L',')) == NULL) return FALSE;

				*ptr++ = L'\0';
				for (i = 0; i < NUM_COLOURS; i++) {
					if (!_wcsicmp(colourSet[i], val)) {
						if (*ptr) {
							po->fmt.fgColour[i] = wcstol(ptr, &failPtr, 16);
							po->fmt.fgColour[i] &= 0xFFFFFF; //No overflow
							if (*failPtr != L'\0') return FALSE;
						} else {
							po->fmt.fgColour[i] = -1; //Default colour
						}
						break;
					}
				}
				if (i == NUM_COLOURS) return FALSE;
			} break;

			case L'd':{
				wchar_t *failPtr;
				if (*pos == argc - 1) return FALSE;
				
				po->delay = wcstol(argv[++(*pos)],&failPtr,10);
				if (*failPtr != L'\0') return FALSE;
			} break;
			
			case L'p':{
				wchar_t *failPtr;
				if (*pos == argc - 1) return FALSE;
				
				po->pageNumber = wcstol(argv[++(*pos)],&failPtr,16) & 0xFFF;
				if (po->pageNumber  >> 8 >= 8)
					po->pageNumber &= 0xFF;
				if (*failPtr != L'\0') return FALSE;
			} break;

			case L'o':{
				if (*pos == argc - 1) return FALSE;
				wcsncpy_s(po->fileOut,MAX_PATH,argv[++(*pos)],_TRUNCATE);
			} break;

			case L'l':{
				wchar_t *failPtr;

				if (*pos == argc - 1) return FALSE;
				if (!_wcsicmp(argv[++(*pos)], L"help")){
					int i;
					wchar_t buf[BUFSIZ];

					wprintf (L"%-13s %s\n", L"Language ID", L"Language"); 
					for (i = LANGID_DEFAULT; i < LANGID_NULL; i++){
						tsgGetLangStr(i, buf, BUFSIZ);
						wprintf (L"%-13d %s\n", i, buf);
					}
					return FALSE;
				}

				po->langId = wcstol(argv[*pos], &failPtr, 10);
				if (*failPtr != L'\0') return FALSE;
				else if (!IsLangId(po->langId)) return FALSE;
			} break;

			default: return FALSE;
		} //End switch
	} else {
		wcsncpy_s(po->fileIn,MAX_PATH,argv[*pos],_TRUNCATE);
	}

	return TRUE;
}

static void printArgs(ParserOpts *po){
	wchar_t buf[BUFSIZ];

	tsgGetLangStr(po->langId, buf, BUFSIZ);
	wprintf (L"Input file:\t\t\t\"%s\"\n",po->fileIn);
	wprintf (L"Output file:\t\t\t\"%s\"\n",po->fileOut[0] == L'\0' ? L"Default" : po->fileOut);
	wprintf (L"Add colour tags:\t\t%s\n", po->addColourTags ? L"Yes" : L"No");
	if (po->addColourTags) {
		int i;
		for (i = 0; i < NUM_COLOURS; i++) {
			if (po->fmt.fgColour[i] >= 0) {
				wprintf(L"\t\t\t\t'%s' -> %06X\n", colourSet[i], po->fmt.fgColour[i]);
			}
		}
	}
	wprintf (L"Overwrite without prompt:\t%s\n", po->overwriteOutput ? L"Yes": L"No");
	wprintf (L"Delay:\t\t\t\t%lld ms\n",po->delay);
	wprintf (L"Teletext page:\t\t\t%.3X\n",!(po->pageNumber >> 8) ? (po->pageNumber | 8 << 8) : po->pageNumber);
	wprintf (L"Language:\t\t\t%s\n", buf);
	wprintf (L"\n");
}

int WINAPI workerThread(void *args){
	CaptionsParser *p = (CaptionsParser*) args;
	int ret;

	CoInitialize(NULL);
	ret = tsgProcess(p);
	if (ret == PARSER_OK)
		ret = tsgWriteout(p);
	CoUninitialize();

	return ret;
}

int wmain(int argc, wchar_t *argv[]){
	int i, ret;
	wchar_t textBuf[BUFSIZ];
	HANDLE hThread, hOutput;
	long workerExitCode, timeStart;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	ParserOpts po = {0};
	CaptionsParser *p;

#ifdef _DEBUG
	if (isStandalone()) {
		atexit(noClose);
	}
	wprintf(L"!!!!DEBUG VERSION!!!!\n");
#endif

	//Default colours
	memset(&po.fmt, -1, sizeof(OutputFormatting));

	wprintf (L"tSubGetC - Version: 0.9b, Core version: %s (%s)\n\n",BUILD_VERSION, BUILD_DATE);
	if (argc < 2){
		printHelp(argv[0]);
		return 1;
	}

	CoInitialize(NULL);
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!hOutput){
		wprintf (L"Error: Could not get the standard output handle!\n");
		return 1;
	}
	
	for (i = 1; i < argc; i++){
		if (!parseArgs(&po, &i, argc, argv)){
			wprintf (L"Stopped processing at argument: %s\n",argv[i]);
			wprintf (L"Check the syntax, and ensure you have supplied valid parameters.\n");
			return 1;
		}
	}

	if (po.fileIn[0] == L'\0'){
		wprintf (L"Error: No input file specified!\n");
		return 1;
	} else if (po.pageNumber == 0)
		po.pageNumber = 1;

	wprintf (L"Parsing file with the following options:\n");
	printArgs(&po);
	
	wprintf (L"Initialising the parser... ");
	do{
		ret = tsgInit(&p, &po);

		if (ret != PARSER_OK){
			if (ret == PARSER_E_OUT_EXISTS){
				wchar_t res;
				wprintf (L"\nWarning: \"%s\" exists. Overwrite? Y/N [N]: ",po.fileOut);
				res = towlower(getwchar());
				if (res == L'y')
					po.overwriteOutput = TRUE;
				else return 1;
			} else{
				tsgGetError(ret,textBuf,BUFSIZ);
				wprintf (L"Error: parserInit returned the following:\n");
				wprintf (L"%s\n",textBuf);
				return 1;
			}
		}
	} while (ret != PARSER_OK);
	wprintf (L"Done!\n");
	
	wprintf (L"Parsing the file... ");
	hThread = (HANDLE)_beginthreadex(NULL,0,workerThread,(void*)p,0,NULL);
	if (!hThread){
		wprintf (L"\nError: Could not start the worker process!\n");
		return 1;
	}

	GetConsoleScreenBufferInfo(hOutput,&csbi);
	timeStart = GetTickCount();
	do{
		GetExitCodeThread(hThread,&workerExitCode);
		if (workerExitCode != STILL_ACTIVE) break;
		
		wprintf (L"%3d%%, %ds",tsgGetProgress(p),(GetTickCount()-timeStart)/1000);
		SetConsoleCursorPosition(hOutput,csbi.dwCursorPosition);
		Sleep(100);
	} while (workerExitCode == STILL_ACTIVE);
	
	tsgClose(&p);

	if (workerExitCode != PARSER_OK){
		tsgGetError(workerExitCode,textBuf,BUFSIZ);
		wprintf (L"Error: The parser returned the following:\n");
		wprintf (L"%s\n",textBuf);
		return 1;
	}
	wprintf (L"Done!\n");

	CoUninitialize();
	return 0;
}