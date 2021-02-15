#include <stdlib.h>
#include <string.h>

#include <clib_internal.h>
#include <iter.h>
#include "list.h"

static void destroy_list_mem(List **ls)
{
	// Free the list also
	memset(*ls, 0, sizeof(List));
	free(*ls);
	*ls = NULL;
}

static struct Node *new_node(List *ls, void *data)
{
	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
	if (node != NULL)
	{
		// allocating space for user data
		node->data =  malloc(ls->elem_size);

		// If malloc error then abort by cleaning
		if (node->data == NULL)
		{
			free(node);
			return NULL;
		}
		memcpy(node->data, data, ls->elem_size);
		node->prev = node->next = NULL;
	}

	return node;
}

static struct Node *full_byte_comp(List *ls, void *needle)
{
	// Iterate over each node and compare full bytes
	for (struct Node *node = (struct Node *)(ls->head); node != NULL; node = node->next)
	{
		// if exact byte match is found then return the user data
		if (memcmp(node->data, needle, ls->elem_size) == 0)
			return node;
	}

	return NULL;
}

static int free_user_data(List *ls, void *data)
{
	if (data == NULL)
		return 0;

	if (ls->destroyer == NULL)
		free(data);
	else
		ls->destroyer(data);

	return 1;
}

static int remove_node(List *ls, struct Node *del_node)
{
	// if prev and next ptr is NULL then del_node is one and only node
	if (del_node->prev == NULL && del_node->next == NULL)
	{
		ls->head = ls->tail = NULL;
	}

	// else if prev ptr is NULL then del_node is first node
	else if (del_node->prev == NULL)
	{
		ls->head = del_node->next;
		((struct Node *)(ls->head))->prev = NULL;
	}

	// else if next ptr is NULL then del_node is last node
	else if (del_node->next == NULL)
	{
		ls->tail = del_node->prev;
		((struct Node *)(ls->tail))->next = NULL;
	}

	// else del_node is middle node
	else
	{
		// prev_node <---> del_ndoe <---> next_node
		del_node->prev->next = del_node->next;
		del_node->next->prev = del_node->prev;
	}

	free_user_data(ls, del_node->data);
	free(del_node);

	return 1;
}

List *list_new(size_t elem_size, void (*destroyer)(void *))
{
	List *ls = (List *)malloc(sizeof(List));
	// If malloc fails then abort
	if (ls == NULL)
		return NULL;

	ls->size 		= 0;
	ls->elem_size 	= elem_size;
	ls->head 		= NULL;
	ls->tail 		= NULL;
	ls->destroyer 	= destroyer;

	return ls;
}

void *list_pushfront(List *ls, void *data)
{
	if (ls == NULL || data == NULL)
		return NULL;

	struct Node *node = new_node(ls, data);
	if (node == NULL)
		return NULL;
	
	struct Node **head = (struct Node **)&ls->head;

	// new node's prev pointer points to last node of the list
	node->next = *head;

	// if head is not null then fist node's prev pointer
	// points to new node
	if (*head)
		(*head)->prev = node;

	// if its a first push then head will point to same node (first node)
	if (ls->head == NULL)
		ls->tail = node;

	// Update stack head, tail and stack meta
	*head = node;
	ls->size++;

	return data;
}

void *list_pushback(List *ls, void *data)
{
	if (ls == NULL || data == NULL)
		return NULL;

	struct Node *node = new_node(ls, data);
	if (node == NULL)
		return NULL;
	
	struct Node **tail = (struct Node **)&ls->tail;

	// new node's prev pointer points to last node of the list
	node->prev = *tail;

	// if tail is not null then last node's next pointer
	// points to new node
	if (*tail)
		(*tail)->next = node;

	// if its a first push then head will point to same node (first node)
	if (ls->head == NULL)
		ls->head = node;

	// Update stack head, tail and stack meta
	*tail = node;
	ls->size++;

	return data;
}

void *list_push(List *ls, void *data)
{
	return list_pushback(ls, data);
}

void *list_popfront(List *ls)
{
	if (ls == NULL) 		return NULL;
	if (ls->head == NULL) 	return NULL;

	// Taking head address and typecasting it to right data type for easy
	// access down below
	struct Node **head = (struct Node **)&ls->head;

	// Store the address of original user data
	void *data = (*head)->data;

	// Store deleting node's address
	struct Node *del = *head;

	// Update head pointer (head will point to next node if present else NULL)
	*head = (*head)->next;
	if (*head)
		(*head)->prev = NULL;
	// current deleting node is last node therefore tail will be NULL
	else
		ls->tail = NULL;

	// Update list meta
	ls->size--;

	// Free the node (front node)
	free(del);

	return data;
}

void *list_popback(List *ls)
{
	if (ls == NULL) 		return NULL;
	if (ls->head == NULL) 	return NULL;

	// Taking tail address and typecasting it to right data type for easy
	// access down below
	struct Node **tail = (struct Node **)&ls->tail;

	// Store the address of original user data
	void *data = (*tail)->data;

	// Store deleting node's address
	struct Node *del = *tail;

	// Update tail pointer (tail will point to prev node if present else NULL)
	*tail = (*tail)->prev;
	if (*tail)
		(*tail)->next = NULL;
	// current deleting node is last node therefore head will be NULL
	else
		ls->head = NULL;

	// Update list meta
	ls->size--;

	// Free the node (front node)
	free(del);

	return data;
}

void *list_pop(List *ls)
{
	return list_popback(ls);
}

void *list_front(List *ls)
{
	if (ls == NULL) 		return NULL;
	if (ls->head == NULL) 	return NULL;

	return ((struct Node *)(ls->head))->data;
}

void *list_back(List *ls)
{
	if (ls == NULL) 		return NULL;
	if (ls->tail == NULL) 	return NULL;

	return ((struct Node *)(ls->tail))->data;
}

void *list_search(List *ls, void *needle, int (*search_callback)(void *data, void *needle))
{
	if (ls == NULL)
		return NULL;

	// if no callback is passed then do full byte level search on user data
	if (search_callback == NULL)
	{
		struct Node *result = full_byte_comp(ls, needle);
		return result ? result->data : NULL;
	}

	// Iterate over each node and call the callback
	for (struct Node *node = (struct Node *)(ls->head); node != NULL; node = node->next)
	{
		if (search_callback(node->data, needle) == 0)
			return node->data;
	}

	return NULL;
}

int list_destroy(List **ls)
{
	if (*ls == NULL)
		return 0;

	// if no elements are present then destroy the list it self
	if ((*ls)->head == NULL)
	{
		destroy_list_mem(ls);
		return 0;
	}

	unsigned int count = 0;
	struct Node *next = NULL;
	for (struct Node *node = (struct Node *)(*ls)->head; node != NULL; node = next)
	{
		if ((*ls)->destroyer)
			(*ls)->destroyer(node->data);
		else
			free(node->data);

		next = node->next;
		free(node);
		count++;
	}

	destroy_list_mem(ls);

	return count;
}

int list_isempty(List *ls)
{
	if (ls == NULL)
		return 1;

	return ls->size == 0;
}

int list_remove(List *ls, void *needle, int (*search_callback)(void *data, void *needle))
{
	const int DELETED = 1, NOT_DELETED = 0;
	void *data;

	if (ls == NULL) 		return NOT_DELETED;
	if (ls->head == NULL)	return NOT_DELETED;

	// TODO: BUG : this code segment will only remove first matching node
	// remove all mathiing node
	if (search_callback == NULL)
	{
		struct Node *result = full_byte_comp(ls, needle);
		if (result == NULL)
			return NOT_DELETED;

		// removes the node and frees user data also
		remove_node(ls, result);
		ls->size--;
		return DELETED;
	}

	int flag = NOT_DELETED;
	// Iterate over each node remove the first matched node
	for (struct Node *node = (struct Node *)(ls->head); node != NULL; node = node->next)
	{
		if (search_callback(node->data, needle) == 0)
		{
			// removes the node and frees user data also
			if (remove_node(ls, node))
			{
				flag = DELETED;
				ls->size--;
			}
		}
	}

	return flag;
}

void list_reverse(List *ls)
{
	if (ls == NULL)			return;
	if (ls->head == NULL) 	return;

	struct Node *node = ls->head;
	ls->head = ls->tail;
	ls->tail = node;

	struct Node *tmp = NULL;
	for (node = ls->head; node != NULL; node = node->next)
	{
		// swap prev with next
		tmp = node->prev;
		node->prev = node->next;
		node->next = tmp;

		// NOTE: since prev became next and vice versa, therfore in
		// updation segment of for loop must be node = node->next
		// because node->next is now pointing to node (n - 1)
	}
}

void  *list_getiter(List *ls)
{
	if (ls == NULL)
		return NULL;

	Iter *itr = iter_new();
	if (itr)
		itr->node = ls->head;

	return itr;
}
