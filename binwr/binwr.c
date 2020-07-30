//tb/2020.07

#include <stdio.h> //printf
#include <stdlib.h> //atoi
#include <string.h> //strlen
#include <inttypes.h> //uint32_t

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		fprintf(stderr, "need args: type value\n");
		fprintf(stderr, "types: -s: int32_t -u uint32_t\n");
		return 1;
	}

	int32_t sval=0;
	uint32_t uval=0;

	if(strlen(argv[1])>0)
	{
		if(argv[1][1]=='s')
		{
			sval=atol(argv[2]);
			fwrite((char*)&sval, sizeof(int32_t), 1, stdout);
		}
		else if(argv[1][1]=='u')
		{
			uval=strtoul(argv[2], NULL, 10);
			fwrite((char*)&uval, sizeof(uint32_t), 1, stdout);
		}
		else
		{
			fprintf(stderr, "unknown type: %c\n", argv[1][1]);
		}
	}

	return 0;
}
//EOF
