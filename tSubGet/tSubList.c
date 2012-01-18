#include "tSubGet.h"

static int getExtentPx(HWND hwndList, wchar_t *text, int textLength){
	HANDLE hdc = GetDC(hwndList);
	HANDLE hFont = (HANDLE) SendMessage(hwndList, WM_GETFONT, 0, 0);
	HANDLE hOld = SelectObject(hdc, hFont);
	SIZE sz;

	if (hOld == HGDI_ERROR || !hOld || !hFont)
		return 0;
	
	GetTextExtentPoint32(hdc, text, textLength, &sz);
	
	SelectObject(hdc, hOld);
	ReleaseDC(hwndList, hdc);
	return sz.cx + 10; //Add 10px for prettyness
}

int listAdd(HWND hwndList, wchar_t *fullPath){
	wchar_t *ptr, *buf;
	int index, newExtent, len = wcslen(fullPath)+1;

	if (!(ptr = wcsrchr(fullPath, L'\\')))
		ptr = fullPath;
	else ptr++;
	
	newExtent = getExtentPx(hwndList, ptr, wcslen(ptr));
	if (newExtent > SendMessage(hwndList, LB_GETHORIZONTALEXTENT, 0, 0))
		SendMessage(hwndList, LB_SETHORIZONTALEXTENT, newExtent, 0);
	
	index = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) ptr);
	if (index < 0)
		return FALSE;

	buf = malloc(sizeof(wchar_t)*len);
	if (!buf) 
		return FALSE;

	wcscpy_s(buf, len, fullPath);
	if (SendMessage(hwndList, LB_SETITEMDATA, index, (LPARAM) buf) == LB_ERR){
		free(buf);
		return FALSE;
	}

	return TRUE;
}

static int delItem(HWND hwndList, int index){
	int ret;
	void *buf = (void *)SendMessage(hwndList, LB_GETITEMDATA, index, 0);
	if ((int) buf == LB_ERR)
		return FALSE;

	free(buf);
	ret = SendMessage(hwndList, LB_DELETESTRING, index, 0) != LB_ERR;
	
	if (!SendMessage(hwndList, LB_GETCOUNT, 0, 0))
		SendMessage(hwndList, LB_SETHORIZONTALEXTENT, 0, 0);
	return ret;
}

int listDeleteSelected(HWND hwndList){
	int nSelected = SendMessage(hwndList, LB_GETSELCOUNT, 0, 0);
	int *buf, nReceived, i, ret = TRUE;
	if (nSelected < 0) return FALSE;

	buf = malloc(sizeof(int) * nSelected);
	if (!buf) return FALSE;

	nReceived = SendMessage(hwndList, LB_GETSELITEMS, nSelected, (LPARAM) buf);
	if (nReceived != nSelected)
		ret = FALSE;
	else for (i = nReceived - 1; i >= 0; i--){
		if (!delItem(hwndList, buf[i])){
			ret = FALSE;
			break;
		}
	}
		
	free (buf);
	return ret;
}

int listClear(HWND hwndList){
	int i, count = SendMessage(hwndList, LB_GETCOUNT, 0, 0);

	if (count >= 0){
		for (i = 0; i < count; i++){
			if (!delItem(hwndList, 0))
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

static int moveItem(HWND hwndList, int from, int to){
	LPARAM data = SendMessage(hwndList, LB_GETITEMDATA, from, 0);
	int len = SendMessage(hwndList, LB_GETTEXTLEN, from, 0);
	int ret = TRUE;
	wchar_t *name;

	if (data == LB_ERR || len == LB_ERR)
		return FALSE;

	name = malloc(sizeof(wchar_t)*(len + 1));
	if (!name)
		return FALSE;

	if (SendMessage(hwndList, LB_GETTEXT, from, (LPARAM) name) == LB_ERR ||
		SendMessage(hwndList, LB_DELETESTRING, from, 0) == LB_ERR){
		ret = FALSE;
	} else if (SendMessage(hwndList, LB_INSERTSTRING, to, (LPARAM) name) == LB_ERR ||
			   SendMessage(hwndList, LB_SETITEMDATA, to, data) == LB_ERR){
		free((void*) data);
		ret = FALSE;
	}
	SendMessage(hwndList, LB_SETSEL, TRUE, to);

	free(name);
	return  ret;
}

int listMoveSelected(HWND hwndList, int direction){
	int nSelected = SendMessage(hwndList, LB_GETSELCOUNT, 0, 0);
	int total = SendMessage(hwndList, LB_GETCOUNT, 0, 0);
	int *buf, nReceived, to, i, ret = TRUE;
	if (nSelected < 0 || total == LB_ERR) 
		return FALSE;

	buf = malloc(sizeof(int) * nSelected);
	if (!buf) return FALSE;

	nReceived = SendMessage(hwndList, LB_GETSELITEMS, nSelected, (LPARAM) buf);
	if (nReceived != nSelected)
		ret = FALSE;
	else for (i = 0; i < nReceived; i++){
		int j;	
		if (direction == UP){
			j = i; //Ensure indexes of ther selected items are unaffected on move
			to = buf[j] - 1;
		} else{
			j = nReceived - i - 1;
			to = buf[j] + 1;
		}
		
		if (to < 0 || to >= total) break;
		else if (!moveItem(hwndList, buf[j], to)){
			ret = FALSE;
			break;
		}
	}

	free(buf);
	return ret;	
}