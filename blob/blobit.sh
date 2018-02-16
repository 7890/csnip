#!/bin/sh

#//tb/1802

if [ $# -ne 3 ]
then
	echo "need 3 args: file_uri var_name object_name"
	exit 1
fi

SIGNATURE="_SIGNATURE_"

FILE_URI="$1"
VAR_NAME="$2"
OBJ_NAME="$3"

ls -1 ./dstub ./blobit >/dev/null 2>&1
ret=$?
if [ $ret -ne 0 ]
then
	echo "compiling programs"
	make -f Makefile
	echo ""
fi

mkdir -p _blobs

#dstub args: file_uri (variable_name (signature))
echo "creating stub"
./dstub "$FILE_URI" "$VAR_NAME" "$SIGNATURE" > _blobs/"$OBJ_NAME".c
cc -c -o _blobs/"$OBJ_NAME".o _blobs/"$OBJ_NAME".c

ls -l _blobs/"$OBJ_NAME".c
ls -l _blobs/"$OBJ_NAME".o

echo ""
echo "embedding file"
#blobit args: object_uri file_uri (signature)
./blobit _blobs/"$OBJ_NAME".o "$FILE_URI" "$SIGNATURE"
echo ""
echo "generating testuse.c"

cat - > testuse.c << _EOF_
#include <stdio.h>
extern const char ${VAR_NAME}[], *${VAR_NAME}_end;
int main()
{
        size_t ${VAR_NAME}_size = ${VAR_NAME}_end - ${VAR_NAME};
        fwrite(${VAR_NAME}, 1, ${VAR_NAME}_size, stdout);
        return 0;
}
_EOF_
cc -o testuse testuse.c _blobs/"$OBJ_NAME".o

echo "try ./testuse"
echo "done"

#EOF
