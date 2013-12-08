#include "tSubInternalV2.h"

struct ArrayBuilder {
	char *buffer;
	size_t nUsed, nTotal;
	size_t unitSize;
};

/*
 * Initialises an array builder. If the initial count is zero,
 * then an initial count of 10 is set.
 */
BOOL ab_init(ArrayBuilder **ab, size_t initialCount, size_t unitSize)
{
	ArrayBuilder *abt;
	if (ab == NULL || unitSize < 1)
		return FALSE;
	else if ((abt = calloc(1, sizeof(ArrayBuilder))) == NULL)
		return FALSE;

	if(initialCount < 1) //Give default size
		initialCount = 10;
	
	abt->buffer = malloc(initialCount * unitSize);
	if (abt->buffer != NULL) {
		abt->nTotal = initialCount;
		abt->unitSize = unitSize;
		*ab = abt;
		return TRUE;
	}
	free(abt);
	return FALSE;
}

/*
 * Adds an item to an array builder.
 */
BOOL ab_add(ArrayBuilder *ab, void *data)
{
	if (ab == NULL)
		return FALSE;
	else if (ab->nUsed == ab->nTotal) //Must realloc
	{
		int nTotal = ab->nTotal * 2;
		void *buffer = realloc(ab->buffer, nTotal * ab->unitSize);
		if (buffer == NULL)
			return FALSE;
		ab->buffer = buffer;
		ab->nTotal = nTotal;
	}

	memcpy(ab->buffer + ab->nUsed * ab->unitSize, data, ab->unitSize);
	ab->nUsed++;
	return TRUE;
}

BOOL ab_add_multiple(ArrayBuilder *ab, void *data, size_t count)
{
	if (ab == NULL || count < 1)
		return FALSE;
	else if ((ab->nUsed + count) >= ab->nTotal) //Must realloc
	{
		int nTotal = ab->nTotal * 2 + count; //ok??
		void *buffer = realloc(ab->buffer, nTotal * ab->unitSize);
		if (buffer == NULL)
			return FALSE;
		ab->buffer = buffer;
		ab->nTotal = nTotal;
	}

	memcpy(ab->buffer + ab->nUsed * ab->unitSize, data, ab->unitSize * count);
	ab->nUsed += count;
	return TRUE;
}

/*
 * Retrieves the number of items currently in the array builder.
 */
size_t ab_length(ArrayBuilder *ab) 
{
	if (ab == NULL)
		return -1;
	return ab->nUsed;
}

/*
 * Finalises the array builder - the buffer is resized to the
 * exact size needed and returned to the caller. The associated
 * array builder structure is freed. The caller should free
 * the returned buffer.
 */
void *ab_finalise(ArrayBuilder **ab, size_t *count)
{
	void *buffer;
	if (ab == NULL || *ab == NULL || count == NULL)
		return NULL;
	
	*count = (*ab)->nUsed;
	//Realloc to exact size
	buffer = realloc((*ab)->buffer, (*ab)->nUsed * (*ab)->unitSize);
	if (buffer == NULL && (*ab)->nUsed > 0)
		buffer = (*ab)->buffer;
	free(*ab);
	*ab = NULL;

	return buffer;
}

/*****************************StringBuilder**************************/

BOOL sb_init(StringBuilder **sb, size_t initialCount) 
{
	return ab_init(sb, initialCount, sizeof(char));
}

/*
 * Encodes a Unicode code point as UTF-8.
 */
BOOL sb_add(StringBuilder *sb, unsigned __int32 codePoint)
{
	unsigned char buf[6];
	int byteCount = 0;
	if (!sb)
		return FALSE;
	
	if (codePoint <= 0x7F){
		buf[0] = codePoint;
		return ab_add(sb, buf);
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

	return ab_add_multiple(sb, buf, byteCount);
}

char *sb_peek(StringBuilder *sb)
{
	if (!sb_add(sb, 0)) { //Add null character
		return NULL;
	}
	
	sb->nUsed--; //Move back to overwrite null char if more is added.
	return sb->buffer;
}

size_t sb_length(StringBuilder *sb) 
{
	return ab_length(sb);
}

void sb_free(StringBuilder **sb) 
{
	if (sb != NULL && *sb != NULL) {
		free((*sb)->buffer);
		free(*sb);
		*sb = NULL;
	}
}