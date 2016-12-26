
#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);
void *memchr(const void *, int, size_t);
void *memmem(const void *, size_t, const void *, size_t);
int strlen(const char *);

#endif

