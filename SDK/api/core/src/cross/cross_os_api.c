#include <cross/cross_os_api.h>

#ifdef _MSC_VER
	#define PATH_BACKSLASH	'\\'
#else
	#define PATH_BACKSLASH	'/'
#endif

void cross_os_append_path(char* path, const char* append)
{
    int32_t len = (int32_t)strlen(path);
    if(path[len - 1] != PATH_BACKSLASH)path[len++] = PATH_BACKSLASH;
    strcpy(&path[len],append);
}

