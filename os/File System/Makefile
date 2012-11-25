# makefile for BAUSS File System
#
#  Created on:     		November 6, 2012
#  Authors:         	Khalil Fazal	Rayhaan Shakeel		Mahalia Cardinal
#  Student Numbers:		100 425 046		100 425 726			100 264 193

PROJECT = sfstest

all: $(PROJECT)

sfstest: sfstest.c fileSystem.c pathUtils.c entry.c blockio.c storeInt.c fControl.c openFiles.c
	gcc -ggdb -std=c99 -pedantic -O3 -Wall -Wextra $^ -o $@

clean:
	$(RM) $(PROJECT)
