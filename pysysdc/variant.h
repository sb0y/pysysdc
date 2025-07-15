#pragma once

//#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

typedef enum variant_state_t 
{
	variant_state_is_int,
	variant_state_is_str,
	variant_state_is_double,
	variant_state_is_float,
	variant_state_is_empty
} variant_state_t;

union variant_union_t
{
	int64_t v_int;
	char *v_str;
	float_t v_float;
	double_t v_double;
};

typedef struct variant_t
{
	union variant_union_t variant;
	variant_state_t state;
} variant_t;

typedef struct variant_array_t
{
	variant_t *variants;
	size_t size;
} variant_array_t;

void variant_array_free(variant_array_t va_arr);
void allocate_variant_array(variant_array_t **va_arr, size_t size);
void variant_set_str(variant_t *variant, const char *val);
void variant_set_int(variant_t *variant, int64_t val);

inline variant_array_t init_variant_array(void)
{
	variant_array_t array = { .variants = NULL, .size = 0 };
	return array;
}

inline int variant_array_is_allocated(variant_array_t va_arr)
{
	return va_arr.variants == NULL ? 0 : 1;
}

inline size_t variant_array_get_size(variant_array_t va_arr)
{
	return va_arr.size;
}

inline int variant_array_is_empty(variant_array_t va_arr)
{
	if(!variant_array_is_allocated(va_arr))
	{
		return 1;
	}

	for(size_t i = 0; va_arr.size > i; ++i)
	{
		if(va_arr.variants[i].state != variant_state_is_empty)
		{
			return 0;
		}
	}

	return 1;
}

inline size_t variant_array_count_elements(variant_array_t va_arr)
{
	return va_arr.size + 1;
}

inline variant_state_t variant_get_state(variant_t variant)
{
	return variant.state;
}

inline int64_t variant_get_int(variant_t variant)
{
	assert(variant_get_state(variant) != variant_state_is_int);
	return variant.variant.v_int;
}

inline int variant_is_int(variant_t variant)
{
	return variant_get_state(variant) == variant_state_is_int ? 1 : 0;
}

inline int variant_is_str(variant_t variant)
{
	return variant_get_state(variant) == variant_state_is_str ? 1 : 0;
}

inline const char* variant_get_str(variant_t variant)
{
	assert(!variant_is_str(variant));
	return variant.variant.v_str;
}