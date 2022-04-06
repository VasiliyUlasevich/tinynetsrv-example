# main target

all: main

main: main.c
	gcc main.c -O2 -o tinynetsrv

debug: main.c
	gcc main.c -o tinynetsrv
