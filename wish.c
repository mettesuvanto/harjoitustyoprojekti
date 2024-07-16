// CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
// Project 3
// Mette Suvanto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

typedef struct list {
    char *string;
    struct list *pNext;
} LIST;

void readFile(char *filename, LIST *pInput, LIST *pPath, char *error_message);
LIST* parseInput(LIST *pInput, char *input, char *error_message);
LIST* executeCommand(LIST *pInput, LIST *pPath, char *error_message);
LIST* addNode(LIST *pInput, char *input, char *error_message);
LIST* freeMemory(LIST *pInput);

int main(int argc, char *argv[]) {
    LIST *pInput = NULL;
    LIST *pPath = NULL;
    char error_message[30] = "An error has occurred\n";

    // Initializing the path variable
    pPath = addNode(pPath, "/bin", error_message);

    if (argc > 2) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (argc == 1) {
        while (1) {
            // Printing the prompt
            printf("wish> ");

            size_t len = 0;
            char *input = NULL;

            // Reading the input on the line to a linked list where the first node is the  command and 
            // the other nodes are the arguments of the command.
            getline(&input, &len, stdin);

            // Parsing the input
            pInput = parseInput(pInput, input, error_message);

            // Executing the command specified on the line of input
            if (pInput != NULL) {
                pPath = executeCommand(pInput, pPath, error_message);
            }

            pInput = freeMemory(pInput);

        }
    } else if (argc == 2) {
        readFile(argv[1], pInput, pPath, error_message); 
    }

    return 0;
}

void readFile(char *filename, LIST *pInput, LIST *pPath, char *error_message) {
    FILE *file;
    char *row = NULL;
    size_t len = 0;

    // Opening the file. If error occurs an error message is printed.
    if ((file = fopen(filename, "r")) == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    // Reading the file, parsing the input and executing the commands line by line.
    while (getline(&row, &len, file) != -1) {
        pInput = parseInput(pInput, row, error_message);
        if (pInput != NULL) {
            pPath = executeCommand(pInput, pPath, error_message);
        }
        pInput = freeMemory(pInput);
    }

    fclose(file);

    return;
}

LIST* parseInput(LIST *pInput, char *input, char *error_message) {

    input = strtok(input, "\n");
    input = strtok(input, " ");

    while (input != NULL) {
        pInput = addNode(pInput, input, error_message);
        input = strtok(NULL, " ");
    }

    // Printing a message if no command is given.
    if (pInput == NULL) {
        printf("At this point, wish is running, and ready to accept commands. Type away!\n");
        return pInput;
    }
    
    return pInput;
}

LIST* executeCommand(LIST *pInput, LIST *pPath, char *error_message) {
    if (strcmp(pInput->string, "exit") == 0) { // Exiting the porgram if the given input is "exit".
        if (pInput->pNext != NULL) { // If any arguments are given to exit command, an error message is printed
            write(STDERR_FILENO, error_message, strlen(error_message));
            return pPath;
        }
        exit(0);
    } else if (strcmp(pInput->string, "cd") == 0) { // Executing the built in command "cd".
        if (pInput->pNext == NULL) { // If 0 arguments are given, an error message is printed.
            write(STDERR_FILENO, error_message, strlen(error_message));
            return pPath;
        } else if (pInput->pNext->pNext == NULL) { // If only one argument is given, the command is ecxecuted
            if (chdir(pInput->pNext->string) != 0) {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        } else { // If >1 arguments are given, an error message is printed.
            write(STDERR_FILENO, error_message, strlen(error_message));
            return pPath;
        }
        
    } else if (strcmp(pInput->string, "path") == 0) { // Executing the built in command "path".
        pPath = freeMemory(pPath);
        while (pInput->pNext != NULL) {
            pInput = pInput->pNext;
            pPath = addNode(pPath, pInput->string, error_message);
        }
    } else {
        LIST *ptr = pPath;
        int error = 1;
        char *pathname = NULL;
        while (ptr != NULL) {
            // Calculating the amount of memory needed to allocate for the "pathname" variable
            size_t len1 = strlen(ptr->string);
            size_t len2 = strlen("/");
            size_t len3 = strlen(pInput->string);
            size_t len = len1 + len2 + len3;

            // Allocating the memory
            if ((pathname = (char *) malloc(len)) == NULL) {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            }

            // Constructing the path where to check for the command
            strcpy(pathname, ptr->string);
            strcat(pathname, "/");
            strcat(pathname, pInput->string);

            if (access(pathname, X_OK) == 0) {
                error = 0;
                pid_t pid = fork();
                if (pid == -1) {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                } else if (pid == 0) {
                    LIST *ptrInput = pInput;

                    // Counting the size of an array needed for the arguments that are given to the execv command.
                    int count = 0;
                    while (ptrInput != NULL) {
                        count++;
                        ptrInput = ptrInput->pNext;
                    }

                    char *args[count + 1];
                    int i = 0;
                    // Constructing the array
                    ptrInput = pInput;
                    while (ptrInput != NULL) {
                        args[i] = ptrInput->string;
                        i++;
                        ptrInput = ptrInput->pNext;
                    }

                    args[count] = NULL; // The list of arguments must end with NULL.
                    
                    execv(pathname, args);
                    write(STDERR_FILENO, strerror(errno), strlen(strerror(errno)));
                    free(pathname);
                    exit(0);
                } else {
                    // Waiting for the child process to stop or terminate
                    wait(NULL);
                }
                break;
            }
            
            ptr = ptr->pNext;
        }
        if (error == 1) {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    return pPath;
}

LIST* addNode(LIST *pInput, char *input, char *error_message) {
    LIST *pNew, *ptr;

    // Allocating memory for new node
    if ((pNew = (LIST *) malloc(sizeof(LIST))) == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    // Initializing the new node
    pNew->string = strdup(input);
    pNew->pNext = NULL;

    if (pInput == NULL) {
        pInput = pNew;
    } else {
        ptr = pInput;
        while (ptr->pNext != NULL) {
            ptr = ptr->pNext;
        }
        ptr->pNext = pNew;
    }

    return pInput;
}

LIST* freeMemory(LIST *pInput) {
    LIST *ptr = pInput;

    while (ptr != NULL) {
        pInput = ptr->pNext;
        free(ptr);
        ptr = pInput;
    }
    
    return pInput;
}