#include "tSubGet.h"

static int initMainDlg(Details *uDets);
static void browseFolders(Details *uDets);
static void getPrefs(Details *uDets);
static void toggleControls(Details uDets, BOOL bEnable);
static void cleanupProcessing(Details *uDets);

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static Details uDets = {0};

	switch (msg){
		case WM_INITDIALOG:{
			uDets.pi.ttPageNumber = 1; //Actually 801.
			uDets.gui.hwndMain = hwnd;
			if (!initMainDlg(&uDets)){
				MessageBox(hwnd,L"Error: Failed to initialise main window!",
							NULL, MB_ICONEXCLAMATION);
				SendMessage(hwnd,WM_COMMAND,ID_MAINCONTROL,0);
			}
		} break;

		case WM_DROPFILES:{
			if (!uDets.gui.hThread){
				uDets.gui.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
				toggleControls(uDets,FALSE);
				getPrefs(&uDets);
				uDets.gui.hDrop = (HDROP)wParam;
				uDets.gui.hThread = (HANDLE)_beginthreadex(NULL,0,processInput,
															(void*)&uDets,0,NULL);
				if (!uDets.gui.hThread){ //hnnnnnnnnnn
					MessageBox(hwnd,L"Error: Could not spawn worker process.",NULL,MB_ICONEXCLAMATION);
					cleanupProcessing(&uDets);
				}
			} else{
				MessageBox(hwnd,L"tSubGet is currently processing files, please wait until this is complete.",NULL,MB_ICONINFORMATION);
				DragFinish((HDROP)wParam);
			}
		} break;

		case WM_COMMAND:{
			switch (LOWORD(wParam)){
				case IDOK: case IDCANCEL:{
					if (uDets.gui.hThread){
						int ret;
						ret = MessageBox(hwnd,L"Warning: tSubGet is still processing files. \n"
											  L"To force-quit, click ok, or try cancelling and aborting first.",
											  L"Warning",MB_OKCANCEL|MB_ICONQUESTION);
						if (ret == IDOK){
							TerminateThread(uDets.gui.hThread,2);
							cleanupProcessing(&uDets);
						}
						else break;
					}
					SendMessage(hwnd,WM_COMMAND,ID_MAINCONTROL,0);
				} break;

				case ID_MAINCONTROL:{
					if (uDets.gui.hThread)
						SetEvent(uDets.gui.hEvent); //Abort event
					else
						EndDialog(hwnd,IDOK);
				} break;

				case ID_FORCEDIR:{
					uDets.pi.forceOutputDir = !IsDlgButtonChecked(hwnd,ID_FORCEDIR);
					CheckDlgButton(hwnd,ID_FORCEDIR,uDets.pi.forceOutputDir);
					EnableWindow(GetDlgItem(hwnd,ID_BROWSE),uDets.pi.forceOutputDir);
				} break;

				case ID_ADDCOLOUR:{
					uDets.pi.colouredOutput = !IsDlgButtonChecked(hwnd,ID_ADDCOLOUR);
					CheckDlgButton(hwnd,ID_ADDCOLOUR,uDets.pi.colouredOutput);
				} break;

				case ID_NOPROMPT:{
					uDets.pi.noPrompt = !IsDlgButtonChecked(hwnd,ID_NOPROMPT);
					CheckDlgButton(hwnd,ID_NOPROMPT,uDets.pi.noPrompt);
				} break;

				case ID_BROWSE:{
					browseFolders(&uDets);
				} break;

				case ID_TOP:{
					int ret = GetMenuState(GetMenu(hwnd),ID_TOP,MF_BYCOMMAND);
					HWND setTopmost = (ret == MF_CHECKED) ? HWND_NOTOPMOST: HWND_TOPMOST;
					ret = (ret == MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED;

					CheckMenuItem(GetMenu(hwnd),ID_TOP,ret);
					SetWindowPos(hwnd,setTopmost,1,1,1,1,SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
				} break;

				case ID_FINISH:{
					DWORD dwExit;
					do{
						Sleep(200);
						GetExitCodeThread(uDets.gui.hThread,&dwExit);
					}while(dwExit == STILL_ACTIVE);
					if (dwExit)
						SetWindowText(uDets.gui.hwndStatus,L"Processing finished.");
					cleanupProcessing(&uDets);
				} break;
			} 
		} break; //End WM_COMMAND

		default:{
			return FALSE;
		} break;
	}
	return TRUE;
}

static int initMainDlg(Details *uDets){
	uDets->gui.hwndProgress = CreateWindow(PROGRESS_CLASS,
										NULL,WS_CHILD,
										10,156,185,20,uDets->gui.hwndMain,NULL,
										GetModuleHandle(NULL),NULL);
	uDets->gui.hwndStatus = CreateWindow(STATUSCLASSNAME,
									L"Drag items into the window to process.",
									WS_CHILD,0,0,0,0,uDets->gui.hwndMain,NULL,
									GetModuleHandle(NULL),NULL);
	if (!uDets->gui.hwndProgress || !uDets->gui.hwndStatus)
		return FALSE;
	ShowWindow(uDets->gui.hwndProgress,SW_SHOW);
	ShowWindow(uDets->gui.hwndStatus,SW_SHOW);

	SetDlgItemInt(uDets->gui.hwndMain,IDE_TTPAGE,801,FALSE);
	SendMessage(GetDlgItem(uDets->gui.hwndMain,IDE_TTPAGE),EM_LIMITTEXT,3,0);

	SendMessage(GetDlgItem(uDets->gui.hwndMain,IDL_CODEPAGE),CB_ADDSTRING,0,
				(LPARAM)L"Latin/English");
	SendMessage(GetDlgItem(uDets->gui.hwndMain,IDL_CODEPAGE),CB_SETCURSEL,0,0);
	
	SendMessage(uDets->gui.hwndMain, WM_SETICON, ICON_SMALL,
				(LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));
	SendMessage(uDets->gui.hwndMain, WM_SETICON, ICON_BIG, 
				(LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));
	
	SendMessage(uDets->gui.hwndMain,WM_COMMAND,ID_TOP,0);
	EnableWindow(GetDlgItem(uDets->gui.hwndMain,ID_BROWSE),FALSE);
	DragAcceptFiles(uDets->gui.hwndMain,TRUE);
	return TRUE;
}

static void browseFolders(Details *uDets){
	BROWSEINFO bi = {0};
	PIDLIST_ABSOLUTE pidl;

	bi.hwndOwner = uDets->gui.hwndMain;
	bi.lpszTitle = L"Select a folder";
	bi.ulFlags = BIF_USENEWUI;
	pidl = SHBrowseForFolder(&bi);
	if (!pidl) return; //Happens if error or user cancelled.
	
	if (!SHGetPathFromIDList(pidl,uDets->pi.outDir))
		uDets->pi.outDir[0] = L'\0';

	SetDlgItemText(uDets->gui.hwndMain,IDE_OUTDIR,uDets->pi.outDir);
	CoTaskMemFree(pidl);
}

static void getPrefs(Details *uDets){
	int lang, dispTT;
	wchar_t dispText[4];

	GetDlgItemText(uDets->gui.hwndMain,IDE_TTPAGE,dispText,4);
	uDets->pi.ttPageNumber = wcstol(dispText,NULL,16);
	uDets->pi.ttPageNumber &= 0x8FF;

	if (uDets->pi.ttPageNumber >> 8 >= 8)
		uDets->pi.ttPageNumber &= 0xFF;
	lang = SendMessage(GetDlgItem(uDets->gui.hwndMain,IDL_CODEPAGE),CB_GETCURSEL,0,0);
	//NOP: Doesn't really support languages yet.
	dispTT = uDets->pi.ttPageNumber;
	if (!(dispTT >> 8))
		dispTT |= 8<<8;
	_snwprintf_s(dispText,4,_TRUNCATE,L"%X",dispTT);

	SetDlgItemText(uDets->gui.hwndMain,IDE_TTPAGE,dispText);
}

static void toggleControls(Details uDets, BOOL bEnable){
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,ID_FORCEDIR),bEnable);
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,ID_ADDCOLOUR),bEnable);
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,ID_NOPROMPT),bEnable);
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,ID_BROWSE), bEnable ? 
									uDets.pi.forceOutputDir : bEnable);
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,IDE_TTPAGE),bEnable);
	EnableWindow(GetDlgItem(uDets.gui.hwndMain,IDL_CODEPAGE),bEnable);
	if (!bEnable)
		SetDlgItemText(uDets.gui.hwndMain,ID_MAINCONTROL,L"Abort");
	else SetDlgItemText(uDets.gui.hwndMain,ID_MAINCONTROL,L"Exit");
}

static void cleanupProcessing(Details *uDets){
	if (uDets->gui.hDrop)
		DragFinish(uDets->gui.hDrop);
	if (uDets->gui.hEvent)
		CloseHandle(uDets->gui.hEvent);
	if (uDets->gui.hThread)
		CloseHandle(uDets->gui.hThread);
	freeDecoder(&uDets->po.d);

	uDets->gui.hDrop = NULL;
	uDets->gui.hEvent = NULL;
	uDets->gui.hThread = NULL;
	SendMessage(uDets->gui.hwndMain,PBM_SETPOS,0,0);
	toggleControls(*uDets,TRUE);
}