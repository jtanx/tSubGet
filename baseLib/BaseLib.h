/* BaseLib - A base library for some custom written functions */
#ifndef _BASELIB_H_
#define BASELIB_H_
#include <stdlib.h>

#ifndef FALSE
	#define FALSE	0
	#define TRUE	1
#endif

/* QueueBase - A relatively simple queue implementation */
typedef struct Queue *Queue;

extern Queue qbCreate(size_t unitSize);
extern int qbAdd(Queue qb, int toFront, void **pData);
extern int qbPeek(Queue qb, int n, int fromFront, void **pData);
extern int qbFreeSingle(Queue qb, int fromFront);
extern void qbClose(Queue *qb);
/* End QueueBase Declarations */

/* StringBase - A very simple string builder (UTF-8) */
typedef struct StringBase *StringBase;

extern StringBase sbCreate(int initialSize, int incrementSize);
extern void sbFree(StringBase sb);
extern int sbAddUC(StringBase sb, unsigned __int32 codePoint);
extern char *sbGetString(StringBase sb);
extern int sbGetCharCount(StringBase sb);
/* End StringBase Declarations */

#endif