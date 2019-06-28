#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vector.h"

int main(void)
{
	//initial capacity=5
	VEC_NEW(v5, 5);

	assert(VEC_SIZE(v5)==0);
	assert(VEC_CAPACITY(v5)==5);

	//default initial capacity
	VEC_NEW(v, 0);
	assert(VEC_SIZE(v)==0);
	assert(VEC_CAPACITY(v)==VECTOR_INIT_CAPACITY);

	VEC_ADD(v, "A");
	VEC_ADD(v, "B");
	VEC_ADD(v, "C");
	VEC_ADD(v, "D");
	assert(VEC_SIZE(v)==4);
	assert(VEC_CAPACITY(v)==4);

	VEC_ADD(v, "E");
	assert(VEC_SIZE(v)==5);
	assert(VEC_CAPACITY(v)==8); //after resize
	//content: ABCDE

	VEC_DEL(v, 0);
	assert(VEC_SIZE(v)==4);
	assert(VEC_CAPACITY(v)==8);
	//content: BCDE

	VEC_DEL(v, 0);
	assert(VEC_SIZE(v)==3);
	assert(VEC_CAPACITY(v)==8);
	//content: CDE

	VEC_DEL(v, 0);
	assert(VEC_SIZE(v)==2);
	assert(VEC_CAPACITY(v)==4); //after resize
	//content: DE

	int i;
	for(i=0;i<7;i++)
	{
		VEC_ADD(v, "C"); //append
	}
	assert(VEC_SIZE(v)==9);
	assert(VEC_CAPACITY(v)==16); //after resize
	//content: DECCCCCCC

	for(i=0;i<4;i++)
	{
		VEC_INS(v, 0, "X"); //prepend
	}
	assert(VEC_SIZE(v)==13);
	assert(VEC_CAPACITY(v)==16);
	//content: XXXXDECCCCCCC

	assert(VEC_CLEAR(v5)==5);
	assert(VEC_SIZE(v)==13); //independent v

	VEC_SET(v, 1, "Y"); //replace
	assert(VEC_SIZE(v)==13);
	assert(VEC_CAPACITY(v)==16);
	//content: XYXXDECCCCCCC

	VEC_DEL(v, 4); //remove
	assert(VEC_SIZE(v)==12);
	assert(VEC_CAPACITY(v)==16);
	//content: XYXXECCCCCCC

	//print
	for (i = 0; i < VEC_SIZE(v); i++)
	{
		printf("%s ", VEC_GET(v, char*, i));
	}
	printf("\n");

	printf("X Y X X E C C C C C C C\n");

	//try out of bounds
	assert(VEC_SET(v, -1, "N")==-1);
	assert(VEC_SET(v, VEC_SIZE(v), "N")==-1);
	assert(VEC_DEL(v, -1)==-1);
	assert(VEC_DEL(v, VEC_SIZE(v))==-1);

	VEC_CLEAR(v);
	assert(VEC_SIZE(v)==0);
	assert(VEC_CAPACITY(v)==VECTOR_INIT_CAPACITY); //after resize

	VEC_CLEAR(v5);
	assert(VEC_SIZE(v5)==0);
	assert(VEC_CAPACITY(v5)==5);

	VEC_FREE(v);
	VEC_FREE(v5);
}
//EOF
