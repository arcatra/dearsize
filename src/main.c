#define _XOPEN_SOURCE 500
#define PROJECT_NAME "dearsize"

// Includes --------
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ---------------

struct cStatus {
    bool verbose;
    bool explicit;
    bool binary;
    bool decimal;
    bool symlinkstatus;
};

long long totalBytes = 0;
int const SPACE = 8;
int totalDirs = 0, totalFiles = 0, maxDepth = 0;

int sizes[] = {4, 4};

int contentStats[4];
double sizeStats[4];

struct cStatus status = {false, true, false, true, false};

int findMax(int num1, int num2) { return num1 > num2 ? num1 : num2; }

int checkAndCalculateSize(const char *fpath, const struct stat *sb,
                          int typeflag, struct FTW *ftwbuf) {

    maxDepth = findMax(maxDepth, ftwbuf->level);
    totalBytes += sb->st_size;

    if (typeflag == FTW_D) {
        if (status.verbose) {
            printf("Directory: %s\n", fpath);
        }

        totalDirs += 1;

        return 0;
    }

    if (status.verbose) {
        printf("Checking : %s\n", fpath);
    }

    if (typeflag == FTW_SL && status.symlinkstatus) {
        printf("Sym link, skipping: %s\n", fpath);
        totalFiles += 1;
        return 0;
    }

    if (typeflag == FTW_F) {
        totalFiles += 1;

    } else if (typeflag == FTW_DNR) {
        fprintf(stderr,
                "Warning: The Directory %s is not readable-----------\n",
                fpath);
        printf("\n");
        totalDirs += 1;
    }

    return 0;
}

void help() {
    printf("dearsize: help\n");
    FILE *fstream =
        fopen("/home/arcatra/Dutils/dearsize/resources/help.txt", "r");

    if (fstream == NULL) {
        printf("Cannot print the help, error occured\n");

        return;
    }

    char buf[256];
    while (fgets(buf, sizeof(buf), fstream) != NULL) {
        printf("%s", buf);
    }

    fclose(fstream);
}

void parseOptions(char *args[], int length) {
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

void verboseIsTrue() {
    printf("Decimal conversion basis: %d\n", status.decimal);
    printf("Binary conversion basis: %d\n", status.binary);
}

void convertBytes() {
    double conversionBasis = 1024.0;

    if (status.decimal) {
        conversionBasis = 1000.0;
    }

    sizeStats[0] = totalBytes;
    for (int index = 1; index < sizes[0]; index++) {
        sizeStats[index] = sizeStats[index - 1] / conversionBasis;
    }
}

void setContentData() {

    contentStats[0] = (totalFiles + totalDirs);
    contentStats[1] = totalDirs;
    contentStats[2] = totalFiles;
    contentStats[3] = maxDepth;
}

void displayContentInfo() {

    char *contentMetrics[] = {
        "Total Items",
        "Total DIRs",
        "Total Files",
        "Max Depth",
    };

    if (status.explicit) {
        printf("Directory Information:\n\n");
    }

    for (int index = 0; index < sizes[1]; index++) {
        int width = strlen(contentMetrics[index]) + SPACE;

        printf("%*s: %d", width, contentMetrics[index], contentStats[index]);
    }
    printf("\n");
}

void displaySizeInfo() {

    char *sizeMetrics[] = {
        "Bytes",
        "KB",
        "MB",
        "GB",
    };

    if (status.explicit) {
        printf("Size Information in %s conversion basis:\n\n",
               status.decimal ? "Decimal" : "Binary");
    }

    for (int index = 0; index < sizes[0]; index++) {
        int width = strlen(sizeMetrics[index]) + SPACE;

        printf("%*s: %.02lf", width, sizeMetrics[index], sizeStats[index]);
    }
    printf("\n");
}

void displayMetadata(char *sourceDir) {
    printf("------------------------------\n");
    printf("\n");
    printf("Source directory: %s\n", sourceDir);
    printf("\n");

    if (totalBytes <= 0) {
        printf("Nothing to print, empty directory\n");
        return;
    }

    setContentData();
    displayContentInfo();
    printf("\n\n");
    convertBytes();
    displaySizeInfo();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Expeted a Directory name, Usage: %s <dir_name>\n", argv[0]);
        exit(0);
    }

    // Goal: find the exact total size of the dir
    // 1. get the name of the target dir => argv at index 1
    // 2. iterate on the dir, sub dirs, and get the size of each file.
    // 3. Print the Size Information to stdout

    char *sourceDir = argv[1];

    if (argc >= 2) {
        printf("\n");
        parseOptions(argv, argc);
    }

    if (status.verbose) {
        verboseIsTrue();
    }

    if (!strcmp(sourceDir, "None")) {
        printf("No source directory provided\n");
        return EXIT_SUCCESS;
    }

    if (nftw(sourceDir, checkAndCalculateSize, 20, FTW_PHYS | FTW_MOUNT) ==
        -1) {
        printf("\n");
        perror("dearsize");

        exit(EXIT_FAILURE);
    }

    displayMetadata(sourceDir);

    return EXIT_SUCCESS;
}
