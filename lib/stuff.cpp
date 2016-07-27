#include "stuff.h"

// 0 - mac, 1 - linux, 2 - windows, 3 - other
int probeOS() {
#ifdef TARGET_OS_MAC
    return 0;
#elif defined __linux__
    return 1;
#elif defined _WIN32 || defined _WIN64
    return 2;
#else
    return 3;
#endif
}
