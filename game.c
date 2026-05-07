#include "game.h"

int width, height, mines;
char sideArt[32][256];
int sideArtLines = 0;

void printPadding(int count) {
	for (int i = 0; i < count; i++) printf(" ");
}

int getVisibleLength(const char* str) {
	int len = 0, i = 0;
	while (str[i] != '\0') {
		if (str[i] == '\x1b') {
			while (str[i] != 'm' && str[i] != '\0') i++;
			if (str[i] == 'm') i++;
		}
		else { len++; i++; }
	}
	return len;
}

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
	int fieldDisplayWidth = 4 + (width * 3);
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
		else printPadding(fieldDisplayWidth);

		if (gameOver && i < sideArtLines && sideArt[i][0] != '\0')
			printf("   %s%s" RES, (win ? YEL : RED), sideArt[i]);
		printf("\n");
	}

	const char* lines[] = { "F - fast restart", "R - new size", "Q - quit to menu" };
	const char* playLines[] = { "Arrows - move", "Space - open", "F - flag", "Q - menu" };

	printf("\n");
	int numLines = gameOver ? 3 : 4;
	for (int i = 0; i < numLines; i++) {
		const char* text = gameOver ? lines[i] : playLines[i];
		printPadding((fieldDisplayWidth - (int)strlen(text)) / 2);
		printf("%s\n", text);
	}
}