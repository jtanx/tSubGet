#include "tSubGet.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPRev, LPWSTR lpCmdLine, int nCmdShow){
	INITCOMMONCONTROLSEX icc = {0};

	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_STANDARD_CLASSES|ICC_PROGRESS_CLASS|ICC_BAR_CLASSES;
	if (!InitCommonControlsEx(&icc) || CoInitialize(NULL) != S_OK){
		MessageBox(NULL,L"Error: Could not initialise common controls!",NULL,MB_ICONEXCLAMATION);
		return 1;
	}

	if (!DialogBox(hInst,MAKEINTRESOURCE(MAIN_DIALOGUE),NULL,MainDlgProc)){
		MessageBox(NULL,L"Error: Could not initialise main window!",NULL,MB_ICONEXCLAMATION);
		return 1;
	}
	CoUninitialize();
	return 0;
}