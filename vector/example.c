#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vector.h"

int main(void)
{
	//initial capacity=5
	VECTOR_INIT(v5, 5);

	assert(VECTOR_SIZE(v5)==0);
	assert(VECTOR_CAPACITY(v5)==5);

	//default initial capacity
	VECTOR_INIT(v, 0);
	assert(VECTOR_SIZE(v)==0);
	assert(VECTOR_CAPACITY(v)==VECTOR_INIT_CAPACITY);

	VECTOR_ADD(v, "A");
	VECTOR_ADD(v, "B");
	VECTOR_ADD(v, "C");
	VECTOR_ADD(v, "D");

	assert(VECTOR_SIZE(v)==4);
	assert(VECTOR_CAPACITY(v)==4);

	VECTOR_ADD(v, "E");

	assert(VECTOR_SIZE(v)==5);
	assert(VECTOR_CAPACITY(v)==8); //after resize
	//content: ABCDE

	int i;
	for(i=0;i<4;i++)
	{
		VECTOR_ADD(v, "C"); //append
	}
	assert(VECTOR_SIZE(v)==9);
	assert(VECTOR_CAPACITY(v)==16); //after resize
	//content: ABCDECCCC

	for(i=0;i<4;i++)
	{
		VECTOR_INSERT(v, 0, "X"); //prepend
	}
	assert(VECTOR_SIZE(v)==13);
	assert(VECTOR_CAPACITY(v)==16);
	//content: XXXXABCDECCCC

	VECTOR_SET(v, 1, "Y"); //replace

	assert(VECTOR_SIZE(v)==13);
	assert(VECTOR_CAPACITY(v)==16);
	//content: XYXXABCDECCCC

	VECTOR_DELETE(v, 4); //remove
	assert(VECTOR_SIZE(v)==12);
	assert(VECTOR_CAPACITY(v)==16);
	//content: XYXXBCDECCCC

	//print
	for (i = 0; i < VECTOR_SIZE(v); i++)
	{
		printf("%s ", VECTOR_GET(v, char*, i));
	}
	printf("\n");

	printf("X Y X X B C D E C C C C\n");

	//try out of bounds
	assert(VECTOR_SET(v, -1, "N")==-1);
	assert(VECTOR_SET(v, VECTOR_SIZE(v), "N")==-1);

	VECTOR_CLEAR(v);

	assert(VECTOR_SIZE(v)==0);
	assert(VECTOR_CAPACITY(v)==VECTOR_INIT_CAPACITY); //after resize

	VECTOR_CLEAR(v5);
	assert(VECTOR_SIZE(v5)==0);
	assert(VECTOR_CAPACITY(v5)==5);
}
//EOF
