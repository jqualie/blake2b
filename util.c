// LCAP C library
// Copyright © 2019 Jim Qualie jqualie@levven.com
// Levven Electronics Ltd. All rights reserved.

#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2005+ */

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__unix__) || defined(__APPLE__)
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

// parse ASCII into nibble
static uint8_t unhex_chr(char a)
{
    if (a >= '0' && a <= '9')
    {
        return a - '0';
    }
    else if (a >= 'a' && a <= 'f')
    {
        return a - 'a' + 10;
    }
    else if (a >= 'A' && a <= 'F')
    {
        return a - 'A' + 10;
    }
    return 0;
}

uint16_t ParseU16(uint8_t *p)
{
    return (unhex_chr(p[0])) << 12 | (unhex_chr(p[1]) << 8) | (unhex_chr(p[2]) << 4) | (unhex_chr(p[3]) << 0);
}

static const uint8_t hd[] = "0123456789abcdef";

void U16ToString(uint8_t *p, uint16_t a)
{
    p[0] = hd[(a >> 12) & 0xF];
    p[1] = hd[(a >> 8) & 0xF];
    p[2] = hd[(a >> 4) & 0xF];
    p[3] = hd[(a >> 0) & 0xF];
}

void U32ToString(uint8_t *p, uint32_t a)
{
    p[0] = hd[(a >> 28) & 0xF];
    p[1] = hd[(a >> 24) & 0xF];
    p[2] = hd[(a >> 20) & 0xF];
    p[3] = hd[(a >> 16) & 0xF];
    p[4] = hd[(a >> 12) & 0xF];
    p[5] = hd[(a >> 8) & 0xF];
    p[6] = hd[(a >> 4) & 0xF];
    p[7] = hd[(a >> 0) & 0xF];
}

size_t unhex(uint8_t *buf, size_t len, const char *str)
{
    size_t used = 0;

    while (*str && len)
    {
        *buf = unhex_chr(str[0]) << 4 | unhex_chr(str[1]);
        buf++;
        used++;
        str += 2;
        len--;
    }

    return used;
}

char *tohex(uint8_t *buf, size_t len)
{
    char *str = malloc(len*2 + 1);
    char *p = str;
    while(len--)
    {
        p[0] = hd[*buf >> 4];
        p[1] = hd[*buf & 0xF];
        buf++;
        p += 2;
    }
    p[0] = '\0'; // terminating null
    return str;
}

void dump(const char *tag, const uint8_t *a, const size_t len)
{
    //printf("%s (%3d) : ", tag, (int)len);
    printf("%s ", tag);
    for (size_t i = 0; i < len; i++)
    {
        printf("%02x", a[i]);
        //if ((i > 0) && (i % 48 == 0))
        //{
        //    printf("\n");
        //}
        //else if ((i > 0) && (i % 16 == 0))
        //{
        //    printf(" ");
        //}
    }
    printf("\n");
}
