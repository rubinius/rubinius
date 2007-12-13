#!/bin/sh

cc -dynamic -bundle -undefined suppress -flat_namespace -I ../../shotgun/lib/subtend -g -o test.bundle test.c
