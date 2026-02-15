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

int main(void) {
	printf("Enter width and height (eg: 9 9): ");
	scanf("%d %d", &width, &height);

	if (width < 1 || height < 1 || width > 100 || height > 100) {
		printf("Invalid size, must be 1-100\n");
		return 1;
	}
	mines = (width * height) / 10;

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



	printf("Field size set to %d x %d, init succesful\n", width, height);
	free(cells);
	return 0;
}


