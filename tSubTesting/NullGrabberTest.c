#include "tSubTesting.h"

static int error(HRESULT hr, NGC_Info *info, wchar_t *buf, size_t bufsize, int line) {
	TSG_GetErrorString(hr, buf, bufsize);
	wprintf(L"Error at line %d: (0x%08lX / %ld): %s\n", line, hr, hr, buf);
	NGC_Free(&info);
	CoUninitialize();
	return 1;
}

static HRESULT callback(unsigned char *buffer, long length, __int64 timestamp, void *userData) {
	if (buffer == NULL) {
		printf ("\nFinished!\n");
	} else {
		printf ("%08X: %ld, %lld\n", buffer, length, timestamp);
	}

	return 0x80040013;
}

static void progress(__int64 num, __int64 den, void *userData) {
	printf ("%3d%%\r", (100 * num)/den);
}

int NullGrabberTest(int argc, wchar_t *argv[]) {
	NGC_Info *info = NULL;
	HRESULT hr;
	AM_MEDIA_TYPE mtAccept = {0};
	wchar_t buf[BUFSIZ];
	
	if (argc != 2) {
		printf ("Usage: %s [file-in]\n", argv[0]);
		return 1;
	}
	
	mtAccept.majortype = MEDIATYPE_MSTVCaption;

	CoInitialize(NULL);
	hr = NGC_Init(argv[1], &mtAccept, &info);
	if (FAILED(hr)) {
		return error(hr, NULL, buf, BUFSIZ, __LINE__);
	}

	NGC_SetSampleCallback(info, callback, info);
	NGC_SetProgressCallback(info, progress, NULL);
	if (FAILED((hr = NGC_Run(info)))) {
		return error(hr, info, buf, BUFSIZ, __LINE__);
	}

	NGC_Free(&info);
	CoUninitialize();
	return 0;
}