#include "rb.h"
//=============================================================================
int main()
{
	rb_t *rb=rb_new(1);
	if(rb==NULL) {return 1;}
	rb_free(rb);
	return 0;
}
