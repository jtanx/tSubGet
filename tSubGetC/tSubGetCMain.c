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

static int parseArgs(CaptionsParser *p, int *pos, int argc, wchar_t *argv[]){
	if (argv[*pos][0] == L'-'){
		switch (argv[*pos][1]){
			case L'c': p->colouredOutput = TRUE; break;
			
			case L'd':{
				wchar_t *failPtr;
				if (*pos == argc - 1) return FALSE;
				
				p->delay = wcstol(argv[++(*pos)],&failPtr,10);
				if (*failPtr != L'\0') return FALSE;
			} break;
			
			case L'p':{
				wchar_t *failPtr;
				if (*pos == argc - 1) return FALSE;
				
				p->pageNum = wcstol(argv[++(*pos)],&failPtr,16) & 0xFFF;
				if (p->pageNum  >> 8 >= 8)
					p->pageNum &= 0xFF;
				if (*failPtr != L'\0') return FALSE;
			} break;

			case L'o':{
				if (*pos == argc - 1) return FALSE;
				wcsncpy_s(p->fileOut,MAX_PATH,argv[++(*pos)],_TRUNCATE);
			} break;

			default: return FALSE;
		} //End switch
	} else {
		wcsncpy_s(p->fileIn,MAX_PATH,argv[*pos],_TRUNCATE);
	}

	return TRUE;
}

static void printArgs(CaptionsParser *p){
	wprintf (L"Input file:\t\t\"%s\"\n",p->fileIn);
	wprintf (L"Output file:\t\t\"%s\"\n",p->fileOut[0] == L'\0' ? L"Default" : p->fileOut);
	wprintf (L"Add colour tags:\t%s\n", p->colouredOutput ? L"Yes" : L"No");
	wprintf (L"Delay:\t\t\t%lld ms\n",p->delay);
	wprintf (L"Teletext page:\t\t%.3X\n",!(p->pageNum >> 8) ? (p->pageNum | 8 << 8) : p->pageNum);
	wprintf (L"\n");
}

int WINAPI workerThread(void *args){
	CaptionsParser *p = (CaptionsParser*) args;
	int ret;

	CoInitialize(NULL);
	ret = parserInit(p);
	if (ret != PARSER_OK) return ret;
	ret = parserReadFile(p);
	if (ret != PARSER_OK) return ret;
	ret = parserWriteout(p);
	if (ret != PARSER_OK) return ret;
	parserClose(p);
	CoUninitialize();

	return ret;
}

int wmain(int argc, wchar_t *argv[]){
	int i, ret;
	wchar_t textBuf[BUFSIZ];
	HANDLE hThread, hOutput;
	long workerExitCode, timeStart;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CaptionsParser p = {0};

	wprintf (L"tSubGetC - Version: 0.2a, Core version: %s, build %d\n\n",BUILD_VERSION, BUILD_COUNT);
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
		if (!parseArgs(&p,&i,argc,argv)){
			wprintf (L"Error processing argument: %s\n",argv[i]);
			wprintf (L"Check the syntax, and ensure you have supplied valid parameters.\n");
			return 1;
		}
	}

	if (p.fileIn[0] == L'\0'){
		wprintf (L"Error: No input file specified!\n");
		return 1;
	} else if (p.pageNum == 0)
		p.pageNum = 1;

	wprintf (L"Parsing file with the following options:\n");
	printArgs(&p);
	
	wprintf (L"Initialising the parser... ");
	do{
		ret = parserInit(&p);

		if (ret != PARSER_OK){
			if (ret == PARSER_W_OUT_EXISTS){
				wchar_t res;
				wprintf (L"\nWarning: \"%s\" exists. Overwrite? Y/N [N]: ",p.fileOut);
				res = towlower(getwchar());
				if (res == L'y')
					p.overwriteOutput = TRUE;
				else return 1;
			} else{
				parserGetError(ret,textBuf,BUFSIZ);
				wprintf (L"Error: parserInit returned the following:\n");
				wprintf (L"%s\n",textBuf);
				return 1;
			}
		}
	} while (ret != PARSER_OK);
	wprintf (L"Done!\n");
	
	wprintf (L"Parsing the file... ");
	hThread = (HANDLE)_beginthreadex(NULL,0,workerThread,(void*)&p,0,NULL);
	if (!hThread){
		wprintf (L"\nError: Could not start the worker process!\n");
		return 1;
	}

	GetConsoleScreenBufferInfo(hOutput,&csbi);
	timeStart = GetTickCount();
	do{
		GetExitCodeThread(hThread,&workerExitCode);
		if (workerExitCode != STILL_ACTIVE) break;
		
		wprintf (L"%3d%%, %ds",parserGetProgress(&p),(GetTickCount()-timeStart)/1000);
		SetConsoleCursorPosition(hOutput,csbi.dwCursorPosition);
		Sleep(100);
	} while (workerExitCode == STILL_ACTIVE);
	
	if (workerExitCode != PARSER_OK){
		parserGetError(workerExitCode,textBuf,BUFSIZ);
		wprintf (L"Error: The parser returned the following:\n");
		wprintf (L"%s\n",textBuf);
		return 1;
	}
	wprintf (L"Done!\n");

	CoUninitialize();
	return 0;
}