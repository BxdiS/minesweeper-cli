#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

int width, height, mines;

typedef struct {
	unsigned int hasMine : 1;
	unsigned int isOpen : 1;
	unsigned int isFlagged : 1;
	int nearMines;
} Cell;

void openCell(Cell* cells, int x, int y) {
	int index = y * width + x;
	if (cells[index].isOpen || cells[index].isFlagged) return;
	cells[index].isOpen = 1;
	if (cells[index].hasMine) return;
	if (cells[index].nearMines == 0) {
		for (int di = -1; di <= 1; di++) {
			for (int dj = -1; dj <= 1; dj++) {
				int nx = x + dj, ny = y + di;
				if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
					openCell(cells, nx, ny);
				}
			}
		}
	}
}

void printField(Cell* cells, int cursorX, int cursorY) {
	system("cls");
	printf("Minesweeper %dx%d\n", width, height);
	printf("  ");
	for (int j = 0; j < width; j++) printf("%d ", j);
	printf("\n");
	for (int i = 0; i < height; i++) {
		printf("%d ", i);
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			char sym = '#';
			if (cells[index].isFlagged) sym = 'F';
			else if (cells[index].isOpen) {
				if (cells[index].hasMine) sym = '*';
				else sym = '0' + cells[index].nearMines;
			}
			if (i == cursorY && j == cursorX) printf("[%c]", sym);
			else printf(" %c ", sym);
		}
		printf("\n");
	}
	printf("Arrows: move, Space: open, F: flag, Q: quit\n");
}


int main(void) {
	printf("Enter width and height (eg: 9 9): ");
	scanf("%d %d", &width, &height);

	if (width < 1 || height < 1 || width > 100 || height > 100) {
		printf("Invalid size, must be 1-100\n");
		return 1;
	}
	mines = (width * height) / 10; // 10% of field

	// memory for cells
	Cell* cells = (Cell*)malloc(height * width * sizeof(Cell));
	if (!cells) {
		printf("Memory error\n");
		return 1;
	}
	// init
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			cells[index].hasMine = 0;
			cells[index].isOpen = 0;
			cells[index].isFlagged = 0;
			cells[index].nearMines = 0;
		}
	}
	// mine placing
	srand(time(NULL));
	int minesplaced = 0;
	while (minesplaced < mines) {
		int i = rand() % height;
		int j = rand() % width;
		int index = i * width + j;
		if (!cells[index].hasMine) {
			cells[index].hasMine = 1;
			minesplaced++;
		}
	}

	// nearMines
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			if (cells[index].hasMine) continue;
			int count = 0;

			for (int i2 = -1; i2 <= 1; i2++) {
				for (int j2 = -1; j2 <= 1; j2++) {
					int ni = i + i2;
					int nj = j + j2;
					if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
						int nindex = ni * width + nj;
						if (cells[nindex].hasMine) count++;
					}
				}
			}
			cells[index].nearMines = count;
		}
	}

	int cursorX = 0, cursorY = 0;
	int gameOver = 0, win = 0;

	while (!gameOver) {
		printField(cells, cursorX, cursorY);
		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72 && cursorY > 0) cursorY--;
			else if (key == 80 && cursorY < height - 1) cursorY++;
			else if (key == 75 && cursorX > 0) cursorX--;
			else if (key == 77 && cursorX < width - 1) cursorX++;
		}
		else if (key == ' ') {
			int index = cursorY * width + cursorX;
			if (!cells[index].isOpen && !cells[index].isFlagged) {
				openCell(cells, cursorX, cursorY);
				if (cells[index].hasMine) {
					gameOver = 1;
					for (int i = 0; i < height * width; i++) {
						if (cells[i].hasMine) cells[i].isOpen = 1;
					}
				}
				else {
					win = 1;
					for (int i = 0; i < height * width; i++) {
						if (!cells[i].hasMine && !cells[i].isOpen) {
							win = 0;
							break;
						}
					}
					if (win) gameOver = 1;
				}
			}
		}
		else if (key == 'f' || key == 'F') {
			int index = cursorY * width + cursorX;
			if (!cells[index].isOpen) {
				cells[index].isFlagged = !cells[index].isFlagged;
			}
		}
		else if (key == 'q' || key == 'Q') break;
	}

	printField(cells, cursorX, cursorY);
	if (win) printf("Win!\n");
	else printf("Game over...\n");

	free(cells);
	return 0;
}


