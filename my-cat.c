// CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
// Project 2, my cat
// Mette Suvanto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readFile(char *filename);

int main(int argc, char *argv[]) {

    // Reading the given files and printing their contents.
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            readFile(argv[i]);
        }
    }
    
    return 0;
}

int readFile(char *filename) {
    FILE *file;
    char *row = NULL;
    size_t len = 0;

    // Opening the file. If error occurs an error message is printed.
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "my-cat: cannot open file\n");
        exit(1);
    }

    // Reading the file and printing out each line
    while (getline(&row, &len, file) != -1) {
        fprintf(stdout, "%s", row);
    }

    fclose(file);

    return 0;
}