#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shlobj.h>
#include <commctrl.h>
#include <process.h>
#include "tSubDefs.h"
#include "..\\tSubGetLib\\tSubGetLib.h"

#define GUI_BUILD_VERSION L"0.9.5"

typedef struct InterfaceOpts {
	ParserOpts po;
	int autosave;
	HWND hwndList;
} InterfaceOpts;

typedef struct StatusData {
	CaptionsParser *p;
	InterfaceOpts *io;
	DWORD timeLapsed;
	HWND hwndStatusDlg;
	HANDLE hWorkerThread;
} StatusData;

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OutputFormatDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK StatusDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ifInitLangSelection(HWND hwndLangList);
void ifDisplayPrefs(HWND hwndMain, InterfaceOpts *io);
void ifOnCommand(HWND hwnd, int id, HWND hwndCtl, InterfaceOpts *io);

int listAdd(HWND hwndList, wchar_t *fullPath);
int listDeleteSelected(HWND hwndList);
int listClear(HWND hwndList);
int listMoveSelected(HWND hwndList, int direction);
int listGetCount(HWND hwndList);
int listGetItem(HWND hwndList, int index, wchar_t **disp, wchar_t *full, size_t fBufSize);
int listLocateFirstSelected(HWND hwndList);

void prefLoad(InterfaceOpts *io);
int prefSave(InterfaceOpts *io);
int prefDelete();

