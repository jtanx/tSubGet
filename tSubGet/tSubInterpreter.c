#include "tSubGet.h"
#define QUERY_NUM	0xFFFFFFFF

__inline void getNames(Details *uDets){
	wchar_t *ptr;
	if (!uDets->pi.forceOutputDir){
		wcsncpy_s(uDets->pi.outDir,MAX_PATH,uDets->pi.filePath,_TRUNCATE);
		ptr = wcsrchr(uDets->pi.outDir,L'\\');
		if (ptr) *ptr = L'\0';
	}
	ptr = wcsrchr(uDets->pi.filePath,L'\\');
	if (ptr) ptr++;
	else ptr = uDets->pi.filePath;
	
	wcsncpy_s(uDets->po.dispFilename,MAX_PATH,ptr,_TRUNCATE);
	_snwprintf_s(uDets->po.outFile,MAX_PATH,_TRUNCATE,L"%s\\%s",
				uDets->pi.outDir,uDets->po.dispFilename);
	ptr = wcsrchr(uDets->po.outFile,L'.');
	if (ptr) *ptr = L'\0';
	wcsncat_s(uDets->po.outFile,MAX_PATH,L".srt",_TRUNCATE);
}

unsigned WINAPI processInput(void *args){
	Details *uDets = (Details*)args;
	unsigned i, retVal = TRUE;

	CoInitialize(NULL);
	uDets->pi.fileCount = DragQueryFile(uDets->gui.hDrop,QUERY_NUM,NULL,0);
	for (i = 0; i < uDets->pi.fileCount; i++){
		wchar_t buffer[1024];

		if (WaitForSingleObject(uDets->gui.hEvent,0) == WAIT_OBJECT_0){
			SetWindowText(uDets->gui.hwndStatus,L"Process aborted.");
			retVal = FALSE; break;
		}
		if (!DragQueryFile(uDets->gui.hDrop,i,uDets->pi.filePath,MAX_PATH)){
			SetWindowText(uDets->gui.hwndStatus,L"DragQueryFile failed!");
			retVal = FALSE; break;
		}

		getNames(uDets);
		_snwprintf_s(buffer,1024,_TRUNCATE,L"(%d/%d) Processing: %s",
						i+1,uDets->pi.fileCount,uDets->po.dispFilename);
		SetWindowText(uDets->gui.hwndStatus,buffer);

		if (!initialiseDecoder(&uDets->po.d,uDets->pi.ttPageNumber,uDets->pi.lang)){
			SetWindowText(uDets->gui.hwndStatus,L"Error: Could not initialise decoder!");
			retVal = FALSE; break;
		}
		if (!parseFile(uDets)){
			_snwprintf_s(buffer,1024,_TRUNCATE,L"(%d/%d) Failed to process: %s",
							i+1,uDets->pi.fileCount,uDets->po.dispFilename);
			SetWindowText(uDets->gui.hwndStatus,buffer);
			retVal = FALSE; break;
		}

		finaliseDecoder(&uDets->po.d);
		if (uDets->po.d.meta.capIdx == 0){
			_snwprintf_s(buffer,1024,_TRUNCATE,L"(%d/%d) No captions found: %s",
							i+1,uDets->pi.fileCount,uDets->po.dispFilename);
			SetWindowText(uDets->gui.hwndStatus,buffer);
			Sleep(1000);
			freeDecoder(&uDets->po.d);
			continue;
		}

		if (!uDets->pi.noPrompt && PathFileExists(uDets->po.outFile)){
			int ret;
			_snwprintf_s(buffer,1024,_TRUNCATE,L"Warning: File %s exists, overwrite?",uDets->po.outFile);
			ret = MessageBox(uDets->gui.hwndMain,buffer,L"File exists",MB_YESNO|MB_ICONQUESTION);
			if (ret != IDYES){
				freeDecoder(&uDets->po.d);
				continue;
			}
		}
		writeOutSubs(uDets);
		freeDecoder(&uDets->po.d);
	}//End for loop

	CoUninitialize();
	PostMessage(uDets->gui.hwndMain,WM_COMMAND,ID_FINISH,0);
	return retVal;
}