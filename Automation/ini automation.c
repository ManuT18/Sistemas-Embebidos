#include <stdbool.h>   // For boolean type
#include <stdio.h>     // For file and console I/O
#include <dirent.h>    // For directory traversal
#include <string.h>    // For string manipulation
#include <sys/stat.h>  // For file status and type
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>    // For dynamic memory allocation


// Dynamic array to store strings (file paths)
typedef struct {
     char **items;
     size_t size;
     size_t capacity;
} StringList;


// Returns 1 if path is a directory, 0 otherwise
int isDirectory(const char *path) {
     struct stat statbuf;
     if (stat(path, &statbuf) != 0)
          return 0;
     return S_ISDIR(statbuf.st_mode);
}


// Initializes a StringList with default capacity
void initStringList(StringList *list) {
     list->size = 0;
     list->capacity = 16;
     list->items = malloc(list->capacity * sizeof(char*));
}


// Adds a string to the StringList, resizing if needed
void addToStringList(StringList *list, const char *str) {
     if (list->size == list->capacity) {
          list->capacity *= 2;
          list->items = realloc(list->items, list->capacity * sizeof(char*));
     }
     list->items[list->size] = strdup(str);
     list->size++;
}


// Frees all memory used by the StringList
void freeStringList(StringList *list) {
     for (size_t i = 0; i < list->size; i++) {
          free(list->items[i]);
     }
     free(list->items);
}


// Recursively traverses directories, adding .c, .cpp, and .ino files to fileList
void listDirRecursive(const char *folder, StringList *fileList) {
     struct dirent *entry;
     DIR *dp = opendir(folder);
     if (dp == NULL) {
          perror("opendir");
          return;
     }
     while ((entry = readdir(dp))) {
          // Skip current and parent directory entries
          if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
               continue;
          char path[1024];
          snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
          if (isDirectory(path)) {
               // Recurse into subdirectory
               listDirRecursive(path, fileList);
          } else {
               // Add only .c, .cpp, and .ino files
               if (path[strlen(path)-1] == 'c' && path[strlen(path)-2] == '.')
                    addToStringList(fileList, path);
               if (path[strlen(path)-1] == 'p' && path[strlen(path)-2] == 'p' && path[strlen(path)-3] == 'c' && path[strlen(path)-4] == '.')
                    addToStringList(fileList, path);
               if (path[strlen(path)-1] == 'o' && path[strlen(path)-2] == 'n' && path[strlen(path)-3] == 'i' && path[strlen(path)-4] == '.')
                    addToStringList(fileList, path);
          }
     }
     closedir(dp);
}


// Returns true if str ends with suffix
bool endsWith(const char *str, const char *suffix) {
     if (!str || !suffix) return false;
     size_t lenstr = strlen(str);
     size_t lensuffix = strlen(suffix);
     if (lensuffix > lenstr) return false;
     return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


// Checks if a given env section is already present in the ini file
bool isEnvInIni(const char *iniPath, const char *envName) {
     FILE *f = fopen(iniPath, "r");
     if (!f) return false;
     char line[512];
     char section[300];
     snprintf(section, sizeof(section), "[env:%s]", envName);
     while (fgets(line, sizeof(line), f)) {
          if (strstr(line, section)) {
               fclose(f);
               return true;
          }
     }
     fclose(f);
     return false;
}


// Appends [env:<FILENAME>] sections to the ini file for each .c/.cpp/.ino file, only if not already present
void appendToIniFile(StringList *fileList, const char *iniPath) {
     FILE *f = fopen(iniPath, "a");
     if (!f) {
          perror("fopen");
          return;
     }
     // If there's not a separation line at the end, add one
     if (fseek(f, -1, SEEK_END) == 0) {
          int c = fgetc(f);
          if (c != '\n') {
               fprintf(f, "\n");
          }
     } 
     for (size_t i = 0; i < fileList->size; i++) {
          const char *fullpath = fileList->items[i];
          // Extract filename from path
          const char *filename = strrchr(fullpath, '/');
          filename = filename ? filename + 1 : fullpath;
          // Only process .c, .cpp, .ino files
          if (endsWith(filename, ".c") || endsWith(filename, ".cpp") || endsWith(filename, ".ino")) {
               char name[256];
               strncpy(name, filename, sizeof(name));
               name[sizeof(name)-1] = '\0';
               char *dot = strrchr(name, '.');
               if (dot) *dot = '\0'; // Remove extension
               // Check if already present
               if (!isEnvInIni(iniPath, name)) {
                    fprintf(f, "[env:%s]\n", name);
               }
          }
     }
     fclose(f);
}


// Prints all lines from the lib_deps section in the given ini file
void printLibDeps(const char *iniPath) {
    FILE *f = fopen(iniPath, "r");
    if (!f) {
        perror("fopen");
        return;
    }
    char line[512];
    int inLibDeps = 0;
    while (fgets(line, sizeof(line), f)) {
        // Detect start of lib_deps section
        char *lib_deps_pos = strstr(line, "lib_deps =");
        if (lib_deps_pos) {
            inLibDeps = 1;
            // Check if there is something after '='
            char *equals = strchr(line, '=');
            if (equals && *(equals + 1) != '\0' && *(equals + 1) != '\n') {
                // Print inline value
                printf("%s", equals + 1);
                inLibDeps = 0;
            }
            continue;
        }
        // If inside lib_deps, print indented lines
        if (inLibDeps) {
            if (line[0] == '[' || line[0] == '\n' || line[0] == ';' || (line[0] != ' ' && line[0] != '\t')) {
                inLibDeps = 0;
            } else {
                printf("%s", line);
            }
        }
    }
    fclose(f);
}


// Entry point: traverses directories, prints found files, and appends to ini file
int main(int argc, char *argv[]) {

     /*
     if (strcmp(argv[1], "--help") || strcmp(argv[1], "-h")) {
          system("cls");
          printf("Usage:\n");
          printf("Scans the specified directories for .c, .cpp, and .ino files and appends [env:<FILENAME>] sections to output.ini\n");
          return 0;
     }*/

     /*
     if (!endsWith(argv[argc], ".ini")) {
          printf("Error: The last argument must be the ini file path and should not be a directory to scan\n");
          return 1;
     }*/

     StringList fileList;
     initStringList(&fileList);

     // If no arguments, use current directory
     if (argc < 2) {
          listDirRecursive(".", &fileList);
     } else {
          // Traverse all directories passed as arguments
          for (int i = 1; i < argc; i++) {
               listDirRecursive(argv[i], &fileList);
          }
     }

     // Print all found files
     printf("Files found:\n");
     for (size_t i = 0; i < fileList.size; i++) {
          printf("%s\n", fileList.items[i]);
     }
     appendToIniFile(&fileList, argv[argc - 1]);

     printLibDeps(argv[argc - 1]);

     freeStringList(&fileList);
     return 0;
     
}