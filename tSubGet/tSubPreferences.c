#include "tSubGet.h"

#define loadValue(hKey, keyName, pBuf, bufSize) \
		RegQueryValueEx(hKey, keyName, 0, NULL, pBuf, &bufSize)
#define setInt(hKey, keyName, pBuf, bufSize) \
		RegSetValueEx(hKey, keyName, 0, REG_DWORD, (void*)pBuf, bufSize)
#define setString(hKey, keyName, pBuf, bufSize) \
		RegSetValueEx(hKey, keyName, 0, REG_SZ, (void*)pBuf, bufSize)

static const wchar_t *keyName = L"Software\\tSubget";
static const wchar_t *numSet[] = 
{L"langID", L"addColourTags", L"customColours", 
 L"overwriteOutput", L"pageNumber", L"delay"};
static const wchar_t *colourSet[8] = 
{L"black", L"red", L"green", L"yellow",
 L"blue", L"magenta", L"cyan", L"white"};
static const wchar_t *stringSet[] = {L"folderOut"};

__inline static void loadInt(HKEY hKey, const wchar_t *keyName, void *pBuf, 
							 int bufSize, int defaultValue){
	if (loadValue(hKey, keyName, pBuf, bufSize) != ERROR_SUCCESS)
			*((int*)(pBuf)) = defaultValue;
}
__inline static void loadString(HKEY hKey, const wchar_t *keyName, void *pBuf, 
								int bufSize){
	if (loadValue(hKey, keyName, pBuf, bufSize) != ERROR_SUCCESS)
			memset(pBuf, 0, sizeof(wchar_t)*(bufSize));
}

void prefLoad(InterfaceOpts *io){
	HKEY hKey;
	int i, ret;

	memset(io, 0, sizeof(InterfaceOpts));
	ret = RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS){
		io->po.pageNumber = 1; //801
		return;
	}

	loadString(hKey, stringSet[0], &(io->po.folderOut), sizeof(io->po.folderOut));

	loadInt(hKey, numSet[0], &(io->po.langId), 
		sizeof(io->po.langId), LANGID_DEFAULT);
	loadInt(hKey, numSet[1], &(io->po.addColourTags), 
		sizeof(io->po.addColourTags), FALSE);
	loadInt(hKey, numSet[2], &(io->po.customColours), 
		sizeof(io->po.customColours), FALSE);
	loadInt(hKey, numSet[3], &(io->po.overwriteOutput), 
		sizeof(io->po.overwriteOutput), FALSE);
	loadInt(hKey, numSet[4], &(io->po.pageNumber), 
		sizeof(io->po.pageNumber), 1);
	loadInt(hKey, numSet[5], &(io->po.delay), 
		sizeof(io->po.delay), 0);
	if (io->po.customColours){
		HKEY hKeyColours;
		ret = RegOpenKeyEx(hKey, L"Colours", 0, KEY_READ, &hKeyColours);
		
		if (ret != ERROR_SUCCESS){
			io->po.customColours = FALSE;
			memset(&(io->po.colours), -1, sizeof(io->po.colours[0]*8));
		}
		else{
			for (i = 0; i < 8; i++)
				loadInt(hKeyColours, colourSet[i], &(io->po.colours[i]), 
					sizeof(io->po.colours[i]), -1);
			RegCloseKey(hKeyColours);
		}
	}
	
	io->autosave = TRUE;
	RegCloseKey(hKey);
}

int prefSave(InterfaceOpts *io){
	HKEY hKey;
	int i, ret;

	ret = RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, 0, KEY_WRITE,
							 NULL, &hKey, NULL);
	if (ret != ERROR_SUCCESS)
		return FALSE;
	
	setString(hKey, stringSet[0], &(io->po.folderOut), sizeof(io->po.folderOut));

	setInt(hKey, numSet[0], &(io->po.langId), sizeof(io->po.langId));
	setInt(hKey, numSet[1], &(io->po.addColourTags), sizeof(io->po.addColourTags));
	setInt(hKey, numSet[2], &(io->po.customColours), sizeof(io->po.customColours));
	setInt(hKey, numSet[3], &(io->po.overwriteOutput), sizeof(io->po.overwriteOutput));
	setInt(hKey, numSet[4], &(io->po.pageNumber), sizeof(io->po.pageNumber));
	setInt(hKey, numSet[5], &(io->po.delay), sizeof(io->po.delay));

	if (io->po.customColours){
		HKEY hKeyColours;
		ret = RegCreateKeyEx(hKeyColours, L"Colours", 0, NULL, 0, KEY_WRITE,
							 NULL, &hKeyColours, NULL);
		if (ret == ERROR_SUCCESS){
			for (i = 0; i < 8; i++)
				setInt(hKeyColours, colourSet[i], &(io->po.colours[i]), 
					sizeof(io->po.colours[i]));
			RegCloseKey(hKeyColours);
		} else{
			RegCloseKey(hKey);
			return FALSE;
		}
	} else RegDeleteKey(hKey, L"Colours");
	
	RegCloseKey(hKey);
	return TRUE;
}

/*
  prefDelete() deletes all preferences located under HKCU\Software\tSubGet.
  prefDelete() will return TRUE on success, and also if there are no settings
  to delete in the first place.
*/
int prefDelete(){
	return (RegDeleteKey(HKEY_CURRENT_USER, keyName) == ERROR_SUCCESS) || 
		   (GetLastError() == 0);
}