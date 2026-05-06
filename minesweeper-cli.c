#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#define RED "\x1b[31m"
#define YEL "\x1b[33m"
#define GRA "\x1b[90m"
#define RES "\x1b[0m"

int width, height, mines;
char sideArt[32][256];
int sideArtLines = 0;

typedef struct {
	unsigned int hasMine : 1;
	unsigned int isOpen : 1;
	unsigned int isFlagged : 1;
	int nearMines;
} Cell;

typedef struct RecordNode {
	char name[20];
	int w, h;
	double time;
	struct RecordNode* next;
} RecordNode;

RecordNode* recordsHead = NULL;

void addRecordToList(const char* name, int w, int h, double time) {
	RecordNode* newNode = (RecordNode*)malloc(sizeof(RecordNode));
	if (!newNode) return;
	strcpy(newNode->name, name);
	newNode->w = w; newNode->h = h; newNode->time = time;
	newNode->next = recordsHead;
	recordsHead = newNode;
}

void saveRecords() {
	FILE* f = fopen("records.txt", "w");
	if (!f) return;
	RecordNode* curr = recordsHead;
	while (curr) {
		fprintf(f, "%s %d %d %.2f\n", curr->name, curr->w, curr->h, curr->time);
		curr = curr->next;
	}
	fclose(f);
}

void loadRecords() {
	FILE* f = fopen("records.txt", "r");
	if (!f) return;
	char name[20]; int w, h; double t;
	while (fscanf(f, "%19s %d %d %lf", name, &w, &h, &t) == 4) {
		addRecordToList(name, w, h, t);
	}
	fclose(f);
}

// Вспомогательная функция для печати пробелов (отступов)
void printPadding(int count) {
	for (int i = 0; i < count; i++) printf(" ");
}

// Функция для получения визуальной длины строки (без учета ANSI-кодов)
int getVisibleLength(const char* str) {
	int len = 0;
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] == '\x1b') { // Пропускаем ANSI последовательности
			while (str[i] != 'm' && str[i] != '\0') i++;
			if (str[i] == 'm') i++;
		}
		else {
			len++;
			i++;
		}
	}
	return len;
}

// Загрузка арта и подсчет его макс. ширины
int getFileMaxWidth(const char* filename) {
	FILE* f = fopen(filename, "r");
	if (!f) return 0;
	char line[256];
	int maxW = 0;
	while (fgets(line, sizeof(line), f)) {
		line[strcspn(line, "\r\n")] = 0;
		int currentW = getVisibleLength(line);
		if (currentW > maxW) maxW = currentW;
	}
	fclose(f);
	return maxW;
}

void showRecordsScreen() {
	system("cls");
	int titleWidth = getFileMaxWidth("title.txt");
	if (titleWidth < 40) titleWidth = 40;

	printf(YEL "=== HALL OF FAME ===\n\n" RES);
	printf("%-15s | %-10s | %-10s\n", "NAME", "SIZE", "TIME");
	printf("------------------------------------------\n");

	RecordNode* curr = recordsHead;
	if (!curr) printf("No records yet!\n");
	while (curr) {
		printf("%-15s | %2dx%-7d | %.2fs\n", curr->name, curr->w, curr->h, curr->time);
		curr = curr->next;
	}
	printf("\nPress any key to return...");
	_getch();
}

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
	int fieldDisplayWidth = 4 + (width * 3); // Вычисляем ширину поля в символах

	printf("Minesweeper %dx%d\n    ", width, height);
	for (int j = 0; j < width; j++) printf("%-3d", j + 1);
	printf("\n");

	int maxRows = (gameOver && sideArtLines > height) ? sideArtLines : height;

	for (int i = 0; i < maxRows; i++) {
		if (i < height) {
			printf("%2d  ", i + 1);
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				char* color = RES; char sym = '#';
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
			printPadding(fieldDisplayWidth);
		}

		if (gameOver && i < sideArtLines && sideArt[i][0] != '\0') {
			printf("   %s%s" RES, (win ? YEL : RED), sideArt[i]);
		}
		printf("\n");
	}

	// Центрирование кнопок управления под полем
	const char* lines[] = { "F - fast restart", "R - new size", "Q - quit to menu" };
	const char* playLines[] = { "Arrows - move", "Space - open", "F - place flag", "Q - quit to menu" };

	printf("\n");
	int numLines = gameOver ? 3 : 4;
	for (int i = 0; i < numLines; i++) {
		const char* text = gameOver ? lines[i] : playLines[i];
		printPadding((fieldDisplayWidth - (int)strlen(text)) / 2);
		printf("%s\n", text);
	}
}

int showMainMenu() {
	int selection = 0;
	int titleWidth = getFileMaxWidth("title.txt");
	if (titleWidth < 20) titleWidth = 20; // Минимум для красоты

	while (1) {
		system("cls");
		drawImage("title.txt");
		printf("\n");

		const char* menuItems[] = { "PLAY", "RECORDS", "QUIT" };
		for (int i = 0; i < 3; i++) {
			char itemText[30];
			if (selection == i) sprintf(itemText, "> %s <", menuItems[i]);
			else sprintf(itemText, "  %s  ", menuItems[i]);

			printPadding((titleWidth - (int)strlen(itemText)) / 2);
			if (selection == i) printf(RED "%s" RES "\n", itemText);
			else printf("%s\n", itemText);
		}

		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) selection = (selection + 2) % 3;
			else if (key == 80) selection = (selection + 1) % 3;
		}
		else if (key == 'q' || key == 'Q') return 2;
		else if (key == 13) return selection;
	}
}

int main(void) {
	system("");
	loadRecords();
	while (1) {
		int choice = showMainMenu();
		if (choice == 2) break;
		if (choice == 1) {
			showRecordsScreen();
			continue;
		}

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

			time_t start_t = time(NULL);
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
								time_t end_t = time(NULL);
								double total_t = difftime(end_t, start_t);
								printField(cells, cursorX, cursorY, gameOver, win);
								printf("\n" YEL "NEW RECORD! Time: %.2fs" RES "\nEnter your name: ", total_t);
								char playerName[20];
								scanf("%19s", playerName);
								addRecordToList(playerName, width, height, total_t);
								saveRecords();
							}
						}
					}
				}
				else if (key == 'f' || key == 'F') {
					int idx = cursorY * width + cursorX;
					if (!cells[idx].isOpen) cells[idx].isFlagged = !cells[idx].isFlagged;
				}
				else if (key == 'q' || key == 'Q') { gameOver = 1; backToMenu = 1; }
			}

			if (!backToMenu) {
				printField(cells, cursorX, cursorY, gameOver, win);
				while (1) {
					int choice = _getch();
					if (choice == 'f' || choice == 'F') break;
					if (choice == 'r' || choice == 'R') {
						system("cls");
						printf("Enter width and height: ");
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