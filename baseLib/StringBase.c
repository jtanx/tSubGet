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
	sb->buffer = malloc(sb->bufferSize * sizeof(char));
	if (!sb->buffer){
		free(sb);
		return NULL;
	}

	return sb;
}

static int sbRealloc(StringBase sb){
	char *buf = sb->buffer;
	int bufSizeRealloc = sb->bufferSize + sb->incrementSize;
	
	buf = realloc(buf, bufSizeRealloc * sizeof(char));
	if (!buf)
		return FALSE;

	sb->buffer = buf;
	sb->bufferSize = bufSizeRealloc;
	return TRUE;
}

static int addElement(StringBase sb, char c){
	if (sb->bufferSize == sb->currentPosition && !sbRealloc(sb))
		return FALSE;
	sb->buffer[sb->currentPosition++] = c;
	return TRUE;
}

/*
 * Encodes a Unicode code point as UTF-8.
 */
int sbAddUC(StringBase sb, unsigned __int32 codePoint){
	unsigned char buf[6];
	int byteCount = 0, i;
	if (!sb)
		return FALSE;
	
	if (codePoint <= 0x7F){
		return addElement(sb, (char)codePoint);
	} else if (codePoint <= 0x7FF){
		buf[0] = 0xC0 | (codePoint >> 6);
		buf[1] = 0x80 | (codePoint & 0x3F);
		byteCount = 2;
	} else if (codePoint <= 0xFFFF){
		buf[0] = 0xE0 | (codePoint >> 12);
		buf[1] = 0x80 | ((codePoint >> 6) & 0x3F);
		buf[2] = 0x80 | (codePoint & 0x3F);
		byteCount = 3;
	} else if (codePoint <= 0x1FFFFF){
		buf[0] = 0xF0 | (codePoint >> 18);
		buf[1] = 0x80 | ((codePoint >> 12) & 0x3F);
		buf[2] = 0x80 | ((codePoint >> 6) & 0x3F);
		buf[3] = 0x80 | (codePoint & 0x3F);
		byteCount = 4;
	} else if (codePoint <= 0x3FFFFFF){
		buf[0] = 0xF8 | (codePoint >> 24);
		buf[1] = 0x80 | ((codePoint >> 18) & 0x3F);
		buf[2] = 0x80 | ((codePoint >> 12) & 0x3F);
		buf[3] = 0x80 | ((codePoint >> 6) & 0x3F);
		buf[4] = 0x80 | (codePoint & 0x3F);
		byteCount = 5;
	} else {
		buf[0] = 0xFC | (codePoint >> 30);
		buf[1] = 0x80 | ((codePoint >> 24) & 0x3F);
		buf[2] = 0x80 | ((codePoint >> 18) & 0x3F);
		buf[3] = 0x80 | ((codePoint >> 12) & 0x3F);
		buf[4] = 0x80 | ((codePoint >> 6) & 0x3F);
		buf[5] = 0x80 | (codePoint & 0x3F);
		byteCount = 6;
	}

	for (i = 0; i < byteCount; i++){
		if (!addElement(sb, buf[i]))
			return FALSE;
	}

	return TRUE;
}

char *sbGetString(StringBase sb){
	if (!sbAddUC(sb,'\0')) return NULL;
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