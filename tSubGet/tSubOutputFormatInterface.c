#include "tSubGet.h"

void ofInitColourSelection(HWND hwndColourList){
	int i;

	for (i = 0; i < 8; i++){
		SendMessage(hwndColourList, CB_ADDSTRING, 0, (LPARAM) colourSet[i]);
	}	
	SendMessage(hwndColourList, CB_SETCURSEL, 0, 0);
}

BOOL CALLBACK OutputFormatDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static InterfaceOpts *io;
	static OutputFormatting tmpFmt;

	switch (msg){
		case WM_INITDIALOG:{
			io = (InterfaceOpts*) lParam;
			tmpFmt = io->po.fmt;

			ofInitColourSelection(GetDlgItem(hwnd, IDL_OF_COLOUR));
			SendDlgItemMessage(hwnd, IDE_OF_COLOURVALUE, EM_LIMITTEXT, 6, 0);
			CheckDlgButton(hwnd, IDR_OF_FOREGROUND, BST_CHECKED);
			PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(0, CBN_SELCHANGE), 0);
		} break;

		case WM_COMMAND:{
			switch (LOWORD(wParam)){
				case ID_OF_DEFAULT:{
					memset(&tmpFmt.fgColour, -1, sizeof(tmpFmt.fgColour));
					PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(0, CBN_SELCHANGE), 0);
				} break;

				case IDOK:
					SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(0, CBN_SELCHANGE), 0);
					io->po.fmt = tmpFmt;
				case IDCANCEL:
					EndDialog(hwnd, LOWORD(wParam));
				break;
			}

			switch (HIWORD(wParam)){
				case CBN_SELCHANGE:{
					int currentSelection;
					currentSelection = SendDlgItemMessage(hwnd, IDL_OF_COLOUR,
																CB_GETCURSEL, 0, 0);
					if (currentSelection >= 0 && currentSelection < 8){
						if (tmpFmt.fgColour[currentSelection] >= 0){
							wchar_t textVal[7];
							_snwprintf_s(textVal, 7, _TRUNCATE, L"%06X", 
								tmpFmt.fgColour[currentSelection]);
							SetDlgItemText(hwnd, IDE_OF_COLOURVALUE, textVal);
						} else {
							SetDlgItemText(hwnd, IDE_OF_COLOURVALUE, L"");
						}
					}
					SetFocus(GetDlgItem(hwnd, IDE_OF_COLOURVALUE));
					SendDlgItemMessage(hwnd, IDE_OF_COLOURVALUE, EM_SETSEL, 0, -1);
				} break;

				case EN_KILLFOCUS:{
					if (LOWORD(wParam) == IDE_OF_COLOURVALUE){
						wchar_t textVal[7];
						int currentVal, currentSelection;
						currentSelection = SendDlgItemMessage(hwnd, IDL_OF_COLOUR,
																CB_GETCURSEL, 0, 0);
						if (currentSelection >= 0 && currentSelection < 8){
							GetDlgItemText(hwnd, IDE_OF_COLOURVALUE, textVal,7);
							if (textVal[0] == L'\0')
								tmpFmt.fgColour[currentSelection] = -1;
							else{
								currentVal = wcstol(textVal, NULL, 16);
								tmpFmt.fgColour[currentSelection] = currentVal;
								_snwprintf_s(textVal, 7, _TRUNCATE, L"%06X", currentVal);
								SetDlgItemText(hwnd, IDE_OF_COLOURVALUE, textVal);
							}
						}
					}
				} break;
			}
		} break;

		default:{
			return FALSE;
		} break;
	}
	return TRUE;
}
