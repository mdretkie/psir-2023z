#ifndef TUPLE_H
#define TUPLE_H

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define tuple_element_type_template_bit 0x100

typedef enum TupleElementType {
    tuple_int = 1, 
    tuple_float = 2, 
    tuple_string = 3,
    tuple_int_template = tuple_int | tuple_element_type_template_bit,
    tuple_float_template = tuple_float | tuple_element_type_template_bit,
    tuple_string_template = tuple_string | tuple_element_type_template_bit
} TupleElementType;

typedef union TupleElementData {
    int data_int;
    float data_float;
    char* data_string;
} TupleElementData;

typedef struct TupleElement {
    TupleElementType type;
    TupleElementData data;
} TupleElement;

typedef struct Tuple {
    uint32_t element_count;
    TupleElement* elements;
} Tuple;

/* Stringi są brane na własność, są głęboko kopiowane. */
Tuple tuple_new(uint32_t element_count, ...);
void tuple_free(Tuple tuple);

int tuple_get_int(Tuple tuple, size_t index);
float tuple_get_float(Tuple tuple, size_t index);
char const* tuple_get_string(Tuple tuple, size_t index);

bool tuple_match(Tuple t1, Tuple t2);

char* tuple_serialise(Tuple tuple, char* buffer);
char const* tuple_deserialise(char const* buffer, Tuple* tuple);
size_t tuple_serialised_length(Tuple tuple);

void tuple_print(Tuple tuple);
void tuple_println(Tuple tuple);

#endif
