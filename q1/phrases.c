#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char* workFile(char* text) {
    char* res = (char*)malloc(10000000 * sizeof(char));
    int i = 0, j = 0;
    while (i < strlen(text)) {
        if (text[i] == '.' || text[i] == '?' || text[i] == '!') {
            res[j] = text[i];
            res[j+1] = ' ';
            i += 2;
            j += 2;
        }
        else if (text[i] == ' ' || text[i] == '\n') {
            res[j] = '$';
            i++;
            j++;
        }
        else {
            res[j] = text[i];
            i++;
            j++;
        }
    }
    return res;
}

void workLine(char* line) {
    for (int i = 0; i < strlen(line); i++) {
        if (line[i] == '$')
            line[i] = ' ';
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2 || argc == 3) {
        char* filename;
        if (argc == 2) filename = argv[1];
        else filename = argv[2];
        FILE* file = fopen(filename, "r+");
        char save[100];
        char* fullText = (char*)malloc(10000000);
        int counter = 0;
        char* token;

        if (file == NULL) {
            perror("File can't open");
            return EXIT_FAILURE;
        }

        while (fgets(save, 100, file) != NULL) {
            strcat(fullText, save);
        }

        char* text = workFile(fullText);

        token = strtok(text, " ");

        while (token != NULL) {
            counter++;

            workLine(token);

            if (!strcmp(argv[1], "-l"))
                printf("[%d] %s\n", counter, token);

            token = strtok(NULL, " ");
        }
        if (strcmp(argv[1], "-l"))
            printf("%d\n", counter);
        return EXIT_SUCCESS;
    }
    else {
        perror("usage: phrases [-1] file");
        return EXIT_FAILURE;
    }
}