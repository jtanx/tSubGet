#include "tSubGet.h"

static void ifInitLangSelection(HWND hwndLangList){
	int i;
	wchar_t buf[BUFSIZ];

	for (i = 0; i <= LANGID_DEFAULT; i++){
		tsgGetLangStr(i, buf, BUFSIZ);
		SendMessage(hwndLangList, CB_ADDSTRING, 0, (LPARAM) buf);
	}	
}

static void ifDisplayPrefs(HWND hwndMain, InterfaceOpts *io){
	wchar_t dispText[4];
	int disp = !(io->po.pageNumber >> 8) ? (io->po.pageNumber | 8 << 8) : 
											io->po.pageNumber;
	int i;
	
	//Set folder out display
	i = (io->po.folderOut[0] != L'\0') ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(hwndMain, ID_SETFOLDER, i);
	SetDlgItemText(hwndMain, IDE_OUTDIR, io->po.folderOut);
	EnableWindow(GetDlgItem(hwndMain, ID_BROWSE), i);

	CheckDlgButton(hwndMain, ID_ADDCOLOUR, io->po.addColourTags);
	CheckDlgButton(hwndMain, ID_NOPROMPT, io->po.overwriteOutput);
		
	//Set teletext page display
	_snwprintf_s(dispText, 4, _TRUNCATE, L"%X", disp);
	SetDlgItemText(hwndMain, IDE_TTPAGE, dispText);

	SetDlgItemInt(hwndMain, IDE_DELAY, (UINT) io->po.delay, TRUE);

	//Set language selection display
	SendMessage(GetDlgItem(hwndMain, IDL_CODEPAGE), CB_SETCURSEL, io->po.langId, 0);

	i = io->autosave ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem(GetMenu(hwndMain), ID_AUTOSAVE, i);
}

static void ifRetrievePrefs(HWND hwndMain, InterfaceOpts *io){
	wchar_t dispText[4];
	
	io->po.addColourTags = IsDlgButtonChecked(hwndMain, ID_ADDCOLOUR);
	io->po.overwriteOutput = IsDlgButtonChecked(hwndMain, ID_NOPROMPT);
	
	GetDlgItemText(hwndMain, IDE_TTPAGE, dispText, 4);
	
	io->po.pageNumber = wcstol(dispText, NULL, 16) & 0xFFF;
	if (io->po.pageNumber  >> 8 >= 8)
					io->po.pageNumber &= 0xFF;
	
	io->po.delay = GetDlgItemInt(hwndMain, IDE_DELAY, NULL, TRUE);
	
	io->po.langId = SendDlgItemMessage(hwndMain, IDL_CODEPAGE, CB_GETCURSEL, 0, 0);
	if (io->po.langId == CB_ERR)
		io->po.langId = LANGID_DEFAULT;
	
	ifDisplayPrefs(hwndMain, io);
}

static int ifAddFileDlg(HWND hwnd, HWND hwndList){
	OPENFILENAME ofn = {0};
	wchar_t buf[BUFSIZE], *ptr;
	wchar_t fullPath[MAX_PATH];
	int count = 0;
	
	buf[0] = L'\0'; //Initialised for use with ofn
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"WMC Recordings (*.dvr-ms; *.wtv)\0*.DVR-MS;*.WTV\0";
	ofn.Flags = OFN_ALLOWMULTISELECT|OFN_FILEMUSTEXIST|OFN_EXPLORER;
	ofn.lpstrFile = buf;
	ofn.nMaxFile = BUFSIZE;
	ofn.hInstance = GetModuleHandle(NULL);

	if (!GetOpenFileName(&ofn))
		return FALSE;

	//If multiple selection, first string is directory, null separated with the
	//selected filenames.
	ptr = wcsrchr(buf, L'\0') + 1;
	while (ptr[0] != L'\0'){
		_snwprintf_s(fullPath, MAX_PATH, _TRUNCATE, L"%s\\%s", buf, ptr);

		if (!listAdd(hwndList, fullPath))
			return FALSE;
		ptr = wcschr(ptr, L'\0') + 1;
		count++;
	}

	if (!count) //Only one file present
		return listAdd(hwndList, buf);
	return TRUE;
}

static void ifFolderDlg(HWND hwndMain, InterfaceOpts *io){
	BROWSEINFO bi = {0};
	PIDLIST_ABSOLUTE pidl;

	bi.hwndOwner = hwndMain;
	bi.lpszTitle = L"Select a folder";
	bi.ulFlags = BIF_USENEWUI;
	pidl = SHBrowseForFolder(&bi);
	if (!pidl) return; //Happens if error or user cancelled.
	
	if (!SHGetPathFromIDList(pidl, io->po.folderOut))
		io->po.folderOut[0] = L'\0';

	SetDlgItemText(hwndMain,IDE_OUTDIR,io->po.folderOut);
	CoTaskMemFree(pidl);
}

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static InterfaceOpts io;

	switch (msg){
		case WM_INITDIALOG:{
			prefLoad(&io);
			
			ifInitLangSelection(GetDlgItem(hwnd, IDL_CODEPAGE));
			ifDisplayPrefs(hwnd, &io);
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
			switch (LOWORD(wParam)){
				case IDOK: case IDCANCEL:{
					listClear(GetDlgItem(hwnd, IDL_INPUT));
					ifRetrievePrefs(hwnd, &io);
					if (io.autosave && !prefSave(&io))
						MessageBox(hwnd, L"Error: Could not save preferences", NULL, MB_ICONEXCLAMATION);
					EndDialog(hwnd, LOWORD(wParam));
				} break;

				case ID_ABOUT:{
				} break;

				case ID_DEL:{
					listDeleteSelected(GetDlgItem(hwnd, IDL_INPUT));
				} break;

				case ID_CLEAR:{
					listClear(GetDlgItem(hwnd, IDL_INPUT));
				} break;

				case ID_ADD:{
					if (!ifAddFileDlg(hwnd, GetDlgItem(hwnd, IDL_INPUT))){
						if (CommDlgExtendedError())
							MessageBox(hwnd, L"Could not add files - ensure filenames are not too long",
										NULL, MB_ICONEXCLAMATION);
					}
				} break;

				case ID_UP: case ID_DOWN:{
					int dir = LOWORD(wParam) == ID_UP ? UP : DOWN;
					listMoveSelected(GetDlgItem(hwnd, IDL_INPUT), dir);
				} break;

				case ID_TOPMOST:{
					int ret = GetMenuState(GetMenu(hwnd),ID_TOPMOST,MF_BYCOMMAND);
					if (ret == MF_CHECKED){
						CheckMenuItem(GetMenu(hwnd), ID_TOPMOST, MF_UNCHECKED);
						SetWindowPos(hwnd, HWND_NOTOPMOST, 1, 1 ,1, 1, 
							SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
					} else{
						CheckMenuItem(GetMenu(hwnd), ID_TOPMOST, MF_CHECKED);
						SetWindowPos(hwnd, HWND_TOPMOST, 1, 1, 1, 1, 
							SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
					}
				} break;

				case ID_SETFOLDER:{
					int state = IsDlgButtonChecked(hwnd, ID_SETFOLDER) ^ BST_CHECKED;
					
					ifRetrievePrefs(hwnd, &io);
					if (state == BST_CHECKED)
						ifFolderDlg(hwnd, &io);
					else
						io.po.folderOut[0] = L'\0';
					
					ifDisplayPrefs(hwnd, &io);
				} break;

				case ID_BROWSE:{
					ifFolderDlg(hwnd, &io);
				} break;

				case ID_AUTOSAVE:{
					io.autosave = !(GetMenuState(GetMenu(hwnd), ID_AUTOSAVE,
									MF_BYCOMMAND) == MF_CHECKED);
					CheckMenuItem(GetMenu(hwnd), ID_AUTOSAVE,
						io.autosave ? MF_CHECKED : MF_UNCHECKED);
					if (!io.autosave)
						SendMessage(hwnd, WM_COMMAND, ID_DELSETTINGS, 0);
				} break;

				case ID_DELSETTINGS:{
					io.autosave = FALSE;
					CheckMenuItem(GetMenu(hwnd), ID_AUTOSAVE, MF_UNCHECKED);

					if (!prefDelete())
						MessageBox(hwnd, L"Error: Could not delete settings! Settings are located under HKCU\\Software\\tSubGet.", NULL, MB_ICONEXCLAMATION);

				} break;

				case ID_MAINCONTROL:{
					ifRetrievePrefs(hwnd, &io);	
				} break;
			} 
		} break; //End WM_COMMAND

		default:{
			return FALSE;
		} break;
	}
	return TRUE;
}