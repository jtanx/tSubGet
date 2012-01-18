#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include "..\\tSubGetLib\\tSubGetLib.h"

#ifdef _DEBUG
#pragma comment (lib, "tSubGetLib.lib")
#else
#pragma comment (lib, "tSubGetLib.lib")
#endif

/*
tsubgetc -d msDelay -c -p pageNum -o fileOut fileIn
tsubgetc [-d msDelay] [-c] [-p pageNum] [-o fileOut] fileIn

*/
