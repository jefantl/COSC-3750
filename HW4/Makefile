##################################
# Makefile
# Jason Fantl
# COSC 3750 Spring 2021
# Homework 4
# 02/11/2021
#
# This makefile compiles wycat
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wycat: wycat.c
	$(CC) $(CFLAGS) wycat.c -o wycat

tidy:
	/bin/rm -f core.*

clean: tidy
	/bin/rm -f wycat
