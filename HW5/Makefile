##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 5
# 03/02/2021
#
# This makefile compiles wyls
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wyls: wyls.c
	$(CC) $(CFLAGS) wyls.c -o wyls

tidy:
	/bin/rm -f core.*

clean: tidy
	/bin/rm -f wyls
