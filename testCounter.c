// Main program for testing the Counter ADT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"

static void test1(void);
static void test2(void);
static void test3(void);
static void test4(void);

int main(void) {
    test1();
    test2();
    test3();
	test4();
}

static void test1(void) {
    Counter counter = CounterNew();

    char *tokens[10] = {
        "a", "dog", "cat", "a", "bird", "dog", "a", "a", "cat", "dog",
    };

    char token[100];
    for (int i = 0; i < 10; i++) {
        strcpy(token, tokens[i]);
        CounterAdd(counter, token);
    }

    assert(CounterNumItems(counter) == 4);

    CounterFree(counter);

    printf("Test 1 passed!\n");
}

static void test2(void) {
    Counter counter = CounterNew();

    char *tokens[10] = {
        "a", "dog", "cat", "a", "bird", "dog", "a", "a", "cat", "dog"
    };

    char token[100];
    for (int i = 0; i < 10; i++) {
        strcpy(token, tokens[i]);
        CounterAdd(counter, token);
    }

    assert(CounterGet(counter, "a") == 4);
    assert(CounterGet(counter, "bird") == 1);
    assert(CounterGet(counter, "cat") == 2);
    assert(CounterGet(counter, "dog") == 3);
    assert(CounterGet(counter, "emu") == 0);

    CounterFree(counter);

    printf("Test 2 passed!\n");
}

static void test3(void) {
    Counter counter = CounterNew();

    char *tokens[10] = {
        "a", "dog", "cat", "a", "bird", "dog", "a", "a", "cat", "dog"
    };

    char token[100];
    for (int i = 0; i < 10; i++) {
        strcpy(token, tokens[i]);
        CounterAdd(counter, token);
    }

    int numItems = 0;
    struct item *items = CounterItems(counter, &numItems);
    assert(numItems == 4);

    // Simple sort to put items in alphabetical order
    for (int i = 0; i < numItems; i++) {
        int min = i;
        for (int j = i + 1; j < numItems; j++) {
            if (strcmp(items[j].token, items[min].token) < 0) {
                min = j;
            }
        }

        struct item tmp = items[i];
        items[i] = items[min];
        items[min] = tmp;
    }

    assert(strcmp(items[0].token, "a") == 0 && items[0].freq == 4);
    assert(strcmp(items[1].token, "bird") == 0 && items[1].freq == 1);
    assert(strcmp(items[2].token, "cat") == 0 && items[2].freq == 2);
    assert(strcmp(items[3].token, "dog") == 0 && items[3].freq == 3);

    for (int i = 0; i < numItems; i++) {
        free(items[i].token);
    }
    free(items);
    CounterFree(counter);

    printf("Test 3 passed!\n");
}

static void test4(void) {
    Counter counter = CounterNew();
    
    char *tokens[] = {
        "apple", "banana", "cherry", "date", "elderberry", 
        "fig", "grape", "honeydew", "kiwi", "lemon"
    };
    int frequencies[] = {10, 5, 15, 20, 1, 8, 25, 2, 12, 9};
    int numTokens = sizeof(tokens) / sizeof(tokens[0]);
    
    for (int i = 0; i < numTokens; i++) {
        for (int j = 0; j < frequencies[i]; j++) {
            CounterAdd(counter, tokens[i]);
        }
    }
    
    for (int i = 0; i < numTokens; i++) {
        assert(CounterGet(counter, tokens[i]) == frequencies[i]);
    }
    
    assert(CounterNumItems(counter) == numTokens);
    
    CounterFree(counter);
    
    printf("Test 4 passed!\n");
}