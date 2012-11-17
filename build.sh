#!/bin/bash -e
rm -f basic.tab.c basic.tab.h lex.yy.c simplebasic0
if [ "$1" = "clean" ]; then 
	exit;
fi
flex basic.l
bison basic.y -d
gcc *.c -o simplebasic0
