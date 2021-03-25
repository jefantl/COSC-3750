##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 6
# 03/02/2021
#
# This makefile compiles wytar
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wytar: wytar.c wytarCreate.o wytarExtract.o
	$(CC) $(CFLAGS) wytar.c wytarCreate.o wytarExtract.o -o wytar

wytarCreate.o: wytarCreate.c wytarUtil.h
	$(CC) $(CFLAGS) -c wytarCreate.c

wytarExtract.o: wytarExtract.c wytarUtil.h
	$(CC) $(CFLAGS) -c wytarExtract.c

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wytar
