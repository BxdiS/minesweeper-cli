#include "records.h"
#include "game.h"

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