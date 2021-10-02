#include <stdlib.h>
#include <clib_internal.h> /* for struct Node */
#include "iter.h"

Iter *iter_new(void)
{
	Iter *iter = (Iter *)malloc(sizeof(Iter));
	if (iter)
		iter->signature = 0xabcdef;

	return iter;
}

void *iter_next(Iter *iter)
{
	if (iter == NULL)		return NULL;
	if (iter->node == NULL)	return NULL;

	void *data = ((struct Node *)(iter->node))->data;
	iter->node = ((struct Node *)(iter->node))->next;

	return data;
}

void iter_end(Iter **iter)
{
	free(*iter);
	*iter = NULL;
}
