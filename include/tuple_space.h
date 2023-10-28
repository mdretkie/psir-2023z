#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H

#include <stdarg.h>
#include <stddef.h>

typedef enum TupleElementType {
    TupleInt, TupleFloat
} TupleElementType;

typedef union TupleElementData {
    int data_int;
    float data_float;
} TupleElementData;

typedef struct TupleElement {
    TupleElementType type;
    TupleElementData data;
} TupleElement;

typedef struct Tuple {
    size_t element_count;
    TupleElement* elements;
} Tuple;

Tuple tuple_new(size_t element_count, ...);
void tuple_free(Tuple tuple);
int tuple_get_int(Tuple tuple, size_t index);
float tuple_get_float(Tuple tuple, size_t index);
void tuple_print(Tuple tuple);


#endif
