#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vapor/FileUtils.h>
#include <vapor/VAssert.h>

void Wasp::_VAssertFailed(const char *expr, const char *path, const unsigned int line) {
    char *fileName = strdup(FileUtils::Basename(std::string(path)).c_str());

    fprintf(stderr, "VAssert(%s) failed in %s, line %i\n", expr, fileName, line);

    free(fileName);

#ifdef NDEBUG
    exit(1);
#else
    raise(SIGTERM);
#endif
}
