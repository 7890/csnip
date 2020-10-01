/*
//tb/2020.10

tailf: similar to tail -f
continuously prints last line in file.

largely inspired by
https://stackoverflow.com/questions/10164597/how-would-you-implement-tail-efficiently
*/

#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

#define BUFF_SIZE 4096

off_t prev_end=0;

FILE *openFile(const char *filePath);
void printLine(FILE *file, off_t startline);
void walkFile(FILE *file, long nlines);

int main(int argc, char *argv[])
{
	FILE *file;
	if(argc<2)
	{
		fprintf(stderr, "need argument: file\n");
		return 1;
	}

	file= openFile(argv[1]);

	while(1==1) ///
	{
		walkFile(file, 1);
		usleep(5000);
	}
	fclose(file);

	return 0;
}

FILE *openFile(const char *filePath)
{
	FILE *file;
	file= fopen(filePath, "r");
	if(file == NULL)
	{
		fprintf(stderr, "Error opening file: %s\n", filePath);
		exit(errno);
	}
	return(file);
}

void printLine(FILE *file, off_t startline)
{

	int fd, ret;
	fd= fileno(file);
	int nread;
	char buffer[BUFF_SIZE];
	lseek(fd, (startline + 1), SEEK_SET);
	while((nread= read(fd, buffer, BUFF_SIZE)) > 0)
	{
		ret=write(STDOUT_FILENO, buffer, nread);
	}
	(void) ret;
}

void walkFile(FILE *file, long nlines)
{
	off_t fposition;
	fseek(file, 0, SEEK_END);
	fposition= ftell(file);

	if(fposition==prev_end)
	{
		//no new data since last call
		return;
	}

	if(fposition < prev_end)
	{
		fprintf(stderr, "*** tail: file truncated\n");
	}

	prev_end=fposition;

	off_t index= fposition;
	off_t end= fposition;
	long countlines= 0;
	char cbyte;

	for(; index >= 0; index --)
	{
		cbyte= fgetc(file);
		if (cbyte == '\n' && (end - index) > 1)
		{
			countlines ++;
			if(countlines == nlines)
			{
		break;
			}
		 }
		fposition--;
		fseek(file, fposition, SEEK_SET);
	}

	if(countlines<1){return;}

	printLine(file, fposition);
}
//EOF
