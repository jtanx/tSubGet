#include "tSubGet.h"

void ifInitLangSelection(HWND hwndLangList){
	int i;
	wchar_t buf[BUFSIZ];

	for (i = 0; i < LANGID_NULL; i++){
		tsgGetLangStr(i, buf, BUFSIZ);
		SendMessage(hwndLangList, CB_ADDSTRING, 0, (LPARAM) buf);
	}	
}

void ifDisplayPrefs(HWND hwndMain, InterfaceOpts *io){
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
	SendDlgItemMessage(hwndMain, IDE_TTPAGE, EM_LIMITTEXT, 3, 0);

	SetDlgItemInt(hwndMain, IDE_DELAY, (UINT) io->po.delay, TRUE);

	//Set language selection display
	SendDlgItemMessage(hwndMain, IDL_CODEPAGE, CB_SETCURSEL, io->po.langId, 0);
}

static void ifReadPrefs(HWND hwndMain, InterfaceOpts *io){
	wchar_t dispText[4];
	
	io->po.addColourTags = IsDlgButtonChecked(hwndMain, ID_ADDCOLOUR);
	io->po.overwriteOutput = IsDlgButtonChecked(hwndMain, ID_NOPROMPT);
	
	GetDlgItemText(hwndMain, IDE_TTPAGE, dispText, 4);
	
	io->po.pageNumber = wcstol(dispText, NULL, 16) & 0xFFF;
	if (io->po.pageNumber  >> 8 >= 8)
					io->po.pageNumber &= 0xFF;
	
	io->po.delay = (int)GetDlgItemInt(hwndMain, IDE_DELAY, NULL, TRUE);
	
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

static void ifBrowseFolderDlg(HWND hwndMain, InterfaceOpts *io){
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

static void ifOnListCommand(HWND hwnd, int id, HWND hwndList, InterfaceOpts *io){
	switch (id){
		case ID_LISTADD:{
			if (!ifAddFileDlg(hwnd, hwndList)){
				if (CommDlgExtendedError())
					MessageBox(hwnd, L"Could not add files - ensure filenames are not too long",
								NULL, MB_ICONEXCLAMATION);
			}
		} break;

		case ID_LISTLOCATE:{
			listLocateFirstSelected(hwndList);
		} break;

		case ID_LISTDEL:{
			listDeleteSelected(hwndList);
		} break;

		case ID_LISTCLEAR:{
			listClear(hwndList);
		} break;

		case ID_LISTUP: case ID_LISTDOWN:{
			int dir = (id == ID_LISTUP) ? UP : DOWN;
			listMoveSelected(hwndList, dir);
		} break;
	}
}

void ifOnCommand(HWND hwnd, int id, HWND hwndCtl, InterfaceOpts *io){
	if (IsListId(id))
		ifOnListCommand(hwnd, id, GetDlgItem(hwnd, IDL_INPUT), io);
	else switch (id){
		case IDOK: case IDCANCEL:{
			listClear(GetDlgItem(hwnd, IDL_INPUT));
			ifReadPrefs(hwnd, io);
			if (io->autosave && !prefSave(io))
				MessageBox(hwnd, L"Error: Could not save preferences", NULL, MB_ICONEXCLAMATION);
			
			EndDialog(hwnd, id);
		} break;

		case ID_ABOUT:{
			wchar_t buf[BUFSIZ];
			_snwprintf_s(buf, BUFSIZ, _TRUNCATE,
				L"tSubGet Version %s (Core version %s (%s))\n\n"
				L"tSubGet extracts teletext-formatted subtitles (usually from DVB-style recordings) "
				L"that may be present in WTV or DVR-MS files. Note that only DVR-MS files are "
				L"supported in Windows XP.\n\n"
				L"Jeremy Tan - 2011-2018", GUI_BUILD_VERSION, BUILD_VERSION, BUILD_DATE);
			MessageBox(hwnd, buf, L"About", MB_ICONINFORMATION);
		} break;

		case ID_TOPMOST:{
			int ret = GetMenuState(GetMenu(hwnd),ID_TOPMOST,MF_BYCOMMAND) ^ MF_CHECKED;
			CheckMenuItem(GetMenu(hwnd), ID_TOPMOST, ret);
			SetWindowPos(hwnd, ret ? HWND_TOPMOST : HWND_NOTOPMOST, 1, 1, 1, 1,
				SWP_NOREDRAW|SWP_NOSIZE|SWP_NOMOVE);
		} break;

		case ID_OUTPUTFORMAT:{
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OUTPUTFORMAT),
						hwnd, OutputFormatDlgProc, (LPARAM) io);
		} break;

		case ID_AUTOSAVE:{
			io->autosave = GetMenuState(GetMenu(hwnd), ID_AUTOSAVE,MF_BYCOMMAND) ^ MF_CHECKED;
			CheckMenuItem(GetMenu(hwnd), ID_AUTOSAVE, io->autosave);
			if (!io->autosave)
				SendMessage(hwnd, WM_COMMAND, ID_DELSETTINGS, 0);
		} break;

		case ID_DELSETTINGS:{
			io->autosave = FALSE;
			CheckMenuItem(GetMenu(hwnd), ID_AUTOSAVE, MF_UNCHECKED);

			if (!prefDelete())
				MessageBox(hwnd, L"Error: Could not delete settings!\n"
								 L"Settings are located under HKCU\\Software\\tSubGet.", 
								 NULL, MB_ICONEXCLAMATION);
		} break;

		case ID_SETFOLDER:{
			int state = IsDlgButtonChecked(hwnd, ID_SETFOLDER) ^ BST_CHECKED;
			
			if (state == BST_CHECKED)
				ifBrowseFolderDlg(hwnd, io);
			else
				io->po.folderOut[0] = L'\0';

			ifReadPrefs(hwnd, io); //Both reads and updates display
		} break;

		case ID_BROWSE:{
			ifBrowseFolderDlg(hwnd, io);
		} break;

		case ID_MAINCONTROL:{
			ifReadPrefs(hwnd, io);
			if (listGetCount(GetDlgItem(hwnd, IDL_INPUT)) > 0)
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STATUS), 
								hwnd, StatusDlgProc, (LPARAM) io);
		} break;
	}
}