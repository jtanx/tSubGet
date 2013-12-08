#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dshow.h>
#include "..\\NullGrabber\\NullGrabberC.h"
#include "..\\tSubGetLib\\tSubGetLibV2.h"

#define ERRORMSG(hr) fmtTsgMsg(hr, __LINE__)

int NullGrabberTest(int argc, wchar_t *argv[]);
int tsglV2Test(int argc, wchar_t *argv[]);
void fmtTsgMsg(HRESULT hr, int line);
