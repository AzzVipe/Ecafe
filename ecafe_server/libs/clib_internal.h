#ifndef _CLIB_INTERNAL_H
#define _CLIB_INTERNAL_H

struct Node {
	void *data;
	struct Node *prev, *next;
};

#endif
