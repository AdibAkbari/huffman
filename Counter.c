// Implementation of the Counter ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"

// node of counter BST
struct node {
	char *token;
	int freq;
	struct node *left;
	struct node *right;
};

struct counter {
	struct node *root;
	int numItems;
};

static void freeNodes(struct node *node);
static struct node *addNode(struct node *node, char *token);
static int tokenFreq(struct node *node, char *token);
static struct item *getItemsArray(struct node *node, struct item *items,
								  int *index);

/**
 * Returns a new empty counter
 */
Counter CounterNew(void) {
	Counter c = malloc(sizeof(struct counter));
	if (c == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	c->root = NULL;
	c->numItems = 0;
	return c;
}

/**
 * Frees all memory allocated to the counter
 */
void CounterFree(Counter c) {
	freeNodes(c->root);
	free(c);
}

/**
 * Adds an occurrence of the given token to the counter
 */
void CounterAdd(Counter c, char *token) {
	if (c->root == NULL || !tokenFreq(c->root, token)) {
		c->numItems++;
	}
	c->root = addNode(c->root, token);
}

/**
 * Returns the number of distinct tokens added to the counter
 */
int CounterNumItems(Counter c) {
	return c->numItems;
}

/**
 * Returns the frequency of the given token
 */
int CounterGet(Counter c, char *token) {
	return tokenFreq(c->root, token);
}

/**
 * Returns a dynamically allocated array containing a copy of each distinct
 * token in the counter and its frequency (in any order), and sets *numItems to
 * the number of distinct tokens. 
 */
struct item *CounterItems(Counter c, int *numItems) {
	struct item *items = malloc(sizeof(struct item) * c->numItems);
	if (items == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}
	*numItems = c->numItems;
	int index = 0;
	getItemsArray(c->root, items, &index);
	return items;
}

// Helper functions:

/**
 * Frees all nodes in BST recursively
*/
static void freeNodes(struct node *node) {
	if (node == NULL) {
		return;
	}
	freeNodes(node->left);
	freeNodes(node->right);
	free(node->token);
	free(node);
}

/**
 * Inserts a node with the given token to the BST, or increments its frequency
 * if it is already in the tree.
*/
static struct node *addNode(struct node *node, char *token) {
	if (node == NULL) {
		node = malloc(sizeof(struct node));
		if (node == NULL) {
			fprintf(stderr, "error: out of memory\n");
			exit(EXIT_FAILURE);
		}

		node->token = strdup(token);
		node->freq = 1;
		node->left = NULL;
		node->right = NULL;
	} else {
		int comparison = strcmp(token, node->token);
		if (comparison < 0) {
			node->left = addNode(node->left, token);
		} else if (comparison > 0) {
			node->right = addNode(node->right, token);
		} else {  // token already in tree
			node->freq++;
		}
	}
	return node;
}

/**
 * Returns the frequency of the given token by finding it in the BST.
*/
static int tokenFreq(struct node *node, char *token) {
	if (node == NULL) {
		return 0;
	}
	int comparison = strcmp(token, node->token);
	if (comparison < 0) {
		return tokenFreq(node->left, token);
	} else if (comparison > 0) {
		return tokenFreq(node->right, token);
	} else {
		return node->freq;
	}
}

/**
 * Returns all nodes in counter BST as an array of items by traversing tree
 * recursively.
*/
static struct item *getItemsArray(struct node *node, struct item *items,
								  int *index) {
	if (node == NULL) {
		return items;
	}
	items = getItemsArray(node->left, items, index);

	items[*index].token = strdup(node->token);
	items[*index].freq = node->freq;
	(*index)++;

	items = getItemsArray(node->right, items, index);

	return items;
}