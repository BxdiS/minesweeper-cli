#ifndef GAME_H
#define GAME_H
#include "common.h"

void printPadding(int count);
int getVisibleLength(const char* str);
int getFileMaxWidth(const char* filename);
void loadSideArt(const char* filename);
void drawImage(const char* filename);
void openCell(Cell* cells, int x, int y);
void printField(Cell* cells, int cursorX, int cursorY, int gameOver, int win);

#endif