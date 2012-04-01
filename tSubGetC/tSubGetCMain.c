#include "tSubGetC.h"

static void printHelp(wchar_t *exec){
	wprintf (L"Usage:\n%s [-d msDelay] [-p pageNum] [-o fileOut] [-c] fileIn\n", exec);
	wprintf (L"\t[-d msDelay]\tDelay in ms to add to each caption. Can be "
			 L"positive\n\t\t\tor negative.\n");
	wprintf (L"\t[-p pageNum]\tSpecifies the teletext page on which to search for\n"
			 L"\t\t\tcaptions. Defaults to page 801 (Australian default)\n");
	wprintf (L"\t[-o fileOut]\tSpecifies the output file. Defaults to the"
			 L"\n\t\t\toriginal filename with the extension removed and"
			 L"\n\t\t\t'.srt' appended.\n");
	wprintf (L"\t[-c]\t\tAdds colour tags to the output, if present.\n");
	wprintf (L"\n");
}

static int parseArgs(ParserOpts *po, int *pos, int argc, wchar_t *argv[]){
	if (argv[*pos][0] == L'-'){
		switch (argv[*pos][1]){
			case L'c': po->addColourTags = TRUE; break;
			
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

			default: return FALSE;
		} //End switch
	} else {
		wcsncpy_s(po->fileIn,MAX_PATH,argv[*pos],_TRUNCATE);
	}

	return TRUE;
}

static void printArgs(ParserOpts *po){
	wprintf (L"Input file:\t\t\"%s\"\n",po->fileIn);
	wprintf (L"Output file:\t\t\"%s\"\n",po->fileOut[0] == L'\0' ? L"Default" : po->fileOut);
	wprintf (L"Add colour tags:\t%s\n", po->addColourTags ? L"Yes" : L"No");
	wprintf (L"Delay:\t\t\t%lld ms\n",po->delay);
	wprintf (L"Teletext page:\t\t%.3X\n",!(po->pageNumber >> 8) ? (po->pageNumber | 8 << 8) : po->pageNumber);
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

	wprintf (L"tSubGetC - Version: 0.8, Core version: %s (%s)\n\n",BUILD_VERSION, BUILD_DATE);
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
			wprintf (L"Error processing argument: %s\n",argv[i]);
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