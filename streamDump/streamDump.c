#include "streamDump.h"
#include <uuids.h>

int wmain(int argc, wchar_t *argv[]){
	StreamInfo si = {0};
	int i, ret;

	if (argc < 4){
		wprintf(L"streamDump version 0.1\n");
		wprintf(L"Usage: %s [-s] [-e] [-g (GUID Major)] -o file_out file_in\n\n",argv[0]);
		wprintf(L"[-s]: Adds the starting timestamp to each sample\n");
		wprintf(L"[-e]: Adds the ending timestamp to each sample\n");
		wprintf(L"[-g (GUID Major)]: Causes streamDump to work with the first stream that\n\t\t\tsatisfies this GUID as its major type.\n");
		wprintf(L"If no GUID is specified, streamDump will connect to the first stream that it encounters.\n\n");
		wprintf(L"Example:\n");
		wprintf(L"%s -s -e -g B88B8A89-B049-4C80-ADCF-5898985E22C1 -o out.dat file.dvr-ms\n",argv[0]);
		wprintf(L"In the above example,the GUID corresponds to MEDIATYPE_MSTVCaption\n(often associated with teletext data.\n");
		return 1;
	}

	CoInitialize(NULL);
	for (i = 1; i < argc; i++){
		if (!_wcsicmp(L"-s",argv[i]))
			si.outStart = TRUE;
		else if (!_wcsicmp(L"-e",argv[i]))
			si.outEnd = TRUE;
		else if (!_wcsicmp(L"-g",argv[i])){
			if (++i < argc){
				RPC_STATUS status;
				status = UuidFromString(argv[i],&si.dsg.am.majortype);
				if (status != RPC_S_OK){
					wprintf(L"Error: Invalid GUID '%s'\n",argv[i]);
					return 1;
				}
			} else{
				wprintf(L"Error: No GUID specified!\n");
				return 1;
			}
		}
		else if (!_wcsicmp(L"-o",argv[i])){
			if (++i < argc){
				if (!_wfopen_s(&si.fp,argv[i],L"r")){
					fclose(si.fp);
					wprintf (L"Error: File '%s' exists!\n",argv[i]);
					return 1;
				}
				if (_wfopen_s(&si.fp,argv[i],L"wb")){
					wprintf (L"Error: Could not open file: '%s' for writing!\n");
					return 1;
				}
			} else{
				wprintf(L"Error: Output flag specified but no output file given!\n");
				return 1;
		}
		}
		else{
			wcsncpy_s(si.inFile,MAX_PATH,argv[i],_TRUNCATE);
		}
	} //end for loop
	
	if (!si.fp){
		wprintf (L"Error: No output file given!\n");
		return 1;
	} 

	wprintf (L"Now processing '%s'...\n\n",si.inFile);
	ret = parseFile(&si);
	fclose(si.fp);
	if (!ret){
		wprintf (L"Error: Could not parse file '%s'!\n",si.inFile);
		wprintf (L"Ensure that you have installed the NullGrabber filter.\n");
		return 1;
	}

	wprintf (L"Processing complete.\n");
	CoUninitialize();
	return 0;
}