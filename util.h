// LCAP C library
// Copyright © 2019 Jim Qualie jqualie@levven.com
// Levven Electronics Ltd. All rights reserved.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>

size_t unhex(uint8_t *buf, size_t len, const char *str);
char *tohex(uint8_t *buf, size_t len);
void dump(const char *tag, const uint8_t *a, const size_t len);

uint16_t ParseU16(uint8_t *p);
void U16ToString(uint8_t *p, uint16_t a);
void U32ToString(uint8_t *p, uint32_t a);
