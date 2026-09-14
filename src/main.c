#define _XOPEN_SOURCE 500
#define PROJECT_NAME "dearsize"

// Includes --------
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"

// ---------------

typedef struct {
    bool verbose;
    bool explicit;
    bool binary;
    bool decimal;
    bool symlinkstatus;
    bool detailed;
} Status;

typedef struct {
    int items;
    int dirs;
    int files;
    int unReadableDirs;
    int maxDepth;
} contentStats;

int SPACE = 10;
off_t totalBytes = 0;
double conversionBasis = 1000.0;
Status status = {false, true, false, true, false, false};
contentStats dirStats = {0, 0, 0, 0, 0};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s: Expected a Directory name, but got None\n", PROJECT_NAME);
        printf("\n");
        help();
        exit(0);
    }

    // Goal: find the exact total size of the dir
    // 1. get the name of the target dir => argv at index 1
    // 2. iterate on the dir, sub dirs, and get the size of each file.
    // 3. Print the Size Information to stdout

    char *sourceDir = argv[1];

    if (argc >= 2) {
        printf("\n");
        parseFlags(argv, argc);
    }

    if (!strcmp(sourceDir, "None")) {
        printf("No source directory provided\n");
        return EXIT_SUCCESS;
    }

    setConversion();
    if (nftw(sourceDir, recordFilesAndData, 20, FTW_PHYS | FTW_MOUNT) == -1) {
        printf("\n");
        perror(sourceDir);
        printf("    -%s", PROJECT_NAME);

        exit(EXIT_FAILURE);
    }

    displayStats(sourceDir);

    return EXIT_SUCCESS;
}

void setConversion() {
    if (status.binary) {
        conversionBasis = 1024.0;
        status.decimal = false;
    }
}

int findMax(int num1, int num2) { return num1 > num2 ? num1 : num2; }

int recordFilesAndData(const char *fpath, const struct stat *fileStatus,
                       int typeflag, struct FTW *ftwbuf) {

    hrSize convSize;
    dirStats.maxDepth = findMax(dirStats.maxDepth, ftwbuf->level);

    dirStats.items += 1;

    if (status.verbose) {
        switch (typeflag) {
        case FTW_F:
            convSize = convertBytes(fileStatus->st_size);
            printf("%.2lf%c  : %s\n", convSize.size, convSize.unit, fpath);
            break;

        case FTW_NS:
            printf("Accs-Denied: %s", fpath);
        }
    }

    if (typeflag == FTW_SL) {
    }

    if (typeflag == FTW_DNR) {
    }

    switch (typeflag) {

    case FTW_D:
        dirStats.dirs += 1;

        if (ftwbuf->level > 0) {
            // totalBytes += (fileStatus->st_blocks * 512);
            totalBytes += fileStatus->st_size;
        }
        return 0;

    case FTW_SL:
        printf("Sym Link, skipping: %s\n", fpath);
        break;

    case FTW_F:
        dirStats.files += 1;

        if (ftwbuf->level > 0) {
            // totalBytes += (fileStatus->st_blocks * 512);
            totalBytes += fileStatus->st_size;
        }
        break;

    case FTW_NS:
        dirStats.files += 1;
        break;

    case FTW_DNR:
        dirStats.dirs += 1;
        dirStats.unReadableDirs += 1;

        fprintf(stderr, "Acs-Denied: %s\n", fpath);
        printf("\n");
        break;
    }

    return 0;
}

void parseFlags(char *args[], int length) {
    int opt;

    while ((opt = getopt(length, args, "XvVbBsShH")) != -1) {
        switch (opt) {
        case 'X':
            status.explicit = false;
            break;

        case 'v':
        case 'V':
            status.verbose = true;
            break;

        case 'b':
        case 'B':
            status.binary = true;
            status.decimal = false;
            break;

        case 's':
        case 'S':
            status.symlinkstatus = true;
            break;

        case 'h':
        case 'H':
            help();
            break;

        case '?':
            return;
        }
    }
}

void help() {
    char *helpFilePath = "/home/arcatra/Dutils/dearsize/resources/help.txt";
    FILE *fstream = fopen(helpFilePath, "r");

    if (fstream == NULL) {
        printf("Cannot print the help, error occured\n");
        printf("May be the file path \"%s\" doesn't exists", helpFilePath);

        return;
    }

    char buf[256];
    while (fgets(buf, sizeof(buf), fstream) != NULL) {
        printf("%s", buf);
    }

    fclose(fstream);
}

hrSize convertBytes(off_t bytes) {
    double Bytes = bytes;
    hrSize convSize = {Bytes, 'B'};

    if (Bytes < conversionBasis) {
        return convSize;
    }

    char units[] = {'B', 'K', 'M', 'G', 'T', 'P'};
    int uIndex = 0;

    while (Bytes >= conversionBasis) {
        Bytes /= conversionBasis;
        uIndex++;
    }
    convSize.size = Bytes;
    convSize.unit = units[uIndex];

    return convSize;
}

void displayContentInfo() {
    int contentStats[] = {
        dirStats.items,    dirStats.dirs,           dirStats.files,
        dirStats.maxDepth, dirStats.unReadableDirs,
    };
    char *contentMetrics[] = {
        "Items", "DIRs", "Files", "level(s) deep", "unreadable DIRs",
    };

    if (status.explicit) {
        printf("Directory Information:\n\n");
    }

    printf("%d %s", contentStats[0], contentMetrics[0]);
    for (int index = 1; index < 5; index++) {
        if (contentStats[index] == 0) {
            continue;
        }

        // int width = SPACE + strlen(contentMetrics[index]);
        printf("%*s%d %s", SPACE, "", contentStats[index],
               contentMetrics[index]);
    }
    printf("\n");
}

void displaySizeInfo() {
    if (status.explicit) {
        printf("%s conversion basis:\n\n",
               status.decimal ? "Decimal" : "Binary");
    }

    hrSize convSize = convertBytes(totalBytes);
    printf("%.2lf%c", convSize.size, convSize.unit);
    printf("\n");
}

void displayStats(char *sourceDir) {
    printf("------------------------------\n");

    // if (status.verbose) {
    //     verboseIsTrue();
    // }

    printf("\n");
    printf("The total actual DISK usage of the source DIR\n");
    printf("%s\n", sourceDir);
    printf("\n");

    if (totalBytes <= 0) {
        printf("Nothing to print, empty directory\n");
        return;
    }

    displayContentInfo();
    printf("\n\n");
    displaySizeInfo();
}
