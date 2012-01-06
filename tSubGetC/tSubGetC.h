#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include "..\\tSubGetLib\\tSubGetLib.h"

#ifdef _DEBUG
#pragma comment (lib, "..\\Bin\\Debug\\tSubGetLib.lib")
#else
#pragma comment (lib, "..\\Bin\\tSubGetLib.lib")
#endif
#pragma comment (lib, "Strmiids.lib")

/*
tsubgetc -d msDelay -c -p pageNum -o fileOut fileIn
tsubgetc [-d msDelay] [-c] [-p pageNum] [-o fileOut] fileIn

*/
