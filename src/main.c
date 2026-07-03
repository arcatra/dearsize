#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME "dearsize"

struct cStatus {
  _Bool verbose;
  _Bool explicit;
};

struct cStatus status = {0, 0};

long long totalBytes = 0;
int totalDirs = -1, totalFiles, maxDepth = 0;

int findMax(int num1, int num2) { return num1 > num2 ? num1 : num2; }

int checkAndCalculateSize(const char *fpath, const struct stat *sb,
                          int typeflag, struct FTW *ftwbuf) {

  if (typeflag == FTW_D) {
    if (status.verbose) {
      printf("Directory: %s\n", fpath);
    }

    totalDirs += 1;
    maxDepth = findMax(maxDepth, ftwbuf->level);

    return 0;
  }

  if (status.verbose) {
    printf("Checking : %s\n", fpath);
  }

  if (typeflag == FTW_F) {
    totalBytes += sb->st_size;
    totalFiles += 1;

  } else if (typeflag == FTW_DNR) {
    fprintf(stderr, "Warning: The Directory %s is not readable", fpath);
    printf("-----------\n");
  }

  maxDepth = findMax(maxDepth, ftwbuf->level);
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

void displayMetadata(char *sourceDir) {
  double totalKIBs = totalBytes / 1024.0;
  double TotalMIBs = totalKIBs / 1024.0;

  printf("------------------------------\n");
  printf("\n");
  printf("Source directory: %s\n", sourceDir);
  printf("\n");

  int DirWidth = strlen("Total Dirs");
  int FileWidth = strlen("Total Files");
  int MaxDepthWidth = strlen("Max depth");

  int const SPACE = 8;

  printf("Content Information:\n\n");
  printf("%*s %*s %*s\n", FileWidth + SPACE, "Total Files", DirWidth + SPACE,
         "Total DIRs", MaxDepthWidth + SPACE, "Max Depth");

  printf("%*d %*d %*d\n", FileWidth + SPACE, totalFiles, DirWidth + SPACE,
         totalDirs, MaxDepthWidth + SPACE,
         maxDepth > 0 ? maxDepth - 1 : maxDepth);

  printf("\n\n");

  printf("Size Information:\n\n");

  int bytesWidth = strlen("Bytes");

  printf("%*s %*s %*s\n", bytesWidth + SPACE, "Bytes", SPACE * 2, "KIB",
         SPACE * 2, "MIB");
  printf("%*lld %*lf %*lf\n", bytesWidth + SPACE, totalBytes, SPACE * 2,
         totalKIBs, SPACE * 2, TotalMIBs);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Expeted a Directory name, Usage: %s <dir_name>\n", argv[0]);
    exit(0);
  }

  // Target: find the total size of the dir
  // 1. get the name of the target dir => argv at index 1
  // 2. iterate on the dir, sub dirs, and get the size of each file.
  // 3. Print the Size Information to stdout

  char *sourceDir = argv[1];

  if (argc >= 2) {
    // printArray(argv, argc);
    printf("\n");
    parseCommands(argv, argc);
  }

  if (nftw(sourceDir, checkAndCalculateSize, 20, 0) == -1) {
    printf("\n");
    perror("nftw");

    exit(EXIT_FAILURE);
  }

  displayMetadata(sourceDir);

  return EXIT_SUCCESS;
}
