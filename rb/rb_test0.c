#include "rb.h"
//=============================================================================
int main()
{
	rb_t *rb=rb_new(1);
	if(rb==NULL) {return 1;}
	if(!rb_mlock(rb)) {return 1;}
	rb_debug(rb);
	rb_free(rb);

	rb=rb_new_shared(1);
	if(rb==NULL) {return 1;}
	if(!rb_mlock(rb)) {return 1;}
	rb_debug(rb);
	rb_free(rb);

	return 0;
}
