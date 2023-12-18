#pragma once

#define new(x) malloc(sizeof((x)))
#define delete(x) free((x))
#define new_clean(n, x) calloc((n), (sizeof(x)))
