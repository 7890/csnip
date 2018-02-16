#include <stdio.h>
#include <stdlib.h>

//inspired by //http://ptspts.blogspot.ch/2013/12/how-to-add-very-large-data-blobs-to-c.html
//tb/1802

#define VARIABLE "blob1"
#define SIGNATURE "__signature__"

int main(int argc, char *argv[])
{
	char *file_uri;
	char *variable;
	char *signature;

	if(argc<2)
	{
		fprintf(stderr, "args: file_uri (variable_name (signature))\n");
		return 1;
	}

	file_uri=argv[1];

	if(argc>2)
	{
		variable=argv[2];
	}
	else
	{
		variable=VARIABLE;
	}

	if(argc>3)
	{
		signature=argv[3];
	}
	else
	{
		signature=SIGNATURE;
	}

	FILE *f=fopen(file_uri, "r");
	if(f==NULL)
	{
		fprintf(stderr, "could not open file\n");
		return 1;
	}

	//get file size
	fseek(f, 0L, SEEK_END);
	size_t size = ftell(f);
	fclose(f);

	fprintf(stderr, "size: %zu\n", size);
	fprintf(stderr, "variable: %s\n", variable);
	fprintf(stderr, "signature: %s\n", signature);

	printf("//generated with dstub\n");
	printf("const char %s[%zu] = \"%s\";\n", variable, size, signature);
	printf("const char *%s_end = %s + sizeof(%s) / sizeof(char);\n", variable, variable, variable);

	fprintf(stderr, "\n// compile to object file:\n");
	fprintf(stderr, "cc -c -o %s.o %s.c\n", variable, variable);

	fprintf(stderr, "\n// use embedded data:\n");
	fprintf(stderr, "extern const char %s[], *%s_end;\n", variable, variable);
	fprintf(stderr, "size_t %s_size = %s_end - %s;\n", variable, variable, variable);
	fprintf(stderr, "fwrite(stderr, %s, 1, %s_size, stdout);\n", variable, variable);

	return 0;
}
//EOF
