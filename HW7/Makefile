##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 7
# 03/02/2021
#
# This makefile compiles wytalkD and wytalkC
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wytalkC: wytalkC.c wytalkUtil.c socketfun.c socketfun.h
	${CC} ${CFLAGS} wytalkC.c wytalkUtil.c socketfun.c -o wytalkC

wytalkD: wytalkD.c wytalkUtil.c socketfun.c socketfun.h
	${CC} ${CFLAGS} wytalkD.c wytalkUtil.c socketfun.c -o wytalkD

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wytalkC wytalkD
