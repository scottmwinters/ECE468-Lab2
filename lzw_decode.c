#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 256
#define VAL_TYPE unsigned int

typedef struct entry_tag {
	unsigned char *seq;
	VAL_TYPE val;
	int strLen;
	struct entry_tag *fwd;
} entry;

entry *head = NULL;
entry *tail = NULL;

// NULL = Not Found
entry *lookup_by_val(VAL_TYPE val) {

	if(head == NULL) return NULL;

	entry *rover = head;

	do {
		if(rover->val == val) return rover;
		if(rover->fwd != NULL) rover = rover->fwd;
	} while(rover->fwd != NULL);

	return NULL;
}

void add_entry(unsigned char *seq, int strLen) {

	entry *newEntry = calloc(sizeof(entry), 1);

	newEntry->seq = seq;
	newEntry->fwd = NULL;
	newEntry->strLen = strLen;

	if(head == NULL) {
		newEntry->val = 0;
		head = newEntry;
	}	

	if(tail == NULL)
		tail = newEntry;
	else {
		newEntry->val = tail->val + 1;
		tail->fwd = newEntry;
		tail = newEntry;
	}
	
}

void print_list() {

	entry *rover = head;

	do {

		printf("%d : ", rover->val);
		for(int i = 0; i < rover->strLen; i++)
			printf("%x", rover->seq[i]);

		printf("\n");

		rover = rover->fwd;
	} while(rover != NULL);
}

void print_string(unsigned char *str, int strLen) {
	for(int i = 0; i < strLen; i++)
		printf("%x", str[i]);

	printf("\n");

	// sleep(1);
}

int main(int argc, char **argv) {

	assert(argc == 2);

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	int strLen = strlen(argv[1]);
	
	char rleFileTag[MAX_STRING_LENGTH];
	strcpy(rleFileTag, argv[1] + strLen - 4);

	if(strcmp(rleFileTag, ".lzw") != 0) {
		printf("Invalid File Type. Must be '.lzw'\n");
		exit(1);
	}

	char outputFile[MAX_STRING_LENGTH];
	strncpy(outputFile, argv[1], strLen - 4);

	fpIn = fopen(argv[1], "rb");
	fpOut = fopen(outputFile, "wb");

	assert(fpIn != NULL);
	assert(fpOut != NULL);

	// Build Initial Single Byte Table (0 - 255)
	for(int i = 0; i < 256; i++) {
		unsigned char *temp = calloc(sizeof(*temp), 1);
		memcpy(temp, (unsigned char *)&i, 1);
		add_entry(temp, 1);
	}

	VAL_TYPE current = -1;
	VAL_TYPE previous = -1;

	entry *currentEntry = NULL;
	entry *previousEntry = NULL;

	unsigned char *X = NULL;
	unsigned char Y;
	unsigned char Z;

	fread(&previous, sizeof(previous), 1, fpIn);
	previousEntry = lookup_by_val(previous);
	fwrite(previousEntry->seq, sizeof(*previousEntry->seq), previousEntry->strLen, fpOut);

	while(fread(&current, sizeof(current), 1, fpIn)) {

		currentEntry = lookup_by_val(current);
		printf("\ncurrent = %x\n", current);
		printf("prevoius = %x\n", previous);

		if(currentEntry == NULL /* Current Entry Doesn't Exist */ ) {

			entry *previousEntry = lookup_by_val(previous);

			unsigned char *X = calloc(sizeof(*X), previousEntry->strLen);
			for(int i = 0; i < previousEntry->strLen; i++)
				X[i] = previousEntry->seq[i];

			Z = previousEntry->seq[0];

			unsigned char *temp = calloc(sizeof(*temp), previousEntry->strLen + 1);
			for(int i = 0; i < previousEntry->strLen; i++)
				temp[i] = X[i];

			temp[previousEntry->strLen] = Z;

			printf("No | ");
			print_string(temp, previousEntry->strLen + 1);
			fwrite(temp, sizeof(*temp), previousEntry->strLen + 1, fpOut);

			add_entry(temp, previousEntry->strLen + 1);

		} else {

			printf("Yes | ");
			print_string(currentEntry->seq, currentEntry->strLen);
			fwrite(currentEntry->seq, sizeof(*currentEntry->seq), currentEntry->strLen, fpOut);

			entry *previousEntry = lookup_by_val(previous);

			X = calloc(sizeof(*X), previousEntry->strLen);
			for(int i = 0; i < previousEntry->strLen; i++)
				X[i] = previousEntry->seq[i];

			Y = currentEntry->seq[0];

			unsigned char *temp = calloc(sizeof(*temp), previousEntry->strLen + 1);

			for(int i = 0; i < previousEntry->strLen; i++)
				temp[i] = X[i];

			temp[previousEntry->strLen] = Y;

			add_entry(temp, previousEntry->strLen + 1);
		}

		previous = current;

	}

	fclose(fpIn);
	fclose(fpOut);

	return 0;
}
