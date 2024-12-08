// Interface to the File ADT

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

#define MAX_TOKEN_LEN 4

typedef struct file *File;

/**
 * Opens a file for reading.
 * The file must be closed with FileClose after you have finished
 * reading from it.
 */
File FileOpenToRead(char *filename);

/**
 * Opens a file for writing.
 * If there is no file with the given name, it will be created.
 * If there is already a file with the given name, it will be truncated
 * (i.e., cleared) first.
 * The file must be closed with FileClose after you have finished
 * writing to it.
 */
File FileOpenToWrite(char *filename);

/**
 * Closes the file.
 */
void FileClose(File file);

/**
 * Reads a token from the file and stores the token, along with the
 * terminating null byte ('\0'), to the given array. The length of a
 * token is at most MAX_TOKEN_LEN bytes, not including the terminating
 * null byte ('\0').
 *
 * Calling this function multiple times on a file will cause it to read
 * tokens sequentially from the file. The first time this is called on a
 * file, it will read the first token, the next time it is called, it
 * will read the second token, and so on.
 *
 * Assumes that the file is open for reading.
 * Returns true if a token was successfully read, and false otherwise
 * (e.g., if the end of the file was reached).
 */
bool FileReadToken(File file, char arr[]);

/**
 * Writes a string to the file.
 * The string will be written to the end of the file (i.e., after all
 * the strings that were previously written with this function).
 *
 * Assumes that the file is open for writing.
 */
void FileWrite(File file, char *str);

#endif
