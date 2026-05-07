#ifndef RECORDS_H
#define RECORDS_H
#include "common.h"

typedef struct RecordNode {
	char name[20];
	int w, h;
	double time;
	struct RecordNode* next;
} RecordNode;

void addRecordToList(const char* name, int w, int h, double time);
void saveRecords();
void loadRecords();
void showRecordsScreen();

#endif