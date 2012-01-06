#include "BaseLib.h"
#include "QueueBase.h"

Queue qbCreate(size_t unitSize){
	Queue qb;

	qb = calloc(1, sizeof(struct Queue));
	if (!qb) return NULL;

	qb->unitDataSize = unitSize;
	return qb;
}

int qbAdd(Queue qb, int toFront, void **pData){
	QueueItem *qi;
	if (!qb || !pData)
		return FALSE;

	*pData = NULL;
	qi = calloc(1,sizeof(QueueItem));
	if (!qi)
		return FALSE;
	qi->data = calloc(1, qb->unitDataSize);
	if (!qi->data){
		free(qi);
		return FALSE;
	}
	
	if (qb->head == NULL){
		qb->head = qi;
		qb->tail = qb->head;
	} else if (toFront){
		qb->head->prev = qi;
		qi->next = qb->head;
		qb->head = qi;
	} else{
		qb->tail->next = qi;
		qi->prev = qb->tail;
		qb->tail = qi;
	}

	*pData = qi->data;
	return TRUE;
}

int qbPeek(Queue qb, int n, int fromFront, void **pData){
	int i;
	QueueItem *qi;
	if (!qb) return FALSE;

	qi = fromFront ? qb->head : qb->tail;
	for (i = 0; i < n && qi; i++)
		qi = fromFront ? qi->next : qi->prev;

	if (i != n || !qi){
		if (pData)
			*pData = NULL;
		return FALSE;
	}

	//Only set if asked for.
	if (pData)
		*pData = qi->data;
	return TRUE;
}

int qbFreeSingle(Queue qb, int fromFront){
	QueueItem *qbOld, *qbNew;
	if (!qb || !qb->head) return FALSE;

	if (fromFront){
		qbOld = qb->head;
		qbNew = qbOld->next;
	} else{
		qbOld = qb->tail;
		qbNew = qbOld->prev;
	}

	if (!qbNew){
		qb->head = NULL;
		qb->tail = NULL;
	} else{
		if (fromFront){
			qbNew->prev = NULL;
			qb->head = qbNew;
		} else{
			qbNew->next = NULL;
			qb->tail = qbNew;
		}
	}
	
	free(qbOld->data);
	free(qbOld);
	return TRUE;
}

void qbClose(Queue *qb){
	if (!qb) return;
	while (qbFreeSingle(*qb, TRUE));
	free(*qb);
	*qb = NULL;
}