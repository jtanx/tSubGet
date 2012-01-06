/* Header file for QueueBase */
#include <memory.h>

typedef struct QueueItem{
	struct QueueItem *prev;
	struct QueueItem *next;
	void *data;
} QueueItem;

struct Queue{
	QueueItem *head;
	QueueItem *tail;
	size_t unitDataSize;
};