#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shlobj.h>
#include <commctrl.h>
#include "tSubDefs.h"
#include "..\\tSubGetLib\\tSubGetLib.h"

typedef struct InterfaceOpts {
	ParserOpts po;
	int autosave;
} InterfaceOpts;

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int listAdd(HWND hwndList, wchar_t *fullPath);
int listDeleteSelected(HWND hwndList);
int listClear(HWND hwndList);
int listMoveSelected(HWND hwndList, int direction);

void prefLoad(InterfaceOpts *io);
int prefSave(InterfaceOpts *io);
int prefDelete();

