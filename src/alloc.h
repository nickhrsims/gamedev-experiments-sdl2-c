#pragma once
#include <stdlib.h>

#define new(x) malloc(sizeof(x))
#define new_array(n, x) malloc(n * sizeof(x))
#define delete(x) free(x)
#define new_clean(n, x) calloc(n, sizeof(x))
