// Implementation of the File ADT

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "File.h"

typedef enum {
	READ,
	WRITE,
} Mode;

struct file {
	FILE *fp;
	Mode mode;
};

/**
 * Opens a file for reading
 * The file must be closed with FileClose
 */
File FileOpenToRead(char *filename) {
	File file = malloc(sizeof(*file));
	if (file == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	file->fp = fopen(filename, "r");
	if (file->fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for reading\n", filename);
		exit(EXIT_FAILURE);
	}

	file->mode = READ;
	return file;
}

/**
 * Opens a file for writing
 * The file must be closed with FileClose
 */
File FileOpenToWrite(char *filename) {
	File file = malloc(sizeof(*file));
	if (file == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	file->fp = fopen(filename, "w");
	if (file->fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for writing\n", filename);
		exit(EXIT_FAILURE);
	}

	file->mode = WRITE;
	return file;
}

/**
 * Closes the file
 */
void FileClose(File file) {
	fclose(file->fp);
	free(file);
}

/**
 * Reads a token from the file and stores it in the given buffer
 * Returns true if a token was successfully read, and false otherwise
 */
bool FileReadToken(File file, char arr[]) {
	assert(file->mode == READ);

	int ch = fgetc(file->fp);
	if (ch == EOF) {
		return false;
	}

	ungetc(ch, file->fp);
	char byte1 = (char) ch;

	int len;
	if ((0b10000000 & byte1) == 0) {
		len = 1;
	} else if ((0b11100000 & byte1) == 0b11000000) {
		len = 2;
	} else if ((0b11110000 & byte1) == 0b11100000) {
		len = 3;
	} else if ((0b11111000 & byte1) == 0b11110000) {
		len = 4;
	} else {
		fprintf(stderr, "error: invalid token\n");
		return false;
	}

	fread(arr, 1, len, file->fp);
	arr[len] = '\0';
	return true;
}

/**
 * Writes a string to the file
 */
void FileWrite(File file, char *str) {
	assert(file->mode == WRITE);

	fprintf(file->fp, "%s", str);
}
