#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <libgen.h>
#include <ctype.h>


// Функция подсчёт всех значений.
int countAll(int inputFd, int* countWords, int* countLines, int* countSymbols)
{
    int countBytesRead = 0;
    char buffer[256] = ""; 

    int isLastSpace = 1;

    do
    {
        if ((countBytesRead = read(inputFd, buffer, 256)) < 0)
        {
            return -1;
        } 

        for (int i = 0; i < countBytesRead; ++i)
        {
            if (isspace(buffer[i]))
            {
                if (!isLastSpace)
                {
                    (*countWords)++;
                }
                isLastSpace = 1;
            }
            else
            {
                isLastSpace = 0;
            }

            if (buffer[i] == '\n')
            {
                (*countLines)++; 
            }
        }

        (*countSymbols) += countBytesRead;   
    }
    while (countBytesRead != 0);

    if (!isLastSpace)
    {
        (*countWords)++;
    }
}

// Функция wc.
int wc(char* filePath, int* options, int* lines, int* words, int* symbols)  
{

    int countWords = 0;
    int countSymbols = 0;
    int countLines = 0;
    int inputFd = STDIN_FILENO;

    if (strcmp(filePath, "-") != 0)
    {
        if ((inputFd = open(filePath, O_RDONLY)) == -1)
        {
            return -2;
        }
    }

    if (countAll(inputFd, &countWords, &countLines, &countSymbols) == -1)
    {
        printf("Error while reading file given: %s\n", filePath);
        return -1;
    }

    if (strcmp(filePath, "-") != 0 && close(inputFd) != 0)
    {
        printf("Error while closing file: %s\n", filePath);
        return -3;
    }

    int answer[3] = {countLines, countWords, countSymbols};
    
    for (int i = 0; i < 3; ++i)
    {
        if (options[i])
        {
            printf("%d ", answer[i]);
        }
    }
    printf("%s\n", basename(filePath));

    (*words) += countWords;
    (*lines) += countLines;
    (*symbols) += countSymbols;

    return 0;    
}



int main(int argc, char** argv)
{
    // Опции: -l, -w, -c.
    int options[3] = {0,0,0};
    int lines = 0;
    int words = 0;
    int symbols = 0;
    int countFiles = 0;
    int countArguments = 0;

    int opt = 0;
    
    // Определим какая(ие) опция(ии) используются.
    while ((opt = getopt(argc, argv, "lwc")) != -1)
    {
        switch (opt)
        {
            case 'l':
                options[0] = 1;
                countArguments++;
                break;

            case 'w':
                options[1] = 1;
                countArguments++;
                break;

            case 'c':
                options[2] = 1;
                countArguments++;
                break;

            case '?':
                printf("Wrong option.\n");
                return 1;
        }
    }   
    
    // Если не была введена ни одна опция. 
    if (countArguments == 0)
    {
        countArguments = 3;
        for (int i = 0; i < 3; ++i)
        {
            options[i] = 1;
        }
    }
    
    for (int i = optind; i < argc; ++i)
    {
        if (argv[i][0] != '-' || !strcmp(argv[i], "-"))
        {
            if (wc(argv[i], options, &lines, &words, &symbols) != 0)
            {
                printf("Error while counting file: %s\n", argv[i]);
            }
            countFiles++;   
        }
    }

    // Если не передаются аргументы.
    if (countFiles == 0)
    {
        if (wc("-", options, &lines, &words, &symbols) != 0)
        {
            printf("Error while default reading from stdin\n");
        }
        else
        {
            countFiles++;
        }
    }

    // Если передаётся более одного аргумента.
    if (countFiles > 1)
    {
        int answer[3] = {lines, words, symbols};
        for (int i = 0; i < 3; ++i)
        {
            if (options[i])
            {
                printf("%d ", answer[i]);
            }
        }
        printf("\n");
    }

    return 0;
}

