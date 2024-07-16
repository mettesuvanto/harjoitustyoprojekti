// CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
// Project 1
// Mette Suvanto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 

typedef struct list {
    char *string;
    struct list *pNext;
    struct list *pPrev;
} LIST;

LIST* readFile(char *filename, LIST *pList);
LIST* readStdin(LIST *pList);
int writeFile(char *filename, LIST *pList);
LIST* addNode(LIST *pList, char *row);
int printFile(LIST *pList);
LIST* freeMemory(LIST *pList);

int main(int argc, char *argv[]) {
    LIST *pList = NULL;
    struct stat stat1, stat2;

    // Checking if too many arguments are passed to the program
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    

    if (argc > 1) {
        // Reading the file
        pList = readFile(argv[1], pList);
    } else if (argc == 1) {
        // Reading from stdin
        pList = readStdin(pList);
    }

    if (argc == 3) {
        // Checking whether the input file and output file are the same file.
        if (strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        
        // Checking whether the input file and output file are hardlinked.
        stat(argv[1], &stat1);
        stat(argv[2], &stat2);
        if (stat1.st_ino == stat2.st_ino && stat1.st_dev == stat2.st_dev) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        
        // Writing to file
        writeFile(argv[2], pList);
    } else if (argc < 3) {
        // printing to stdout
        printFile(pList);
    }
    
    // Freeing the memory
    freeMemory(pList);

    return 0;
}

LIST* readFile(char *filename, LIST *pList) {
    FILE *file;
    char *row = NULL;
    size_t len = 0;

    // Opening the file. If error occurs an error message is printed.
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "reverse: cannot open file '%s'\n", filename);
        exit(1);
    }

    // Reading the file to a linked list
    while (getline(&row, &len, file) != -1) {
        pList = addNode(pList, row);
    }

    fclose(file);

    return pList;
}

LIST* readStdin(LIST *pList) {
    char *row = NULL;
    size_t len = 0;

    // Reading rows from stdin to a linked list.
    while (getline(&row, &len, stdin) != -1) {
        pList = addNode(pList, row);
    }

    return pList;
}

int writeFile(char *filename, LIST *pList) {
    FILE *file;

    // Opening the file. If error occurs an error message is printed.
    if ((file = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "reverse: cannot open file '%s'\n", filename);
        exit(1);
    }
    
    // Going to the end of the list
    while (pList->pNext != NULL) {
        pList = pList->pNext;
    }

    // Writing the list from the last node to the first node
    while (pList != NULL) {
        fprintf(file, "%s", pList->string);
        pList = pList->pPrev;
    }
    
    fclose(file);

    return 0;
}

LIST* addNode(LIST *pList, char *row) {
    LIST *pNew, *ptr;

    // Allocating memory for new node
    if ((pNew = (LIST *) malloc(sizeof(LIST))) == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    // Initializing the new node
    pNew->string = strdup(row);
    pNew->pNext = NULL;
    pNew->pPrev = NULL;

    if (pList == NULL) {
        pList = pNew;
    } else {
        ptr = pList;
        while (ptr->pNext != NULL) {
            ptr = ptr->pNext;
        }
        ptr->pNext = pNew;
        pNew->pPrev = ptr;
    }

    return pList;
}

int printFile(LIST *pList) {

    // Going to the end of the list
    while (pList->pNext != NULL) {
        pList = pList->pNext;
    }

    // Printing the list from the last node to the first node
    while (pList != NULL) {
        fprintf(stdout, "%s", pList->string);
        pList = pList->pPrev;
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