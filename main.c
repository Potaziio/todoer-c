/*
    Todoer:
    Finds and reports todos in code files.
    Made by: Potaziio.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define array_size(x) (int)sizeof(x) / (int)sizeof(x[0])

typedef enum {
    CPP,
    C,
    PY,
    NONCODE,
} LANG;

typedef struct {
    char* name;
    LANG lang;
} File;

void CheckFileLanguage(File* file) {
    file->lang = NONCODE;

     char* treat_as_code[] = {
        ".cpp",
        ".c",
        ".py"
    };

    for (int i = 0; i < array_size(treat_as_code); i++) {
        // Checks for existence of substring in string
        if (strstr(file->name, treat_as_code[i])) {
            // If it finds a substring, it compares the substring with the code extensions to find a match
            if (!strcmp(strstr(file->name, treat_as_code[i]), treat_as_code[i])) {
                file->lang = i;
                break;
            }
        }
    }
}

void ReportTodo(File file) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int linecount = 0;

    fp = fopen(file.name, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: File not found: %s\n", file.name);
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, "TODO")) {
            printf("%s:%i -> %s", file.name, linecount, line);
        }
        linecount++;
    }

    fclose(fp);
    if (line)
                        
        free(line);
}

void ReadIgnoreFile(char* start_path, int* ignore_size, char** ignores) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int linecount = 0;

    char buff[128];

    strcpy(buff, start_path);
    strcat(buff, "/");
    strcat(buff, "todoer.ignore");

    fp = fopen(buff, "r");
    if (fp == NULL) {
        *ignore_size = 0;
    } else {
        while ((read = getline(&line, &len, fp)) != -1) {
            if (line[strlen(line)-2] == '/') {
                line[strlen(line)-1] = '\0';
            }

            ignores[linecount] = line;
            ignores[linecount][strlen(ignores[linecount])-1] = 0;

            line = 0;

            linecount++;
        }

        *ignore_size = linecount;

        fclose(fp);
    }
}

void RecursiveSearch(char* start_path, char** ignores, int ignore_size) {
    DIR* d;
    struct dirent* dir;
    d = opendir(start_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
                char buffer[128];
                strcpy(buffer, start_path);
                strcat(buffer, "/");
                strcat(buffer, dir->d_name);

                if (dir->d_type == DT_REG) {
                    File current_file = {
                        .name = buffer,
                    };

                    // Checks language of file and reports a todo

                    CheckFileLanguage(&current_file);
                    if (current_file.lang != NONCODE)
                        ReportTodo(current_file);
                } else if (dir->d_type == DT_DIR) {
                    int found = 0;

                    for (int i = 0; i < ignore_size; i++) {
                        if (strstr(buffer, ignores[i])) {
                            if (!strcmp(strstr(buffer, ignores[i]), ignores[i])) {
                                found = 1;
                                break;
                            }
                        }
                    }
                    
                    if (!found)
                        RecursiveSearch(buffer, ignores, ignore_size);
                }
            }
        }

        closedir(d);
    }
}

int main(int args, char** argc) {
    int ignore_size;
    char** ignores = (char**)malloc(sizeof(char*) * 20);
    ReadIgnoreFile(".", &ignore_size, ignores);
    RecursiveSearch(".", ignores, ignore_size);

    free(ignores);

    return 0; 
}
