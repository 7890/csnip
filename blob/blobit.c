#define _GNU_SOURCE //memmem

#include <stdio.h> //printf
#include <sys/mman.h> //mmap
#include <string.h> //memcpy

//tb/1802

#define SIGNATURE "__signature__"

int main(int argc, char *argv[])
{
	char *object_uri;
	char *file_uri;
	char *signature;

	if(argc<3)
	{
		fprintf(stderr, "args: object_uri file_uri (signature)\n");
		return 1;
	}

	object_uri=argv[1];
	file_uri=argv[2];

	if(argc>3)
	{
		signature=argv[3];
	}
	else
	{
		signature=SIGNATURE;
	}

//open files
	FILE *fobj=fopen(object_uri, "r+"); //read/write
	FILE *femb=fopen(file_uri, "r"); //read

	if(fobj==NULL || femb==NULL)
	{
		fprintf(stderr,"could not open file(s)\n");
		return 1;
	}

//get file sizes
	fseek(fobj, 0L, SEEK_END);
	size_t obj_len = ftell(fobj);
	fseek(fobj, 0L, SEEK_SET);

	fseek(femb, 0L, SEEK_END);
	size_t emb_len = ftell(femb);
	fseek(femb, 0L, SEEK_SET);

	printf("obj_len: %zu\n", obj_len);
	printf("emb_len: %zu\n", emb_len);

//mmap files
	char *obj=(char*)mmap(0, obj_len, PROT_READ | PROT_WRITE
		, MAP_SHARED, fileno(fobj), 0);

	char *emb=(char*)mmap(0, emb_len, PROT_READ
		, MAP_SHARED, fileno(femb), 0);

	fclose(fobj);
	fclose(femb);

	if(obj==NULL || emb==NULL)
	{
		fprintf(stderr,"could not map file(s)\n");
		return 1;
	}

//find signature offset
	char *ptr=memmem(obj, obj_len, signature, strlen(signature));
	if(ptr==NULL)
	{
		fprintf(stderr, "signature '%s' not found.\n", signature);
		munmap(fobj, obj_len);
		munmap(femb, emb_len);
		return 1;
	}

	size_t offset=(char*)ptr-obj;

	printf("found signature at offset %zu\n", offset);

//copy from embed to object at offset
	memcpy((char*)obj+offset, emb, emb_len);

//clean up
	munmap(fobj, obj_len);
	munmap(femb, emb_len);

	printf("file %s was embedded to object %s at byte offset %zu\n"
		, file_uri, object_uri, offset);

	return 0;
}
//EOF
