#include "hide_std_error_util.h"

#ifndef WIN32
#include <stdio.h>
#include <unistd.h>
#endif

static int _savedSTDERR;

void HideSTDERR() {
#ifndef WIN32
    _savedSTDERR = dup(STDERR_FILENO);
    freopen("/dev/null", "w", stderr);
#endif
}
void RestoreSTDERR() {
#ifndef WIN32
    dup2(_savedSTDERR, STDERR_FILENO);
#endif
}
