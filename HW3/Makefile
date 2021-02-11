##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 3
# 02/11/2021
#
# This makefile compiles code, generates executables, and cleans the directory
##################################

CC = gcc
CFLAGS = -ggdb

OBJS = display.o compute.o prompt.o

.PHONY: tidy clean

approxe: approxe.c $(OBJS)
	$(CC) $(CFLAGS) approxe.c $(OBJS) -o approxe

display.o: display.h display.c
	$(CC) $(CFLAGS) -c display.c

compute.o: compute.h compute.c
	$(CC) $(CFLAGS) -c compute.c

prompt.o: prompt.h prompt.c
	$(CC) $(CFLAGS) -c prompt.c

tidy:
	/bin/rm -f $(OBJS) core.*

clean: tidy
	/bin/rm -f approxe