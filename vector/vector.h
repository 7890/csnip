#ifndef _VECTOR_H_
#define _VECTOR_H_

#define VECTOR_INIT_CAPACITY 4

#define VEC_NEW(vec, init_capacity) vector_t vec; vector_init(&vec, init_capacity)
#define VEC_ADD(vec, item) vector_add(&vec, (void *) item)
#define VEC_SET(vec, index, item) vector_set(&vec, index, (void *) item)
#define VEC_GET(vec, type, index) (type) vector_get(&vec, index)
#define VEC_DEL(vec, index) vector_delete(&vec, index)
#define VEC_INS(vec, index, item) vector_insert(&vec, index, (void *) item)

#define VEC_SIZE(vec) vector_size(&vec)
#define VEC_CAPACITY(vec) vector_capacity(&vec)
#define VEC_CLEAR(vec) vector_clear(&vec)
#define VEC_FREE(vec) vector_free(&vec)

typedef struct
{
	void **items;
	int initial_capacity;
	int capacity;
	int size;
} vector_t;

int vector_init(vector_t *, int);
int vector_size(vector_t *);
int vector_capacity(vector_t *);
int vector_add(vector_t *, void *);
int vector_set(vector_t *, int, void *);
void *vector_get(vector_t *, int);
int vector_delete(vector_t *, int);
int vector_insert(vector_t *v, int index, void *);
int vector_clear(vector_t *);
void vector_free(vector_t *);

#endif //_VECTOR_H_
//EOF
