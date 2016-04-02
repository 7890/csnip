//#include <stdio.h>
#include "hexdump.h"

int main (int argc, char *argv[])
{
	char my_str[] = "a char string greater than 16 chars";
	hexdump ("my_str", &my_str, sizeof (my_str));
	return 0;
}
