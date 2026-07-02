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

struct cStatus status;

long long totalSize = 0;

int calculateSize(const char *fpath, const struct stat *sb, int typeflag,
                  struct FTW *ftwbuf) {

  if (status.verbose) {
    printf("Checking: %s, Living at a depth: %d\n", fpath, ftwbuf->level);
    printf("Size: %ld bytes\n", sb->st_size);
    printf("\n");
  }

  if (typeflag == FTW_F) {
    totalSize += sb->st_size;

  } else if (typeflag == FTW_DNR) {
    fprintf(stderr, "Warning: The Directory %s is not readable", fpath);
    printf("-----------\n");
  }

  return 0;
}

void parseCommands(char *args[], int length) {
  for (int index = 2; index < length; index++) {
    if (strcmp(args[index], "-v") || strcmp(args[index], "-V")) {
      status.verbose = 1;
    }

    if (strcmp(args[index], "-ex") || strcmp(args[index], "-Ex")) {
      status.explicit = 1;

    } else {
      printf("%s command not found\n", args[index]);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Expeted a Directory name, Usage: %s <dir_name>\n", argv[0]);
    exit(0);
  }

  // find the total size of the dir
  // 1. get the name of the target dir => argv@1
  // 2. iterate on the dir, sub dirs, and get the size of each file.
  // 3. Print the dir size to stdout

  char *sourceDir = argv[1];

  if (argc >= 2) {
    parseCommands(argv, argc);
  }

  if (nftw(sourceDir, calculateSize, 20, 0) == -1) {
    printf("\n");
    perror("nftw");

    exit(EXIT_FAILURE);
  }

  double KB = totalSize / 1024.0;
  double MB = KB / 1024.0;

  printf("------Result-------\n");
  printf("Source directory: %s\n", sourceDir);
  printf("Total Size Information = ");
  printf("Bytes: %lld, \t KBs: %lf, \t MBs: %lf\n", totalSize, KB, MB);

  return EXIT_SUCCESS;
}
