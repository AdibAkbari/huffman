
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "File.h"
#include "huffman.h"

static struct huffmanTree *readHuffmanTree(char *filename);
static struct huffmanTree *readTree(FILE *fp, char str[]);
static struct huffmanTree *newHuffmanNode(char *str, int freq);
static void freeHuffmanTree(struct huffmanTree *t);

static void printTreeToHtml(struct huffmanTree *t, char *filename);
static void printNodes(struct huffmanTree *t, FILE *fp);
static void doPrintNodes(struct huffmanTree *t, FILE *fp, int *id);
static void printEdges(struct huffmanTree *t, FILE *fp);
static void doPrintEdges(struct huffmanTree *t, FILE *fp, int *id);
static void printEscapedToken(char *token, FILE *fp);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <tree file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct huffmanTree *tree = readHuffmanTree(argv[1]);
    printTreeToHtml(tree, argv[2]);
    freeHuffmanTree(tree);
}

////////////////////////////////////////////////////////////////////////

static struct huffmanTree *readHuffmanTree(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "error: failed to open '%s' for reading\n", filename);
		exit(EXIT_FAILURE);
	}

	char str[MAX_TOKEN_LEN + 1];
	struct huffmanTree *tree = readTree(fp, str);
	fclose(fp);
	return tree;
}

static struct huffmanTree *readTree(FILE *fp, char str[]) {
	struct huffmanTree *t = newHuffmanNode(NULL, 0);

	int c = fgetc(fp);

	if (c == '(') {
		t->left = readTree(fp, str);
		fgetc(fp); // should always be a ','
		t->right = readTree(fp, str);
		fgetc(fp); // should always be a ')'
	} else {
		int i = 0;
		while (c != ',' && c != ')') {
			if (c == '\\') {
				c = fgetc(fp);
			}
			str[i++] = c;
			c = fgetc(fp);
		}

		ungetc(c, fp);
		str[i] = '\0';
		t->token = strdup(str);
	}

	return t;
}

static struct huffmanTree *newHuffmanNode(char *str, int freq) {
	struct huffmanTree *new = malloc(sizeof(struct huffmanTree));
	if (new == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	new->token = str;
	new->freq = freq;
	new->left = NULL;
	new->right = NULL;
	return new;
}

static void freeHuffmanTree(struct huffmanTree *t) {
	if (t != NULL) {
		freeHuffmanTree(t->left);
		freeHuffmanTree(t->right);
		free(t->token);
		free(t);
	}
}

////////////////////////////////////////////////////////////////////////

static void printTreeToHtml(struct huffmanTree *t, char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "error: failed to open '%s' for writing\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(
        fp,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <title>Huffman tree</title>\n"
        "  <meta name=\"viewport\" content=\"width=device-width, user-scalable=no, initial-scale=1, maximum-scale=1\">\n"
        "\n"
        "  <script src=\"https://unpkg.com/cytoscape/dist/cytoscape.min.js\"></script>\n"
        "  <script src=\"https://unpkg.com/dagre@0.7.4/dist/dagre.js\"></script>\n"
        "  <script src=\"cytoscape-dagre.js\"></script>\n"
        "\n"
        "  <link rel=\"stylesheet\" href=\"./style.css\">\n"
        "\n"
        "  <script>\n"
        "    var elements = {\n"
    );

    printNodes(t, fp);
    printEdges(t, fp);

    fprintf(
        fp,
        "    };\n"
        "  </script>\n"
        "\n"
        "  <script src=\"./visualiser.js\"></script>\n"
        "</head>\n"
        "\n"
        "<body>\n"
        "  <h1>Huffman tree</h1>\n"
        "  <div id=\"cy\"></div>\n"
        "</body>"
        "</html>"
    );

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////

static void printNodes(struct huffmanTree *t, FILE *fp) {
    int id = 0;
    fprintf(fp, "      nodes: [\n");
    doPrintNodes(t, fp, &id);
    fprintf(fp, "      ],\n");
}

static void doPrintNodes(struct huffmanTree *t, FILE *fp, int *id) {
    if (t == NULL) {
        return;
    }

    (*id)++;
    int myId = *id;

    if (t->left == NULL && t->right == NULL) {
        fprintf(fp, "        ");
        fprintf(fp, "{data: {id: 'n%d', token: '", myId);
        printEscapedToken(t->token, fp);
        fprintf(fp, "'}, classes: ['leaf']},\n");
        return;
    }

    fprintf(fp, "        {data: {id: 'n%d'}},\n", myId);
    doPrintNodes(t->left, fp, id);
    doPrintNodes(t->right, fp, id);
}

static void printEdges(struct huffmanTree *t, FILE *fp) {
    int id = 0;
    fprintf(fp, "      edges: [\n");
    doPrintEdges(t, fp, &id);
    fprintf(fp, "      ],\n");
}

static void doPrintEdges(struct huffmanTree *t, FILE *fp, int *id) {
    if (t == NULL) {
        return;
    }

    (*id)++;
    int myId = *id;

    if (t->left == NULL && t->right == NULL) {
        return;
    }

    fprintf(
        fp,
        "        {data: {source: 'n%d', target: 'n%d', label: '0'}},\n",
        myId, *id + 1);

    doPrintEdges(t->left, fp, id);

    fprintf(
        fp,
        "        {data: {source: 'n%d', target: 'n%d', label: '1'}},\n",
        myId, *id + 1
    );

    doPrintEdges(t->right, fp, id);
}

////////////////////////////////////////////////////////////////////////

static void printEscapedToken(char *token, FILE *fp) {
    switch (token[0]) {
        case '\n': fprintf(fp, "\\\\n");     break;
        case '\r': fprintf(fp, "\\\\r");     break;
        case '\t': fprintf(fp, "\\\\t");     break;
        case '\'': fprintf(fp, "\\\'");      break;
        case '\\': fprintf(fp, "\\\\");      break;
        default:   fprintf(fp, "%s", token); break;
    }
}

////////////////////////////////////////////////////////////////////////
