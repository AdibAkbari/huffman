// Implementation of the Huffman module

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Counter.h"
#include "File.h"
#include "huffman.h"

#define MAX_TOKEN_ENC_SIZE 100

struct listNode {
	struct huffmanTree *huffNode;
	struct listNode *next;
};

struct bstNode {
	char *encoding;
	int encLength;
	char *token;
	struct bstNode *left;
	struct bstNode *right;
};
// Prototypes:
static struct listNode *listInsertOrdered(struct listNode *head,
										  struct huffmanTree *node);

static int listSize(struct listNode *head);

static struct huffmanTree *removeFirstNode(struct listNode **head);

static struct huffmanTree *createHuffmanNode(char *token, int freq);

static struct huffmanTree *combineTrees(struct huffmanTree *root1,
										struct huffmanTree *root2);

static struct bstNode *newBstNode(char *token, char *encoding, int encLength);

static struct bstNode *bstInsert(struct bstNode *node, char *token,
								 char *encoding, int encLength);

static void bstFree(struct bstNode *tree);

static char *getTokenEncoding(struct bstNode *node, char *token,
							  int *encLength);

static struct bstNode *addTokenEncodings(struct huffmanTree *huffNode,
										 struct bstNode *encTree,
										 char *currEncoding, int depth);


// Task 1

/**
 * Decodes the given encoded text using the huffman tree.
*/
void decode(struct huffmanTree *tree, char *encoding, char *outputFilename) {
	struct huffmanTree *node = tree;

	int encodingLength = strlen(encoding);
	// text cannot be longer than the length of the encoding string.
	char *decodedText = malloc(encodingLength + 1);
	if (decodedText == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	int decodedIndex = 0;

	for (int i = 0; i < encodingLength; i++) {
		if (encoding[i] == '0')
			node = node->left;
		else
			node = node->right;

		if (node->token != NULL) {
			// Copies token at the node into the text string.
			// Using string to avoid repeatedly writing to file which 
			// becomes very slow.
			strcpy(&decodedText[decodedIndex], node->token);
			decodedIndex += strlen(node->token);
			node = tree;
		}
	}
	decodedText[decodedIndex] = '\0';

	File file = FileOpenToWrite(outputFilename);
	FileWrite(file, decodedText);
	FileClose(file);

	free(decodedText);
}

// Task 3

/**
 * Constructs a Huffman tree from the tokens in the given file. 
 * Higher frequency tokens appear higher in the tree.
*/
struct huffmanTree *createHuffmanTree(char *inputFilename) {
	File file = FileOpenToRead(inputFilename);
	Counter counter = CounterNew();
	char *token = malloc(MAX_TOKEN_LEN + 1);
	if (token == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}
	while (FileReadToken(file, token)) {
		CounterAdd(counter, token);
	}
	free(token);
	FileClose(file);

	struct listNode *head = NULL;

	int numItems = 0;
	struct item *items = CounterItems(counter, &numItems);
	// adds all tokens in counter to linked list sorted in non-descending order
	for (int i = 0; i < numItems; i++) {
		struct huffmanTree *node =
			createHuffmanNode(items[i].token, items[i].freq);
		head = listInsertOrdered(head, node);
	}
	// freeing items:
	for (int i = 0; i < numItems; i++) {
		free(items[i].token);
	}
	free(items);
	CounterFree(counter);

	while (listSize(head) > 1) {
		// combines the two nodes with lowest frequency and adds back to linked
		// list in sorted order
		struct huffmanTree *root1 = removeFirstNode(&head);
		struct huffmanTree *root2 = removeFirstNode(&head);

		struct huffmanTree *combinedRoot = combineTrees(root1, root2);

		head = listInsertOrdered(head, combinedRoot);
	}
	// last remaining node in linked list will hold the final huffman tree.
	struct huffmanTree *finalTree = head->huffNode;
	free(head);
	return finalTree;
}

// Task 4

/**
 * Uses the given huffman tree to encode the text in the given file.
*/
char *encode(struct huffmanTree *tree, char *inputFilename) {
	int maxSize = 512;
	char *result = malloc(maxSize);
	int index = 0;

	// initialising encoding BST and adding the encoding of each token to it
	struct bstNode *encTree = NULL;
	char currentEncoding[MAX_TOKEN_ENC_SIZE];
	encTree = addTokenEncodings(tree, encTree, currentEncoding, 0);

	File file = FileOpenToRead(inputFilename);
	char *token = malloc(MAX_TOKEN_LEN + 1);
	if (token == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}
	while (FileReadToken(file, token)) {
		// gets token encoding from encoding BST and copies it to result array.
		int encLength;
		char *tokenEnc = getTokenEncoding(encTree, token, &encLength);
		for (int i = 0; i < encLength; i++) {
			result[index] = tokenEnc[i];
			index++;

			if (index >= maxSize - 5) {
				maxSize *= 2;
				char *newResult = realloc(result, maxSize);
				if (newResult == NULL) {
					fprintf(stderr, "error: out of memory\n");
					free(result);
					exit(EXIT_FAILURE);
				}
				result = newResult;
			}
		}
	}
	free(token);
	bstFree(encTree);
	FileClose(file);
	result[index] = '\0';
	return result;
}


// Helper functions for Task 3:

/**
 * Creates a list node that stores given huffman tree node and inserts it
 * into the linked list in non-descending order (of frequency).
*/
static struct listNode *listInsertOrdered(struct listNode *head,
										  struct huffmanTree *node) {

	struct listNode *newNode = malloc(sizeof(struct listNode));
	if (newNode == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	newNode->huffNode = node;
	if (head == NULL) {
		newNode->next = NULL;
		return newNode;
	}

	struct listNode *curr = head;
	struct listNode *prev = NULL;
	while (curr != NULL) {
		if (node->freq < curr->huffNode->freq) {
			if (prev == NULL) {
				newNode->next = curr;
				return newNode;
			}
			prev->next = newNode;
			newNode->next = curr;
			return head;
		}
		prev = curr;
		curr = curr->next;
	}
	prev->next = newNode;
	newNode->next = NULL;
	return head;
}

/**
 * Returns the size of the given linked list
*/
static int listSize(struct listNode *head) {
	struct listNode *curr = head;
	int size = 0;
	while (curr != NULL) {
		size++;
		curr = curr->next;
	}
	return size;
}

/**
 * Removes the first node from the given list and returns the huffman tree node
 * it stores. Frees the list node.
*/
static struct huffmanTree *removeFirstNode(struct listNode **head) {
	struct huffmanTree *node = (*head)->huffNode;

	struct listNode *temp = *head;
	*head = (*head)->next;
	free(temp);

	return node;
}

/**
 * Creates a huffman tree node with token and frequency. Initialises left and 
 * right children to NULL.
*/
static struct huffmanTree *createHuffmanNode(char *token, int freq) {
	struct huffmanTree *node = malloc(sizeof(struct huffmanTree));
	if (node == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}
	node->freq = freq;
	node->token = strdup(token);
	node->left = NULL;
	node->right = NULL;
	return node;
}

/**
 * Combines two huffman trees by making them the left and right children of a 
 * new tree. Frequency of new tree is combined frequency of two children.
*/
static struct huffmanTree *combineTrees(struct huffmanTree *root1,
										struct huffmanTree *root2) {

	struct huffmanTree *newRoot = malloc(sizeof(struct huffmanTree));
	if (newRoot == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	newRoot->freq = root1->freq + root2->freq;
	newRoot->token = NULL;
	newRoot->left = root1;
	newRoot->right = root2;
	return newRoot;
}

// Helper functions for Task 4:

/**
 * Creates new node for encoding BST.
*/
static struct bstNode *newBstNode(char *token, char *encoding, int encLength) {
	struct bstNode *new = malloc(sizeof(struct bstNode));
	if (new == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	new->token = strdup(token);
	new->encoding = strdup(encoding);
	new->encLength = encLength;
	new->left = NULL;
	new->right = NULL;
	return new;
}

/**
 * Creates and inserts a new node into the encoding BST. 
 * Ascending order of token.
*/
static struct bstNode *bstInsert(struct bstNode *node, char *token,
								 char *encoding, int encLength) {
	if (node == NULL) {
		return newBstNode(token, encoding, encLength);
	}
	int comparison = strcmp(token, node->token);
	if (comparison < 0) {
		node->left = bstInsert(node->left, token, encoding, encLength);
	} else if (comparison > 0) {
		node->right = bstInsert(node->right, token, encoding, encLength);
	}

	return node;
}

/**
 * Frees encoding BST and string fields in each node.
*/
static void bstFree(struct bstNode *tree) {
	if (tree == NULL) {
		return;
	}
	bstFree(tree->left);
	bstFree(tree->right);
	free(tree->encoding);
	free(tree->token);
	free(tree);
}

/**
 * Searches the BST and returns the encoding matching with the given token.
*/
static char *getTokenEncoding(struct bstNode *node, char *token,
							  int *encLength) {
	if (node == NULL) return NULL;

	int comparison = strcmp(token, node->token);
	if (comparison < 0) {
		return getTokenEncoding(node->left, token, encLength);
	} else if (comparison > 0) {
		return getTokenEncoding(node->right, token, encLength);
	} else {
		*encLength = node->encLength;
		return node->encoding;
	}
}

/**
 * Adds the encoding patterns of each token in the huffman tree to the encoding
 * BST.
*/
static struct bstNode *addTokenEncodings(struct huffmanTree *huffNode,
										 struct bstNode *encTree,
										 char *currEncoding, int depth) {
	if (huffNode == NULL) return encTree;

	// if leaf: adds null terminator and inserts node the encoding tree
	if (huffNode->token != NULL) { 
		currEncoding[depth] = '\0';
		encTree = bstInsert(encTree, huffNode->token, currEncoding, depth);
	} else {
		// if left child exists, adds 0 to encoding and continues to add
		// token encodings down left branch of tree
		if (huffNode->left != NULL) {
			currEncoding[depth] = '0';
			encTree = addTokenEncodings(huffNode->left, encTree, currEncoding,
										depth + 1);
		}

		// same with right child (adds 1 instead of 0) 
		if (huffNode->right != NULL) {
			currEncoding[depth] = '1';
			encTree = addTokenEncodings(huffNode->right, encTree, currEncoding,
										depth + 1);
		}
	}
	// returns the encoding tree after every leaf of huffman tree has been 
	// visited and the encoding added to encoding tree.
	return encTree;
}
