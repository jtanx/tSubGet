#include "tsubGet.h"

int wmain(int argc, wchar_t *argv[]){
	Decoder d;
	int i;

	CoInitialize(NULL);
	if (argc < 2){
		wprintf (L"tSubGet Version 0.2\n");
		wprintf (L"Usage: %s file1.dvr-ms [file2.dvr-ms...]\n",argv[0]);
		return 1;
	}

	for (i = 1; i < argc; i++){
		wchar_t subFilename[MAX_PATH];
		int lang[2] = {0};

		wprintf (L"Processing file: %s\n",argv[i]);
		if (!initialiseDecoder(&d,001,lang) || !parseFile(&d,argv[i])){
			wprintf (L"Error: Could not parse file: %s\n", argv[i]);
			wprintf (L"Check that the file is a supported type, and that it has\n");
			wprintf (L"a subtitle stream.\n");
			return 1;
		}

		if (d.capCount == 0)
			wprintf (L"Warning: No captions detected for %s\n",argv[i]);
		else{
			getSubFilename(argv[i],subFilename,MAX_PATH);
			
			wprintf (L"Successfully parsed file. Now generating %s\n",subFilename);
			if (!writeOutSubs(d,subFilename)){
				wprintf (L"Error: Could not write out to %s\n",subFilename);
				return 1;
			}
			wprintf (L"Successfully wrote out captions file.\n\n");
		}
		finaliseDecoder(&d);
	}

	CoUninitialize();
	return 0;
}