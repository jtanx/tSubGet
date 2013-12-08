#include "tSubTesting.h"

static void exitPause(void) {
	system("pause");
}

void fmtTsgMsg(HRESULT hr, int line) {
	wchar_t buf[BUFSIZ];
	TSG_GetErrorString(hr, buf, BUFSIZ); 
	wprintf(L"Error at line %d: 0x%08X/%ld: %s\n", line, hr, hr, buf);
}

int wmain(int argc, wchar_t *argv[]) {
	atexit(exitPause);
	//return NullGrabberTest(argc, argv);
	return tsglV2Test(argc, argv);
}