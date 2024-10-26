#include <lib/string.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dest, const void *src, size_t n)
{
    u8 *pdest = static_cast<u8 *>(dest);
    const u8 *psrc = static_cast<const u8 *>(src);
    
    for (size_t i = 0; i < n; i++)
        pdest[i] = psrc[i];

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    u8 *p = static_cast<u8 *>(s);
    for (size_t i = 0; i < n; i++)
        p[i] = static_cast<uint8_t>(c);

    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    u8 *pdest = static_cast<u8 *>(dest);
    const u8 *psrc = static_cast<const u8 *>(src);

    if (src > dest)
    {
        for (size_t i = 0; i < n; i++)
            pdest[i] = psrc[i];
    }
    else if (src < dest)
    {
        for (size_t i = n; i > 0; i--)
            pdest[i - 1] = psrc[i - 1];
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *p1 = static_cast<const u8 *>(s1);
    const u8 *p2 = static_cast<const u8 *>(s2);

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return p1[i] < p2[i] ? -1 : 1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

size_t strlen(const char *s)
{
    const char *str;
    for (str = s; *str; str++) ;
    
    return str - s;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for ( ; n--; s1++, s2++)
    {
        if (*s1 != *s2)
            return *s1 - *s2;
    }
    
    return 0;
}

char *strcpy(char *d, const char *s)
{
    if (!d)
        return nullptr;
    
    char *ptr = d;
    while (*s)
        *d++ = *s++;
    
    *d = '\0';
    return ptr;
}

char *strncpy(char *d, const char *s, size_t n)
{
    if (!d)
        return nullptr;
    
    char *ptr = d;
    while (*s && n--)
        *d++ = *s++;
    
    *d = '\0';
    return ptr;
}

char *strchr(const char *s, const char c)
{
    while (*s != c && *s != '\0')
        s++;
    if (*s == c) 
        return (char *)s;
    
    return nullptr;
}

size_t strspn(const char *s1, const char *s2)
{
    size_t len = 0;
    if (!s1 || !s2)
        return len;
    
    while (*s1 && strchr(s2, *s1++))
        len++;
    
    return len;
}

size_t strcspn(const char *s1, const char *s2)
{
    size_t len = 0;
    if (!s1 || !s2)
        return len;
    
    while (*s1)
    {
        if (strchr(s2, *s1))
            return len;
        
        s1++;
        len++;
    }

    return len;
}

const char *strstr(const char *s1, const char *s2)
{
    size_t n = strlen(s2);
    while (*s1)
    {
        if (!memcmp(s1, s2, n))
            return s1;
 
        s1++;
    }

    return nullptr;
}

char *strtok(char *s, const char *delim)
{
    static char *p = nullptr;
    if (!s && ((s = p) == nullptr))
        return nullptr;
    
    s += strspn(s, delim);
    if (!*s)
        return p = nullptr;
    
    p = s + strcspn(s, delim);
    if (*p)
        *p++ = '\0';
    else 
        p = nullptr;
    
    return s;
}