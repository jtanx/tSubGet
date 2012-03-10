#include "tSubGet.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmdLine, int nCmdShow){
	INITCOMMONCONTROLSEX icc = {0};

	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_STANDARD_CLASSES|ICC_PROGRESS_CLASS;
	if (!InitCommonControlsEx(&icc)){
		MessageBox(NULL,L"Error: Could not initialise common controls!",NULL,MB_ICONEXCLAMATION);
		return 1;
	} else if (!DialogBox(hInst,MAKEINTRESOURCE(IDD_MAIN),NULL,MainDlgProc)){
		MessageBox(NULL,L"Error: Could not initialise main window!",NULL,MB_ICONEXCLAMATION);
		return 1;
	}

	return 0;
}

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static InterfaceOpts io;

	switch (msg){
		case WM_INITDIALOG:{
			prefLoad(&io);
			io.hwndList = GetDlgItem(hwnd, IDL_INPUT);
			
			ifInitLangSelection(GetDlgItem(hwnd, IDL_CODEPAGE));
			ifDisplayPrefs(hwnd, &io);

			CheckMenuItem(GetMenu(hwnd), ID_AUTOSAVE, io.autosave ? MF_CHECKED : 
																	MF_UNCHECKED);
			//Set dialogue icons
			SendMessage(hwnd, WM_SETICON, ICON_SMALL,
				(LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));
			SendMessage(hwnd, WM_SETICON, ICON_BIG, 
				(LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));

			DragAcceptFiles(hwnd, TRUE);
		} break;

		case WM_DROPFILES:{
			HANDLE hDrop = (HANDLE) wParam;
			int i, count = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			wchar_t buf[MAX_PATH], *ptr;

			for (i = 0; i < count; i++){
				if (!DragQueryFile(hDrop, i, buf, MAX_PATH))
					break;
				ptr = wcsrchr(buf, L'.');
				if (ptr && (!_wcsicmp(ptr, L".dvr-ms") || !_wcsicmp(ptr, L".wtv")))
					listAdd(GetDlgItem(hwnd, IDL_INPUT), buf);
			}
			DragFinish(hDrop);
		} break;

		case WM_COMMAND:{
			ifOnCommand(hwnd, LOWORD(wParam), (HWND)lParam, &io);
		} break;

		default:{
			return FALSE;
		} break;
	}
	return TRUE;
}