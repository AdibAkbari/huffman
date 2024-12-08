// Interface to a Counter ADT that keeps count of distinct tokens

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef COUNTER_H
#define COUNTER_H

typedef struct counter *Counter;

struct item {
	char *token;
	int freq;
};

/**
 * Returns a new empty counter
 */
Counter CounterNew(void);

/**
 * Frees all memory allocated to the counter
 */
void CounterFree(Counter c);

/**
 * Adds an occurrence of the given token to the counter
 */
void CounterAdd(Counter c, char *token);

/**
 * Returns the number of distinct tokens added to the counter
 */
int CounterNumItems(Counter c);

/**
 * Returns the frequency of the given token
 */
int CounterGet(Counter c, char *token);

/**
 * Returns a dynamically allocated array containing a copy of each distinct
 * token in the counter and its frequency (in any order), and sets *numItems to
 * the number of distinct tokens. Each token in the array must be dynamically
 * allocated.
 */
struct item *CounterItems(Counter c, int *numItems);

#endif
