#include "common.h"
#include "game.h"
#include "records.h"

int showMainMenu() {
	int selection = 0;
	int titleWidth = getFileMaxWidth("title.txt");
	if (titleWidth < 20) titleWidth = 20;

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
		if (choice == 1) { showRecordsScreen(); continue; }

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
								gameOver = 1; loadSideArt("win.txt");
								double total_t = difftime(time(NULL), start_t);
								printField(cells, cursorX, cursorY, gameOver, win);
								printf("\n" YEL "NEW RECORD! %.2fs" RES "\nName: ", total_t);
								char pName[20]; scanf("%19s", pName);
								addRecordToList(pName, width, height, total_t);
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
						system("cls"); printf("Enter width and height: ");
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