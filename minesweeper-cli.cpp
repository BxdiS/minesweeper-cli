#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


int width, height;

int main(void) {
	printf("Enter width and height (eg: 9 9): ");
	scanf("%d %d", &width, &height);

	if (width < 1 || height < 1 || width > 100 || height > 100) {
		printf("Invalid size, must be 1-100\n");
		return 1;
	}

	printf("Field size set to %d x %d\n", width, height);
	getchar();
}


