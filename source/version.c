#include "version.h"

static char programName[10] =  "kingLouis";
static char version[2] = "I";
static char author[11] = "vasasbrent";

const char* getProgramName() {
    return programName;
}

const char* getVersionNumber() {
    return version;
}

const char* getAuthor() {
    return author;
}
