#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#include "cd.hpp"

static char prev_dir[PATH_MAX] = {0};

void init_cd() {
    getcwd(prev_dir, sizeof(prev_dir));
}

char* expandENV(const char* path) {
    static char buffer[PATH_MAX];
    if (path[0] == '$') {
        char *val = getenv(path + 1);
        if (val) {
            snprintf(buffer, sizeof(buffer), "%s", val);
            return buffer;
        }
    }
    return (char*)path;
}

char* strip_quotes(char* path) {
    size_t len = strlen(path);
    if (len >= 2 && path[0] == '"' && path[len - 1] == '"') {
        path[len - 1] = '\0';
        return path + 1;
    }
    return path;
}

int cd_command(char** args) {
    char currDir[PATH_MAX];
    getcwd(currDir, sizeof(currDir));

    // CASE 1: cd OR cd ~
    if (args[1] == NULL || strcmp(args[1], "~") == 0) {
        const char *home = getenv("HOME");
        if (!home) home = "/";
        if (chdir(home) == 0) {
            strcpy(prev_dir, currDir);
            return 0;
        } else {
            perror("cd");
            return -1;
        }
    }

    // CASE 2: cd -
    if (strcmp(args[1], "-") == 0) {
        if (prev_dir[0] == '\0') {
            printf("cd: No Previous Directory\n");
            return -1;
        }
        printf("%s\n", prev_dir);
        char temp[PATH_MAX];
        strcpy(temp, prev_dir);

        if (chdir(temp) == 0) {
            strcpy(prev_dir, currDir);
            return 0;
        }
        perror("cd");
        return -1;
    }

    // CASE 3: cd -P DIR
    if (strcmp(args[1], "-P") == 0) {

        // No DIR → treat like cd to HOME
        if (args[2] == NULL) {
            const char *home = getenv("HOME");
            if (!home) home = "/";
            if (chdir(home) == 0) {
                strcpy(prev_dir, currDir);
                return 0;
            }
            perror("cd");
            return -1;
        }

        char resolved[PATH_MAX];
        if (!realpath(args[2], resolved)) {
            perror("cd");
            return -1;
        }

        if (chdir(resolved) == 0) {
            strcpy(prev_dir, currDir);
            return 0;
        }
        perror("cd");
        return -1;
    }

    // DEFAULT: cd PATH (with ENV expansion and quotes)
    char buf[PATH_MAX];
    strcpy(buf, args[1]);

    char *path = strip_quotes(buf);
    path = expandENV(path);

    if (chdir(path) != 0) {
        perror("cd");
        return -1;
    }

    strcpy(prev_dir, currDir);
    return 0;
}



