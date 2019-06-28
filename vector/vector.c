#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

//return: initial capacity
int vector_init(vector_t *v, int capacity)
{
	if(capacity<=0)
	{
		v->initial_capacity = VECTOR_INIT_CAPACITY;
	}
	else
	{
		v->initial_capacity = capacity;
	}
	v->capacity=v->initial_capacity;

	v->size = 0;
	v->items = malloc(sizeof(void *) * v->capacity);
	if(v->items==NULL)
	{
		return -1;
	}

#ifdef DEBUG_ON
	printf("initial capacity: %d\n", v->capacity);
#endif

	return v->capacity;
}

//return: size
int vector_size(vector_t *v)
{
	return v->size;
}

//return: capacity
int vector_capacity(vector_t *v)
{
	return v->capacity;
}

//return: new capacity
static int vector_resize(vector_t *v, int capacity)
{
	if(capacity<1){capacity=1;} //minimum
	if(v->capacity==capacity){return v->capacity;}

#ifdef DEBUG_ON
	printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

	void **items = realloc(v->items, sizeof(void *) * capacity);
	if(items)
	{
		v->items = items;
		v->capacity = capacity;
		return v->capacity;
	}
	return -1;
}

//return: new size
int vector_add(vector_t *v, void *item)
{
	if(v->capacity == v->size)
	{
		vector_resize(v, v->capacity * 2);
	}
	v->items[v->size++] = item;
	return v->size;
}

//return: size
int vector_set(vector_t *v, int index, void *item)
{
	if(index >= 0 && index < v->size)
	{
		v->items[index] = item;
		return v->size;
	}
	return -1;
}

//return: item
void *vector_get(vector_t *v, int index)
{
	if(index >= 0 && index < v->size)
	{
		return v->items[index];
	}
	return NULL;
}

//return new size
int vector_delete(vector_t *v, int index)
{
	if(index < 0 || index > v->size-1)
	{
		return -1;
	}

	v->items[index] = NULL;

	int i;
	for(i = index; i < v->size - 1; i++)
	{
		v->items[i] = v->items[i + 1];
		v->items[i + 1] = NULL;
	}

	v->size--;

	if(v->size > 0 && v->size == v->capacity / 4)
	{
		vector_resize(v, v->capacity / 2);
	}

	return v->size;
}

//return new size
int vector_insert(vector_t *v, int index, void *item)
{
	if(index < 0 || index > v->size-1)
	{
		return -1;
	}

	if(v->capacity == v->size)
	{
		vector_resize(v, v->capacity * 2);
	}

	int i;
	for(i = v->size; i > index; i--)
	{
		v->items[i] = v->items[i-1];
	}

	v->items[index] = item;
	v->size++;
	return v->size;
}

int vector_clear(vector_t *v)
{
	int i;
	int size=v->size;
	for(i=0; i<size-1; i++)
	{
		v->items[i]=NULL;
	}
	v->size=0;
	return vector_resize(v, v->initial_capacity);	
}

void vector_free(vector_t *v)
{
	free(v->items);
}
//EOF
