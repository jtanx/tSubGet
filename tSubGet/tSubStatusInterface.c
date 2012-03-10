#include "tSubGet.h"

static unsigned WINAPI parseFiles(void *args){
	StatusData *sd = (StatusData*) args;
	int i, ret, count = listGetCount(sd->io->hwndList);
	wchar_t *disp, buf[BUFSIZ];

	CoInitialize(NULL);
	for (i = 0; i < count ; i++){
		if (!listGetItem(sd->io->hwndList, i, &disp, sd->io->po.fileIn, MAX_PATH)){
			MessageBox(sd->hwndStatusDlg, L"Error: Could not read info from list!", NULL, MB_ICONEXCLAMATION);
			return FALSE;
		}
		
		_snwprintf_s(buf, BUFSIZ, _TRUNCATE, L"tSubGet Status - [%d / %d]", i+1, count);
		SetWindowText(sd->hwndStatusDlg, buf);
		SetDlgItemText(sd->hwndStatusDlg, IDE_S_FILENAME, disp);
		free(disp);

		sd->io->po.fileOut[0] = L'\0'; //Re-init to default
		while ((ret = tsgInit(&sd->p, &sd->io->po)) == PARSER_E_OUT_EXISTS){
			int mbret;
			
			_snwprintf_s(buf, BUFSIZ, _TRUNCATE, L"Warning: %s exists, overwrite?", sd->io->po.fileOut);
			mbret = MessageBox(sd->hwndStatusDlg, buf, L"File Exists", MB_YESNO|MB_ICONQUESTION);
			if (mbret != IDYES)
				break;

			sd->io->po.overwriteOutput = TRUE;
		}

		if (ret != PARSER_OK) break;
		else if ((ret = tsgProcess(sd->p)) != PARSER_OK) break;
		else if (tsgWriteout(sd->p) != PARSER_OK) break;

		tsgClose(&sd->p);
	}

	switch (ret){
		case PARSER_OK: case PARSER_E_ABORT: case PARSER_E_OUT_EXISTS:
			break;
		
		default:{
			wchar_t fmt[BUFSIZ];
			tsgGetError(ret, buf, BUFSIZ);
			_snwprintf_s(fmt, BUFSIZ, _TRUNCATE, 
							L"Error: The parser returned the following:\r\n\r'%s'", buf);
			MessageBox(sd->hwndStatusDlg, fmt, NULL, MB_ICONEXCLAMATION);
		}
	}

	tsgClose(&sd->p);
	CoUninitialize();
	return TRUE;
}

BOOL CALLBACK StatusDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static StatusData sd = {0};

	switch (msg){
		case WM_INITDIALOG:{
			sd.io = (InterfaceOpts *) lParam;
			sd.hwndStatusDlg = hwnd;
			
			sd.hWorkerThread = (HANDLE)_beginthreadex(NULL,0,parseFiles,
															(void*)&sd,0,NULL);

			SetTimer(hwnd, 1, TIMERUNIT, NULL);
			SendMessage(hwnd, WM_TIMER, 1, 0);
			
			//Sets focus on abort button (use in place of SetFocus). Must be PostMessage due to _beginthreadex
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hwnd, ID_S_ABORT), TRUE);
		} break;

		case WM_TIMER:{
			wchar_t buf[BUFSIZ];
			int timeInSeconds = sd.timeLapsed / TIMERUNITSPERSEC;
			
			//Determine if the worker thread is finished
			if (IsEventActive(sd.hWorkerThread)){
				EndDialog(sd.hwndStatusDlg, IDOK);
				break;
			}

			if (tsgGetPositionStr(sd.p, buf, BUFSIZ))
				SetDlgItemText(hwnd, IDE_S_FILEPOS, buf);
			
			_snwprintf_s(buf, BUFSIZ, _TRUNCATE, L"%.2d:%.2d:%.2d", 
				timeInSeconds / 3600,(timeInSeconds % 3600) / 60, timeInSeconds);
			SetDlgItemText(hwnd, IDE_S_TIMELAPSED, buf);
			InterlockedIncrement(&sd.timeLapsed);

			SendDlgItemMessage(hwnd, IDP_S_PROGRESS, PBM_SETPOS, 
								tsgGetProgress(sd.p), 0);
		} break;

		case WM_COMMAND:{
			switch (LOWORD(wParam)){
		case ID_S_ABORT: case IDOK: case IDCANCEL:{
					tsgSignalAbort(sd.p);
					//EndDialog(hwnd, LOWORD(wParam));
				} break;
			}
		} break;

		default:{
			return FALSE;
		} break;
	}
	return TRUE;
}