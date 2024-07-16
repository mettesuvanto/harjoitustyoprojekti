// CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
// Project 2, my grep
// Mette Suvanto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list {
    char *string;
    struct list *pNext;
} LIST;

LIST* readFile(char *filename, LIST *pList, char *str);
LIST* readStdin(LIST *pList, char *str);
LIST* addNode(LIST *pList, char *row);
int printFile(LIST *pList);
LIST* freeMemory(LIST *pList);

int main(int argc, char *argv[]) {
    LIST *pList = NULL;

    if (argc == 3) {
        pList = readFile(argv[2], pList, argv[1]);
    } else if (argc == 2) {
        pList = readStdin(pList, argv[1]);
    } else if (argc == 1) {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    printFile(pList);
    freeMemory(pList);
    
    return 0;
}

LIST* readFile(char *filename, LIST *pList, char *str) {
    FILE *file;
    char *row = NULL;
    size_t len = 0;

    // Opening the file. If error occurs an error message is printed.
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "my-grep: cannot open file\n");
        exit(1);
    }

    // Reading the row from a file to a linked list, if the search term occurs in it.
    while (getline(&row, &len, file) != -1) {
        if (strstr(row, str) != NULL) {
            pList = addNode(pList, row);
        }
    }

    fclose(file);

    return pList;
}

LIST* readStdin(LIST *pList, char *str) {
    char *row = NULL;
    size_t len = 0;

    // Reading the row from stdin to a linked list, if the search term occurs in it.
    while (getline(&row, &len, stdin) != -1) {
        if (strstr(row, str) != NULL) {
            pList = addNode(pList, row);
        }
    }

    return pList;
}

LIST* addNode(LIST *pList, char *row) {
    LIST *pNew, *ptr;

    // Allocating memory for new node
    if ((pNew = (LIST *) malloc(sizeof(LIST))) == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    // Initializing the new node
    pNew->string = strdup(row);
    pNew->pNext = NULL;

    if (pList == NULL) {
        pList = pNew;
    } else {
        ptr = pList;
        while (ptr->pNext != NULL) {
            ptr = ptr->pNext;
        }
        ptr->pNext = pNew;
    }

    return pList;
}

int printFile(LIST *pList) {
    // Printing the list
    while (pList != NULL) {
        fprintf(stdout, "%s", pList->string);
        pList = pList->pNext;
    }
    
    return 0;
}

LIST* freeMemory(LIST *pList) {
    LIST *ptr = pList;

    while (ptr != NULL) {
        pList = ptr->pNext;
        free(ptr);
        ptr = pList;
    }
    
    return pList;
}