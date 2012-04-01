#include "tSubGet.h"

#define loadValue(hKey, keyName, pBuf, bufSize) \
		RegQueryValueEx(hKey, keyName, 0, NULL, pBuf, &bufSize)
#define setInt(hKey, keyName, pBuf, bufSize) \
		RegSetValueEx(hKey, keyName, 0, REG_DWORD, (void*)pBuf, bufSize)
#define setString(hKey, keyName, pBuf, bufSize) \
		RegSetValueEx(hKey, keyName, 0, REG_SZ, (void*)pBuf, bufSize)

static const wchar_t *keyName		= L"Software\\tSubget";
static const wchar_t *stringSet[]	= 
{
	L"folderOut" 
};
static const wchar_t *numSet[]		= 
{
	L"langID", L"addColourTags", 
	L"overwriteOutput", L"pageNumber", L"delay" 
};

static void loadInt(HKEY hKey, const wchar_t *keyName, void *pBuf, 
							int bufSize, int defaultValue){
	if (loadValue(hKey, keyName, pBuf, bufSize) != ERROR_SUCCESS)
			*((int*)(pBuf)) = defaultValue;
}
static void loadString(HKEY hKey, const wchar_t *keyName, void *pBuf, 
							int bufSize){
	if (loadValue(hKey, keyName, pBuf, bufSize) != ERROR_SUCCESS)
			memset(pBuf, 0, sizeof(wchar_t)*(bufSize));
}

static void prefLoadFmt(HKEY hKeyMain, InterfaceOpts *io){
	HKEY hKeyFmt;
	int i, ret;

	ret = RegOpenKeyEx(hKeyMain, L"Formatting", 0, KEY_READ, &hKeyFmt);
	if (ret == ERROR_SUCCESS){
		HKEY hKeyFgColour;
		ret = RegOpenKeyEx(hKeyFmt, L"Foreground Colour", 0, KEY_READ, &hKeyFgColour);
		if (ret == ERROR_SUCCESS){
			for (i = 0; i < 8; i++)
				loadInt(hKeyFgColour, colourSet[i], &(io->po.fmt.fgColour[i]), 
						sizeof(io->po.fmt.fgColour[i]), -1);
			RegCloseKey(hKeyFgColour);
		}
		
		RegCloseKey(hKeyFmt);
	}
}

static void prefSaveFmt(HKEY hKeyMain, InterfaceOpts *io){
	HKEY hKeyFmt;
	int i, ret;

	ret = RegCreateKeyEx(hKeyMain, L"Formatting", 0, NULL, 0, KEY_WRITE,
							 NULL, &hKeyFmt, NULL);
	if (ret == ERROR_SUCCESS){
		HKEY hKeyFgColour;
		
		ret = RegCreateKeyEx(hKeyFmt, L"Foreground Colour", 0, NULL, 0, KEY_WRITE,
							 NULL, &hKeyFgColour, NULL);
		if (ret == ERROR_SUCCESS){
			for (i = 0; i < 8; i++)
				setInt(hKeyFgColour, colourSet[i], &(io->po.fmt.fgColour[i]), 
						sizeof(io->po.fmt.fgColour[i]));
			RegCloseKey(hKeyFgColour);
		}
		RegCloseKey(hKeyFmt);
	}	
}

void prefLoad(InterfaceOpts *io){
	HKEY hKey;
	int ret;

	memset(io, 0, sizeof(InterfaceOpts));
	memset(&io->po.fmt.fgColour, -1, sizeof(io->po.fmt.fgColour));
	ret = RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS){
		io->po.pageNumber = 1; //801
		return;
	}

	loadString(hKey, stringSet[0], io->po.folderOut, sizeof(io->po.folderOut));

	loadInt(hKey, numSet[0], &(io->po.langId), 
		sizeof(io->po.langId), LANGID_DEFAULT);
	loadInt(hKey, numSet[1], &(io->po.addColourTags), 
		sizeof(io->po.addColourTags), FALSE);
	loadInt(hKey, numSet[2], &(io->po.overwriteOutput), 
		sizeof(io->po.overwriteOutput), FALSE);
	loadInt(hKey, numSet[3], &(io->po.pageNumber), 
		sizeof(io->po.pageNumber), 1);
	loadInt(hKey, numSet[4], &(io->po.delay), 
		sizeof(io->po.delay), 0);

	prefLoadFmt(hKey, io);
	
	io->autosave = TRUE;
	RegCloseKey(hKey);
}

int prefSave(InterfaceOpts *io){
	HKEY hKey;
	int ret;

	ret = RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, 0, KEY_WRITE,
							 NULL, &hKey, NULL);
	if (ret != ERROR_SUCCESS)
		return FALSE;
	
	setString(hKey, stringSet[0], io->po.folderOut, sizeof(io->po.folderOut));

	setInt(hKey, numSet[0], &(io->po.langId), sizeof(io->po.langId));
	setInt(hKey, numSet[1], &(io->po.addColourTags), sizeof(io->po.addColourTags));
	setInt(hKey, numSet[2], &(io->po.overwriteOutput), sizeof(io->po.overwriteOutput));
	setInt(hKey, numSet[3], &(io->po.pageNumber), sizeof(io->po.pageNumber));
	setInt(hKey, numSet[4], &(io->po.delay), sizeof(io->po.delay));

	prefSaveFmt(hKey, io);
	
	RegCloseKey(hKey);
	return TRUE;
}

/*
  prefDelete() deletes all preferences located under HKCU\Software\tSubGet.
  prefDelete() will return TRUE on success, and also if there are no settings
  to delete in the first place.
*/
int prefDelete(){
	HKEY hKeyMain, hKeyFmt;
	int ret = RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_READ|DELETE, &hKeyMain);
	if (ret != ERROR_SUCCESS) return TRUE;
	
	ret = RegOpenKeyEx(hKeyMain, L"Formatting", 0, KEY_READ|DELETE, &hKeyFmt);
	if (ret == ERROR_SUCCESS){
		RegDeleteKey(hKeyFmt, L"Foreground Colour");
		RegCloseKey(hKeyFmt);
	}
	RegDeleteKey(hKeyMain, L"Formatting");
	RegCloseKey(hKeyMain);

	return (RegDeleteKey(HKEY_CURRENT_USER, keyName) == ERROR_SUCCESS) || 
		   (GetLastError() == 0);
}