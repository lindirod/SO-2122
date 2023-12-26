#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static int received = 1;

char* aux (char* word, char list[100][100], int size) {
    for (int i = 0; i < size; i+=2) {
        if (strncmp(word, list[i], strlen(list[i])) == 0) {
            if (strlen(word) == strlen(list[i])) {
                return strcat(list[i+1], " ");
            }
            else {
                char* res = (char*)malloc(sizeof(char) * 100000);
                strcpy(res, list[i+1]);
                for (int j = strlen(list[i]); j < strlen(word); j++)
                    strcat(res, &word[j]);

                return strcat(res, " ");
            }
        }
        if (strncmp(word, list[i+1], strlen(list[i+1])) == 0) {
            if (strlen(word) == strlen(list[i+1])) { 
                return strcat(list[i], " ");
            }
            else {
                char* res = (char*)malloc(sizeof(char) * 100000);
                strcpy(res, list[i]);
                for (int j = strlen(list[i+1]); j < strlen(word); j++)
                    strcat(res, &word[j]);

                return strcat(res, " ");
            }
        }
    }
    char* res = (char*)malloc(strlen(word) + 1);
    strcpy(res, word);
    return strcat(res, " ");
}

char* cyphering(char* text, char cypher[100][100], int size) {
    char* texto = (char*)malloc(sizeof(char) * 1000);
    free(texto);
    char* word = strtok(text, " ");
    while (word != NULL) {
        strcat(texto, aux(word, cypher, size));

        word = strtok(NULL, " ");
    }

    return texto;
}

void sig_handler() {
    received = 0;
}

int main(int argc, char* argv[]) {
    if (argc != 1) {
        perror("Wrong number of parameters");
        return EXIT_FAILURE;
    }
    char* fullText = (char*)malloc(sizeof(char) * 10000);
    char cypher[100][100];
    int pid, nBytes;
    int fd[2], fd2[2];
    int i = 0;

    if (pipe(fd) < 0) {
        perror("Error with pipe 1");
        return EXIT_FAILURE;
    }

    if (pipe(fd2) < 0) {
        perror("Error with pipe 2");
        return EXIT_FAILURE;
    }
    
    signal(SIGUSR1, sig_handler);

    pid = fork();

    if (pid < 0) {
        perror("Error in fork");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        while(received);
        close(fd[1]);
        nBytes = read(fd[0], fullText, sizeof(char) * 10000);
        FILE* file = fopen("cypher.txt", "r+");
        if (file == NULL) {
            perror("File can't open");
            return EXIT_FAILURE;
        } 

        while(fscanf(file, "%s", cypher[i]) == 1) {
            i++;
        }
        fclose(file);

        if (nBytes < 0) {
            perror("Error reading text.");
            return EXIT_FAILURE;
        }
        char* cyphered = cyphering(fullText, cypher, i);
        free(fullText);
        close(fd2[0]);
        write(fd2[1], cyphered, strlen(cyphered));
        exit(0);
    }
    else {
        close(fd[0]);
        read(STDIN_FILENO, fullText, sizeof(char) * 10000);
        write(fd[1], fullText, strlen(fullText));
        kill(pid, SIGUSR1);
        wait();
        char* cyphered = (char*)malloc(sizeof(char) * 1000000);
        close(fd2[1]);
        nBytes = read(fd2[0], cyphered, sizeof(char) * 1000000);
        if (nBytes < 0) {
            perror("Error reading cyphered text.");
            return EXIT_FAILURE;
        }
        printf("%s\n", cyphered);
    }
}