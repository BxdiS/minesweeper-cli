#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define RED "\x1b[31m"
#define YEL "\x1b[33m"
#define GRA "\x1b[90m"
#define RES "\x1b[0m"

extern int width, height, mines;
extern char sideArt[32][256];
extern int sideArtLines;

typedef struct {
	unsigned int hasMine : 1;
	unsigned int isOpen : 1;
	unsigned int isFlagged : 1;
	int nearMines;
} Cell;

#endif