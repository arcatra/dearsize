#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME "dearsize"

struct cStatus {
  _Bool verbose;
  _Bool explicit;
};

struct cStatus status;

long long totalSize = 0;
int totalDirs = -1, totalFiles, maxDepth = 0;

int calculateSize(const char *fpath, const struct stat *sb, int typeflag,
                  struct FTW *ftwbuf) {

  if (typeflag == FTW_D) {
    // printf("Found DIR, DIR Path: %s\n", fpath);
    totalDirs += 1;
    maxDepth = fmax(maxDepth, ftwbuf->level);
    return 0;
  }

  if (status.verbose) {
    printf("Checking: %s\n", fpath);
  }

  if (typeflag == FTW_F) {
    totalSize += sb->st_size;
    totalFiles += 1;

  } else if (typeflag == FTW_DNR) {
    fprintf(stderr, "Warning: The Directory %s is not readable", fpath);
    printf("-----------\n");
  }

  maxDepth = fmax(maxDepth, ftwbuf->level);
  return 0;
}

void printArray(char *args[], int length) {
  for (int index = 0; index < length; index++) {
    printf("index = %d, elm = %s\n", index, args[index]);
  }
}

void parseCommands(char *args[], int length) {
  for (int index = 2; index < length; index++) {
    if (strcmp(args[index], "-v") == 0 || strcmp(args[index], "-V") == 0) {
      status.verbose = 1;
    }

    if (strcmp(args[index], "-ex") == 0 || strcmp(args[index], "-Ex") == 0) {
      status.explicit = 1;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Expeted a Directory name, Usage: %s <dir_name>\n", argv[0]);
    exit(0);
  }

  // find the total size of the dir
  // 1. get the name of the target dir => argv at index 1
  // 2. iterate on the dir, sub dirs, and get the size of each file.
  // 3. Print the Size Information to stdout

  char *sourceDir = argv[1];

  if (argc >= 2) {
    // printArray(argv, argc);
    printf("\n");
    parseCommands(argv, argc);
  }

  if (nftw(sourceDir, calculateSize, 20, 0) == -1) {
    printf("\n");
    perror("nftw");

    exit(EXIT_FAILURE);
  }

  double KB = totalSize / 1024.0;
  double MB = KB / 1024.0;

  printf("------------------------------\n");
  printf("\n");
  printf("Source directory: %s\n", sourceDir);
  printf("\n");

  int sDirWidth = strlen("Total Dirs");
  int sFileWidth = strlen("Total Files");
  int sMaxDepthWidth = strlen("Max depth");

  int const SPACE = 8;

  printf("Content Information\n\n");
  printf("%s %*s %*s\n", "Total Files", sDirWidth + SPACE, "Total DIRs",
         sMaxDepthWidth + SPACE, "Max Depth");

  printf("%*d %*d %*d\n", sFileWidth, totalFiles, sDirWidth + SPACE, totalDirs,
         sMaxDepthWidth + SPACE, maxDepth - 1);

  printf("\n\n");

  printf("Size Information\n\n");
  int bytesWidth = strlen("Bytes");
  printf("%s %*s %*s\n", "Bytes", SPACE * 2, "KB", SPACE * 2, "MB");
  printf("%*lld %*lf %*lf\n", bytesWidth, totalSize, SPACE * 2, KB, SPACE * 2,
         MB);

  return EXIT_SUCCESS;
}
