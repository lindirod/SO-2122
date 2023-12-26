#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

char* workFile(char* text) 
{
    char* res = (char*)malloc(10000000 * sizeof(char));
    int i = 0, j = 0;
    while (i < strlen(text)) 
    {
        if (text[i] == ' ' || text[i] == '\n') 
        {
            res[j] = '$';
            i++;
            j++;
        }
        else 
        {
            res[j] = text[i];
            i++;
            j++;
        }
    }
    return res;
}
void workLine(char* line) 
{
    for (int i = 0; i < strlen(line); i++) 
    {
        if (line[i] == '$')
            line[i] = ' ';
    }
}

void getMatrixSize(char * line, int *lines, int *columns) 
{
    char * token = strtok(line, "x");
    *lines = atoi(token);
    while (token != NULL) 
        {
            token = strtok(NULL, "x");
            if (token != NULL) 
                *columns = atoi(token);
        }
}

void initializeMatrix(int*** matrix, int lines, int columns) 
{
    *matrix = malloc(sizeof(int*) * lines);
    for (int i = 0; i < lines; i++) 
    {
        (*matrix)[i] = malloc(sizeof(int) * columns);
    }
}

void fillMatrix(char * text, int*** matrix, int lines, int columns) 
{
    char * token = strtok(text, " ");
    int lineCount = 0;
    int columnCount = 0;
    while (token != NULL) 
    {
        token = strtok(NULL, " ");
        if (token != NULL) 
        {
            if (columnCount == columns) 
            {
                columnCount = 0;
                lineCount++;
                if (lineCount == lines) 
                    break;
            }
            (*matrix)[lineCount][columnCount] = atoi(token);
            columnCount++;
        }
    }
}

int main(int argc, char** argv)
{
    char* filename1 = argv[1];
    char* filename2 = argv[2];
    int fd1 = open(filename1, O_RDONLY, S_IRUSR);
    int fd2 = open(filename2, O_RDONLY, S_IRUSR);
    struct stat sb1;
    struct stat sb2;
    int lines = 0, columns = 0;
    int lines2 = 0, columns2 = 0;

    /* READING FILE */
    if (fstat(fd1, &sb1) == -1) 
    {
        perror("Couldn't get file 1 size.\n");
    }
    if (fstat(fd2, &sb2) == -1) 
    {
        perror("Couldn't get file 2 size.\n");
    }
    char * file1 = mmap(NULL, sb1.st_size, PROT_READ, MAP_PRIVATE, fd1, 0);
    char * file2 = mmap(NULL, sb2.st_size, PROT_READ, MAP_PRIVATE, fd2, 0);
    close(fd1);
    close(fd2);

    /* GETTING FINAL MATRIX SIZE */
    char aux[sb1.st_size];
    for (int i = 0; i < sb1.st_size; i++)
    {
        strcpy(aux,file1);
    }
    getMatrixSize(aux, &lines, &columns);


    char aux2[sb2.st_size];
    for (int i = 0; i < sb2.st_size; i++)
    {
        strcpy(aux2,file2);
    }
    getMatrixSize(aux2, &lines2, &columns2);

    if (lines != lines2 || columns != columns2) 
    {
        printf("Matrix sizes are different\n");
        return EXIT_FAILURE;
    }
    

    /* INITIALIZING MATRICES */
    int** matrix1;
    initializeMatrix(&matrix1, lines, columns);
    int** matrix2;
    initializeMatrix(&matrix2, lines, columns);
    int* matrixRes = malloc((lines * columns) * sizeof(int));
    matrixRes = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    /* GETTING AND STORING MATRIX VALUES */ 
    char* text1 = workFile(file1);
    workLine(text1);
    fillMatrix(text1 ,&matrix1, lines, columns);
    char* text2 = workFile(file2);
    workLine(text2);
    fillMatrix(text2 ,&matrix2, lines, columns);

    /* CREATING CHILD PROCESSES */
    int parentPID = getpid();
    int id = fork();
    
    for (int i = 0; i < columns - 1; i++) 
    {
        if (id != 0) 
            id = fork();    
    }
    
    /* SUMMING THE MATRICES */
    for (int i = 0; i < columns; i++) 
    {
        if (getpid() == parentPID + 1 + i) 
        {
            for (int j = 0; j < lines; j++) 
            {
                matrixRes[j * columns + i] = matrix1[j][i] + matrix2[j][i];
            }
            return 0;
        }
        if (id != 0)
            wait();
    }

    /* PRINTING RESULT */
    printf("%dx%d\n", lines, columns);
    for (int i = 0; i < lines; i++) 
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%d ", matrixRes[i * columns + j]);
        }
        printf("\n");
    }

    for (int i = 0; i < lines; i++) 
    {
        free(matrix1[i]);
    }
    free(matrix1);

    for (int i = 0; i < lines; i++) 
    {
        free(matrix2[i]);
    }
    free(matrix2);

    for (int i = 0; i < lines; i++)
    {
        int err3 = munmap(matrixRes, sizeof(int));
        if(err3 != 0)
        {
        printf("UnMapping Failed\n");
        return 1;
        }
    }
    
    int err = munmap(file1, sb1.st_size);
    if(err != 0){
        printf("UnMapping Failed\n");
        return 1;
    }
    int err2 = munmap(file2, sb2.st_size);
    if(err2 != 0){
        printf("UnMapping Failed\n");
        return 1;
    }

    return EXIT_SUCCESS;
}