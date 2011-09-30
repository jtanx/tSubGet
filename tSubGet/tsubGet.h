#include <windows.h>
#include <Shlobj.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <shlwapi.h>
#include "tSubdefs.h"
#include "tsubIO.h"

struct _GUI {
	HANDLE hEvent;
	HANDLE hThread;
	HANDLE hDrop;
	HWND hwndMain;
	HWND hwndProgress;
	HWND hwndStatus;
};

struct _ParamsIn {
	unsigned forceOutputDir;
	unsigned colouredOutput;
	unsigned noPrompt;
	unsigned ttPageNumber;
	unsigned lang[2];
	wchar_t outDir[MAX_PATH];
	unsigned fileCount;
	wchar_t filePath[MAX_PATH];
};

struct _ParamsOut {
	wchar_t outFile[MAX_PATH];
	wchar_t dispFilename[MAX_PATH];
	Decoder d;
	DShowGraph g;
};

//GUI stuff
typedef struct _Details {
	struct _GUI gui;
	struct _ParamsIn pi;
	struct _ParamsOut po;
} Details;

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int parseFile(Details *uDets);
int writeOutSubs(Details *uDets);
unsigned WINAPI processInput(void *args);