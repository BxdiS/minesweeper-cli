#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <string.h>

// Коды цветов
#define RED "\x1b[31m"
#define YEL "\x1b[33m"
#define GRA "\x1b[90m"
#define RES "\x1b[0m"

int width, height, mines;
char sideArt[32][256];
int sideArtLines = 0; // Переменная для хранения реального количества строк в арте

typedef struct {
	unsigned int hasMine : 1;
	unsigned int isOpen : 1;
	unsigned int isFlagged : 1;
	int nearMines;
} Cell;

void loadSideArt(const char* filename) {
	for (int i = 0; i < 32; i++) sideArt[i][0] = '\0';
	sideArtLines = 0;
	FILE* f = fopen(filename, "r");
	if (!f) return;
	while (sideArtLines < 32 && fgets(sideArt[sideArtLines], 256, f)) {
		sideArt[sideArtLines][strcspn(sideArt[sideArtLines], "\r\n")] = 0;
		sideArtLines++;
	}
	fclose(f);
}

void drawImage(const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f) {
		char line[256];
		while (fgets(line, sizeof(line), f)) printf("%s", line);
		fclose(f);
	}
}

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

void printField(Cell* cells, int cursorX, int cursorY, int gameOver, int win) {
	system("cls");
	printf("Minesweeper %dx%d\n    ", width, height);
	for (int j = 0; j < width; j++) printf("%-3d", j + 1);
	printf("\n");

	// Определяем, сколько строк всего нужно напечатать
	int maxRows = height;
	if (gameOver && sideArtLines > maxRows) maxRows = sideArtLines;

	for (int i = 0; i < maxRows; i++) {
		if (i < height) {
			printf("%2d  ", i + 1);
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				char* color = RES;
				char sym = '#';

				if (cells[index].isFlagged) { color = YEL; sym = 'F'; }
				else if (cells[index].isOpen) {
					if (cells[index].hasMine) { color = RED; sym = '*'; }
					else {
						sym = '0' + cells[index].nearMines;
						if (sym == '0') { color = GRA; sym = '.'; }
					}
				}
				else { color = GRA; sym = '#'; }

				if (!gameOver && i == cursorY && j == cursorX) printf("[%s%c%s]", color, sym, RES);
				else printf(" %s%c%s ", color, sym, RES);
			}
		}
		else {
			// Если поле закончилось, а арт еще нет — печатаем пустые отступы вместо клеток
			printf("    "); // отступ для номера строки
			for (int j = 0; j < width; j++) printf("   "); // отступы для ячеек
		}

		// Печать арта справа
		if (gameOver && i < sideArtLines && sideArt[i][0] != '\0') {
			printf("   %s%s" RES, (win ? YEL : RED), sideArt[i]);
		}
		printf("\n");
	}

	if (!gameOver) printf("Arrows - move\nSpace - open\nF - place flag\nQ - quit to menu\n");
}

int showMainMenu() {
	int selection = 0;
	while (1) {
		system("cls");
		drawImage("title.txt");
		printf("\n");
		if (selection == 0) printf("    " RED "> PLAY <" RES "\n");
		else printf("      PLAY\n");
		if (selection == 1) printf("    " RED "> QUIT <" RES "\n");
		else printf("      QUIT\n");

		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) selection = 0;
			else if (key == 80) selection = 1;
		}
		else if (key == 'q' || key == 'Q') return 1;
		else if (key == 13) return selection;
	}
}

int main(void) {
	system("");
	while (1) {
		if (showMainMenu() == 1) return 0;

		system("cls");
		printf("Enter width and height (max 32 32): ");
		if (scanf("%d %d", &width, &height) != 2) return 1;
		width = (width > 32) ? 32 : (width < 1 ? 1 : width);
		height = (height > 32) ? 32 : (height < 1 ? 1 : height);

		int backToMenu = 0;
		while (!backToMenu) {
			mines = (width * height) / 10;
			if (mines < 1) mines = 1;
			Cell* cells = (Cell*)calloc(height * width, sizeof(Cell));
			srand((unsigned int)time(NULL));
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
				printField(cells, cursorX, cursorY, gameOver, win);
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
							loadSideArt("lose.txt");
						}
						else {
							win = 1;
							for (int i = 0; i < width * height; i++)
								if (!cells[i].hasMine && !cells[i].isOpen) { win = 0; break; }
							if (win) {
								gameOver = 1;
								loadSideArt("win.txt");
							}
						}
					}
				}
				else if (key == 'f' || key == 'F') {
					int idx = cursorY * width + cursorX;
					if (!cells[idx].isOpen) cells[idx].isFlagged = !cells[idx].isFlagged;
				}
				else if (key == 'q' || key == 'Q') {
					gameOver = 1; backToMenu = 1;
				}
			}

			if (!backToMenu) {
				printField(cells, cursorX, cursorY, gameOver, win);
				printf("\nF - fast restart\nR - new size\nQ - quit to menu\n");
				while (1) {
					int choice = _getch();
					if (choice == 'f' || choice == 'F') break;
					if (choice == 'r' || choice == 'R') {
						system("cls");
						printf("Enter width and height (max 32 32): ");
						scanf("%d %d", &width, &height);
						width = (width > 32) ? 32 : (width < 1 ? 1 : width);
						height = (height > 32) ? 32 : (height < 1 ? 1 : height);
						break;
					}
					if (choice == 'q' || choice == 'Q') { backToMenu = 1; break; }
				}
			}
			free(cells);
		}
	}
	return 0;
}