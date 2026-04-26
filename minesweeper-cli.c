#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

// Коды цветов
#define RED "\x1b[31m"
#define YEL "\x1b[33m"
#define GRA "\x1b[90m"
#define RES "\x1b[0m"

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
				if (nx >= 0 && nx < width && ny >= 0 && ny < height) openCell(cells, nx, ny);
			}
		}
	}
}

void printField(Cell* cells, int cursorX, int cursorY) {
	system("cls");
	printf("Minesweeper %dx%d\n  ", width, height);
	for (int j = 0; j < width; j++) printf("%d ", j % 10);
	printf("\n");

	for (int i = 0; i < height; i++) {
		printf("%d ", i % 10);
		for (int j = 0; j < width; j++) {
			int index = i * width + j;

			// Выбор цвета и символа
			char* color = RES;
			char sym = '#';

			if (cells[index].isFlagged) {
				color = YEL; sym = 'F'; // Флаг - Желтый
			}
			else if (cells[index].isOpen) {
				if (cells[index].hasMine) {
					color = RED; sym = '*'; // Мина - Красный
				}
				else {
					sym = '0' + cells[index].nearMines;
					if (sym == '0') { color = GRA; sym = '.'; } // Пустота - Серая точка
				}
			}
			else {
				color = GRA; sym = '#'; // Закрытая - Серая
			}

			if (i == cursorY && j == cursorX) printf("[%s%c%s]", color, sym, RES);
			else printf(" %s%c%s ", color, sym, RES);
		}
		printf("\n");
	}
	printf("Arrows: move, Space: open, F: flag, Q: quit\n");
}

int main(void) {
	system(""); // Включает поддержку ANSI цветов в Windows 10+
	printf("Enter width and height: ");
	if (scanf("%d %d", &width, &height) != 2) return 1;

	mines = (width * height) / 10;
	Cell* cells = (Cell*)calloc(height * width, sizeof(Cell));

	srand(time(NULL));
	int placed = 0;
	while (placed < mines) {
		int idx = rand() % (width * height);
		if (!cells[idx].hasMine) { cells[idx].hasMine = 1; placed++; }
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (cells[i * width + j].hasMine) continue;
			for (int di = -1; di <= 1; di++) {
				for (int dj = -1; dj <= 1; dj++) {
					int ni = i + di, nj = j + dj;
					if (ni >= 0 && ni < height && nj >= 0 && nj < width && cells[ni * width + nj].hasMine)
						cells[i * width + j].nearMines++;
				}
			}
		}
	}

	int cursorX = 0, cursorY = 0, gameOver = 0, win = 0;
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
			int idx = cursorY * width + cursorX;
			if (!cells[idx].isOpen && !cells[idx].isFlagged) {
				openCell(cells, cursorX, cursorY);
				if (cells[idx].hasMine) {
					gameOver = 1;
					for (int i = 0; i < width * height; i++) if (cells[i].hasMine) cells[i].isOpen = 1;
				}
				else {
					win = 1;
					for (int i = 0; i < width * height; i++)
						if (!cells[i].hasMine && !cells[i].isOpen) { win = 0; break; }
					if (win) gameOver = 1;
				}
			}
		}
		else if (key == 'f' || key == 'F') {
			int idx = cursorY * width + cursorX;
			if (!cells[idx].isOpen) cells[idx].isFlagged = !cells[idx].isFlagged;
		}
		else if (key == 'q' || key == 'Q') break;
	}

	printField(cells, cursorX, cursorY);
	printf(win ? "Win!\n" : "Game over...\n");
	free(cells);
	return 0;
}