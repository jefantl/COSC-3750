##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 8
# 04/11/2021
#
# This makefile compiles simple and wyscanner
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wysehll: wyshell.c wyscanner.c wyscanner.h linkedlist.c linkedlist.h
	${CC} ${CFLAGS} wyshell.c wyscanner.c linkedlist.c -o wyshell

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wyshell
