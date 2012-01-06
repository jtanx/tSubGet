/* Header file for StringBase */
struct StringBase {
	wchar_t *buffer;
	int bufferSize;
	int incrementSize;
	int currentPosition;
};

