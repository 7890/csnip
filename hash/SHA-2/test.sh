#!/bin/sh
make OPTIMIZE= && \
make OPTIMIZE=-O1 && \
make OPTIMIZE=-O2 && \
make OPTIMIZE=-O3 && \
ls -1 _build/*|while read line; do echo -n ";${line};"; "$line"; done
