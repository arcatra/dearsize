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

typedef struct {
    double kb;
    double mb;
    double gb;
} sizeStats;

long long totalBytes = 0;
int const SPACE = 8;
int totalDirs = -1, totalFiles, maxDepth = 0;
struct cStatus status = {false, true, true, false, false};

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
        fprintf(stderr, "Warning: The Directory %s is not readable\n", fpath);
        printf("-----------\n");
        totalDirs += 1;
    }

    return 0;
}

void help() { printf("dearsize: help\n"); }

void parseCommands(char *args[], int length) {
    int opt;

    while ((opt = getopt(length, args, "XvVdDsShH")) != -1) {
        switch (opt) {
        case 'X':
            status.explicit = false;
            break;

        case 'v':
        case 'V':
            status.verbose = true;
            break;

        case 'd':
        case 'D':
            status.decimal = true;
            status.binary = false;
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

sizeStats convertBytes() {
    sizeStats sStats;

    double conversionBasis = 1024.0;

    if (status.decimal) {
        conversionBasis = 1000.0;
    }

    sStats.kb = totalBytes / conversionBasis;
    sStats.mb = sStats.kb / conversionBasis;
    sStats.gb = sStats.mb / conversionBasis;

    return sStats;
}

void displayContentInfo() {
    int DirWidth = strlen("Total Dirs");
    int totalItemsWidth = strlen("Total items");
    int FileWidth = strlen("Total Files");
    int MaxDepthWidth = strlen("Max depth");

    if (status.explicit) {
        printf("Content Information:\n\n");
        printf("%s %*s %*s %*s\n", "Total items", totalItemsWidth + SPACE,
               "Total Files", DirWidth + SPACE, "Total DIRs",
               MaxDepthWidth + SPACE, "Max Depth");
    }

    printf("%*d %*d %*d %*d\n", totalItemsWidth, (totalDirs + totalFiles),
           FileWidth + SPACE, totalFiles, DirWidth + SPACE,
           totalDirs > 0 ? totalDirs - 1 : 0, MaxDepthWidth + SPACE,
           maxDepth > 0 ? maxDepth - 1 : 0);
}

void displaySizeInfo(sizeStats stats) {

    int bytesWidth = strlen("Bytes");
    if (status.explicit) {
        printf("Size Information in %s conversion basis:\n\n",
               status.decimal ? "Decimal" : "Binary");

        printf("%*s %*s %*s %*s\n", bytesWidth + SPACE, "Bytes", SPACE * 2,
               "KIB", SPACE * 2, "MIB", SPACE * 2, "GB");
    }

    printf("%*lld %*.02lf %*.02lf %*.02lf\n", bytesWidth + SPACE, totalBytes,
           SPACE * 2, stats.kb, SPACE * 2, stats.mb, SPACE * 2, stats.gb);
}

void displayMetadata(char *sourceDir) {
    printf("------------------------------\n");
    printf("\n");
    printf("Source directory: %s\n", sourceDir);
    printf("\n");

    if (totalBytes <= 0 || !(totalFiles)) {
        printf("Nothing to print, empty directory\n");
        return;
    }

    displayContentInfo();
    printf("\n\n");
    sizeStats convStats = convertBytes();
    displaySizeInfo(convStats);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Expeted a Directory name, Usage: %s <dir_name>\n", argv[0]);
        exit(0);
    }

    // What we need to achive: find the exact total size of the dir
    // 1. get the name of the target dir => argv at index 1
    // 2. iterate on the dir, sub dirs, and get the size of each file.
    // 3. Print the Size Information to stdout

    char *sourceDir = argv[1];

    if (argc >= 2) {
        // printArray(argv, argc);
        printf("\n");
        parseCommands(argv, argc);
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
