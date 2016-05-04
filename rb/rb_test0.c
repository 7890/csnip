#include "rb.h"
//=============================================================================
int main()
{
	rb_t *rb=rb_new(1);
	fprintf(stderr,"sizeof(rb_t): %zu\n",sizeof(rb_t));
	if(rb==NULL) {fprintf(stderr,"rb NULL\n");return 1;}
	rb_debug(rb);
	if(!rb_mlock(rb)) {fprintf(stderr,"!mlock\n");return 1;}
	rb_debug(rb);
	if(!rb_munlock(rb)) {fprintf(stderr,"!munlock\n");return 1;}
	rb_debug(rb);
	rb_free(rb);

	rb=rb_new_shared(1);
	fprintf(stderr,"sizeof(rb_t): %zu\n",sizeof(rb_t));
	if(rb==NULL) {fprintf(stderr,"rb shm NULL\n");return 1;}
	rb_debug(rb);
	if(!rb_mlock(rb)) {fprintf(stderr,"!mlock\n");return 1;}
	rb_debug(rb);
	if(!rb_munlock(rb)) {fprintf(stderr,"!munlock\n");return 1;}
	rb_debug(rb);
	rb_free(rb);

	return 0;
}
