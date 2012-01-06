#include "BaseLib.h"
#include "StringBase.h"

StringBase sbCreate(int initialSize, int incrementSize){
	StringBase sb;
	if (initialSize < 1 || incrementSize == 0)
		return NULL;
	
	sb = calloc(1,sizeof(struct StringBase));
	if (!sb) return NULL;

	sb->bufferSize = initialSize;
	sb->incrementSize = incrementSize < 0 ? (initialSize/2 + 1) : incrementSize;
	sb->buffer = malloc(sb->bufferSize * sizeof(wchar_t));
	if (!sb->buffer){
		free(sb);
		return NULL;
	}

	return sb;
}

static int sbRealloc(StringBase sb){
	wchar_t *buf = sb->buffer;
	int bufSizeRealloc = sb->bufferSize + sb->incrementSize;
	
	buf = realloc(buf, bufSizeRealloc * sizeof(wchar_t));
	if (!buf)
		return FALSE;

	sb->buffer = buf;
	sb->bufferSize = bufSizeRealloc;
	return TRUE;
}

int sbAddChar(StringBase sb, wchar_t c){
	if (!sb)
		return FALSE;
	else if (sb->bufferSize == sb->currentPosition && !sbRealloc(sb))
		return FALSE;
	sb->buffer[sb->currentPosition++] = c;
	return TRUE;
}

int sbAddString(StringBase sb, wchar_t *s){
	while (*s != L'\0'){
		if (!sbAddChar(sb,*s++))
			return FALSE;
	}
	return TRUE;
}

wchar_t *sbGetString(StringBase sb){
	if (!sbAddChar(sb,L'\0')) return NULL;
	else sb->currentPosition--;

	return sb->buffer;
}

int sbGetCharCount(StringBase sb){
	return sb->currentPosition;
}

void sbFree(StringBase sb){
	if (!sb) return;
	free(sb->buffer);
	free(sb);
}