#pragma once

#include <kernel.hpp>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *d, const char *s);
char *strncpy(char *d, const char *s, size_t n);

char *strchr(const char *s, const char c);
size_t strspn(const char *s1, const char *s2);
size_t strcspn(const char *s1, const char *s2);
const char *strstr(const char *s1, const char *s2);
char *strtok(char *s, const char *delim);