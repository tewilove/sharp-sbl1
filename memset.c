
#include <stddef.h>

void memset(void *data, int value, size_t size) {
    size_t i = 0;
    char *p = (char *) data;
    while (i < size) {
        *p = value;
        i++;
    }
}

