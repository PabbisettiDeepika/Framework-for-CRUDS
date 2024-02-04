// To create a framework program for CRUDS operations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MENU_FILE "menu.cfg"
#define FIELDS_FILE "fields.cfg"
#define DATA_FILE "CRUDS_Framework.dat"

#define MAXIMUM_LENGTH 100

FILE *fpMenu;
FILE *fpFields;
FILE *fpData;

char **fieldNames;
char **printStatements;
int fieldsCount;
int fieldLength;

int getFieldLength()
{
	FILE *fpFieldLength = fopen("fieldLength.cfg", "r");
	fscanf(fpFieldLength, "%d", &fieldLength);
	fclose(fpFieldLength);
	return fieldLength;
}

int getFieldsCount()
{
	fieldsCount = 0;
	char fieldName[fieldLength];
	fpFields = fopen(FIELDS_FILE, "r");
	while (fgets(fieldName, MAXIMUM_LENGTH, fpFields))
	{
		fieldsCount++;
	}
	return fieldsCount;
}

char** getFieldNames()
{
	int fieldsCounter;

	fieldsCount = getFieldsCount();
	fieldNames = (char**)malloc(sizeof(char*) * fieldsCount);
	rewind(fpFields);
	for (fieldsCounter = 0; fieldsCounter <fieldsCount; fieldsCounter++)
	{
		fieldNames[fieldsCounter] = (char*)malloc(sizeof(char) * MAXIMUM_LENGTH);
		fgets(fieldNames[fieldsCounter], MAXIMUM_LENGTH, fpFields);
		fieldNames[fieldsCounter][strlen(fieldNames[fieldsCounter]) - 1] = '\0';
	}
	fclose(fpFields);
	return fieldNames;
}

char** getPrintStatements()
{
	int CountOfStatements = 0;

	FILE *fpPrintStatements = fopen("userStatements.cfg", "r");
	char userStatement[MAXIMUM_LENGTH];
	while (fgets(userStatement, MAXIMUM_LENGTH, fpPrintStatements))
	{
		CountOfStatements++;
	}
	rewind(fpPrintStatements);
	printStatements = (char**)malloc(sizeof(char*) * CountOfStatements);
	int counter;
	for (counter = 0; counter < CountOfStatements; counter++)
	{
		printStatements[counter] = (char*)malloc(sizeof(char) * MAXIMUM_LENGTH);
		fgets(printStatements[counter], MAXIMUM_LENGTH, fpPrintStatements);
		printStatements[counter][strlen(printStatements[counter]) - 1] = '\0';
	}
	return printStatements;
}

void printCreateStatus(int numberOfFieldsAdded)
{
	if (numberOfFieldsAdded == fieldsCount)
	{
		printf("%s\n", printStatements[0]);
	}
	else
	{
		printf("%s\n", printStatements[1]);
	}
}

char* getFieldValue(int fieldsCounter)
{
	char *fieldValue = (char*)malloc(sizeof(char) * MAXIMUM_LENGTH);
	printf("Please enter %s: ", fieldNames[fieldsCounter]);
	scanf(" %[^\n]s", fieldValue);
	return fieldValue;
}

void addRecord()
{
	int numberOfFieldsAdded = 0;
	int fieldsCounter;
	char *fieldValue = (char*)malloc(sizeof(char) * fieldLength);
	for (fieldsCounter = 0; fieldsCounter < fieldsCount; fieldsCounter++)
	{
		fieldValue = getFieldValue(fieldsCounter);
		fpData = fopen(DATA_FILE, "a");
		numberOfFieldsAdded = numberOfFieldsAdded + fwrite(fieldValue, fieldLength, 1, fpData);
		fclose(fpData);
	}
	strcpy(fieldValue, "A");
	fpData = fopen(DATA_FILE, "a");
	fwrite(fieldValue, fieldLength, 1, fpData);
	fclose(fpData);
	printCreateStatus(numberOfFieldsAdded);
}

void showRecords()
{
	char record[fieldsCount + 1][fieldLength];
	int recordsCount = 0;
	int fieldsCounter;
	fpData = fopen(DATA_FILE, "r");
	while (fread(record, fieldLength, fieldsCount + 1, fpData))
	{
		if (!strcmp(record[fieldsCount], "A"))
		{
			recordsCount++;
			printf("Record %d details: \n", recordsCount);
			for (fieldsCounter = 0; fieldsCounter < fieldsCount; fieldsCounter++)
			{
				printf("%s: %s\n", fieldNames[fieldsCounter], record[fieldsCounter]);
			}
			printf("\n");
		}
	}
	if (recordsCount == 0)
	{
		printf("No records found.\n");
	}
}

char* getRecordId(char operation[])
{
	char *recordId = (char*)malloc(sizeof(char) * fieldLength);
	printf("Please enter %s to %s: ", fieldNames[0], operation);
	scanf("%s", recordId);
	return recordId;
}

void printRecordStatus(char recordId[], int recordStatus)
{
	rewind(fpFields);
	if (recordStatus == -1)
	{
		printf("%s %s not found.\n", fieldNames[0], recordId);
	}
	else
	{
		printf("%s %s found.\n", fieldNames[0], recordId);
	}
}

void printUpdateStatus(char operation[], char recordId[], int isRecordUpdated)
{
	if (operation == "update")
	{
		if (isRecordUpdated)
		{
			printf("%s-%s\n", recordId, printStatements[2]);
		}
		else
		{
			printf("%s-%s\n", recordId, printStatements[3]);			
		}
	}

	if (operation == "delete")
	{
		if (isRecordUpdated)
		{
			printf("%s-%s\n", recordId, printStatements[4]);
		}
		else
		{
			printf("%s-%s\n", recordId, printStatements[5]);			
		}
	}
}

int getRecordPosition(char recordId[])
{
	int recordPosition = -1;
	char record[fieldsCount + 1][fieldLength];
	fpData = fopen(DATA_FILE, "r");
	while (fread(record, fieldLength, fieldsCount + 1, fpData))
	{
		if (!strcmp(record[fieldsCount], "A") && !strcmp(record[0], recordId))
		{
			recordPosition = ftell(fpData) - ((fieldsCount + 1) * fieldLength);
			break;
		}
	}
	return recordPosition;
}

char* getNewValue()
{
	char *newValue = (char*)malloc(sizeof(char) * fieldLength);
	printf("Please enter %s (new value): ", fieldNames[fieldsCount - 1]);
	scanf("%s", newValue);
	return newValue;
}

void updateRecord(char recordId[])
{
	int recordPosition = getRecordPosition(recordId);
	int isRecordUpdated = 0;
	char record[fieldsCount + 1][fieldLength];
	char *newValue;

	if (recordPosition != -1)
	{
		newValue = getNewValue();
		fpData = fopen(DATA_FILE, "r+");
		fseek(fpData, recordPosition, SEEK_SET);
		fread(record, fieldLength, fieldsCount + 1, fpData);
		strcpy(record[fieldsCount - 1], newValue);
		fseek(fpData, -(fieldLength * (fieldsCount + 1)), SEEK_CUR);
		isRecordUpdated = fwrite(record, fieldLength, fieldsCount + 1, fpData);
		fclose(fpData);
		printUpdateStatus("update", recordId, isRecordUpdated);
	}
	else
	{
		printRecordStatus(recordId, recordPosition);
	}
}

void deleteRecord(char recordId[])
{
	int recordPosition = getRecordPosition(recordId);
	int isRecordDeleted = 0;
	char record[fieldsCount + 1][fieldLength];

	if (recordPosition != -1)
	{
		fpData = fopen(DATA_FILE, "r+");
		fseek(fpData, recordPosition, SEEK_SET);
		fread(record, fieldLength, fieldsCount + 1, fpData);
		strcpy(record[fieldsCount], "D");
		fseek(fpData, -(fieldLength * (fieldsCount + 1)), SEEK_CUR);
		isRecordDeleted = fwrite(record, fieldLength, fieldsCount + 1, fpData);
		fclose(fpData);
		printUpdateStatus("delete", recordId, isRecordDeleted);
	}
	else
	{
		printRecordStatus(recordId, recordPosition);
	}
}

void searchRecord(char recordId[])
{
	int recordPosition = getRecordPosition(recordId);
	int fieldsCounter;
	char record[fieldsCount + 1][fieldLength];
	if (recordPosition != -1)
	{
		fpData = fopen(DATA_FILE, "r");
		fseek(fpData, recordPosition, SEEK_SET);
		fread(record, fieldLength, fieldsCount + 1, fpData);
		for (fieldsCounter = 0; fieldsCounter < fieldsCount; fieldsCounter++)
		{
			printf("%s: %s\n", fieldNames[fieldsCounter], record[fieldsCounter]);
		}
	}
	else
	{
		printRecordStatus(recordId, recordPosition);
	}
}

void showMenu()
{
	char menuLine[MAXIMUM_LENGTH];
	fpMenu = fopen(MENU_FILE, "r");
	while (fgets(menuLine, MAXIMUM_LENGTH, fpMenu))
	{
		printf("%s", menuLine);
	}
	fclose(fpMenu);
}

void chooseOption()
{
	int choice;
	printf("Enter your choice: ");
	scanf("%d", &choice);

	switch(choice)
	{
		case 0: exit(0);
		case 1: addRecord();
				break;
		case 2: showRecords();
				break;
		case 3: updateRecord(getRecordId("update"));
				break;
		case 4: deleteRecord(getRecordId("delete"));
				break;
		case 5: searchRecord(getRecordId("search"));
				break;
		default: printf("Invalid choice.\n");
				 break;
	}
}

void main()
{
	fieldLength = getFieldLength();
	fieldNames = getFieldNames();
	printStatements = getPrintStatements();
	while (1)
	{
		showMenu();
		chooseOption();
	}
}