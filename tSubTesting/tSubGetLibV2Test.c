#include "tSubTesting.h"

static void progress(__int64 num, __int64 den, void *userData) {
	printf ("%3d%%\r", (100 * num)/den);
}

int tsglV2Test(int argc, wchar_t *argv[]) {
	TSG_Parser *parser;
	HRESULT hr;

	if (argc != 2) {
		wprintf(L"Usage: tsglV2Test[%s] file\n", argv[0]);
		return 1;
	}

	CoInitialize(NULL);
	hr = TSG_Init(argv[1], 0x801, LANGID_DEFAULT, &parser);
	if (FAILED(hr)) {
		ERRORMSG(hr);
		return 1;
	}

	TSG_SetProgressCallback(parser, progress, parser);
	hr = TSG_Run(parser);
	if (FAILED(hr)) {
		ERRORMSG(hr);
		return 1;
	}

	TSG_Free(&parser);
	return 0;
}