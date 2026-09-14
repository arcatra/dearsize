#ifndef __MAIN_H__
#define __MAIN_H__
#include <sys/types.h>

typedef struct {
    double size;
    char unit;
} hrSize;

int findMax(int num1, int num2);
// double findMax(double num1, double num2);

int recordFilesAndData(const char *fpath, const struct stat *fileStatus,
                       int typeflag, struct FTW *ftwbuf);
void help();

void parseFlags(char *args[], int lenght);

hrSize convertBytes(off_t bytes);
void setConversion();

void displayContentStats();
void displaySizeStats();

void displayStats(char *sourceDir);

#endif // !__MAIN_H__
