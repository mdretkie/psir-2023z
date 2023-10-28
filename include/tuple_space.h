#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H

#include <stdarg.h>
#include <stddef.h>
#include <threads.h>

typedef enum TupleElementType {
    tuple_int = 1, 
    tuple_float = 2, 
    tuple_string = 3,
    tuple_int_template = tuple_int | 0x100,
    tuple_float_template = tuple_float | 0x100,
    tuple_string_template = tuple_string | 0x100
} TupleElementType;

typedef union TupleElementData {
    int data_int;
    float data_float;
    char const* data_string;
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
char const* tuple_get_string(Tuple tuple, size_t index);
void tuple_print(Tuple tuple);


/* TupleSpace zarządza zasobami wszystkich krotek w tablicy tuples,
 * więc nie zwalniamy samemu krotek (również template), 
 * które zostały wstawione do TupleSpace,
 * ale zwalniamy te, które wzięliśmy z TupleSpace. */
typedef struct TupleSpace {
    size_t tuple_count;
    Tuple* tuples;
    mtx_t tuples_mtx;
} TupleSpace;

typedef enum TupleSpaceOperationStatus {
    tuple_space_success, tuple_space_failure
} TupleSpaceOperationStatus;

TupleSpace* tuple_space_new();
void tuple_space_free(TupleSpace* tuple_space);

/* Operacje ze slajdów 18 - 22. */

/* Wstawiamy krotkę do przestrzeni.
 * Nie blokujemy. */
void tuple_space_out(TupleSpace* tuple_space, Tuple tuple);

/* Bierzemy i usuwamy krotkę z przestrzeni.
 * Jeśli kilka pasuje do wzorca, bierzemy dowolną.
 * Blokujemy. */
void tuple_space_in(TupleSpace* tuple_space, Tuple tuple_template);

/* Bierzemy i usuwamy krotkę z przestrzeni.
 * Jeśli kilka pasuje do wzorca, bierzemy dowolną.
 * Nie blokujemy. 
 * Zwracamy status operacji. */
void tuple_space_inp(TupleSpace* tuple_space, Tuple tuple_template);

/* Bierzemy i pozostawiamy krotkę w przestrzeni.
 * Jeśli kilka pasuje do wzorca, bierzemy dowolną.
 * Blokujemy. */
void tuple_space_rd(TupleSpace* tuple_space, Tuple tuple_template);

/* Bierzemy i pozostawiamy krotkę w przestrzeni.
 * Jeśli kilka pasuje do wzorca, bierzemy dowolną.
 * Nie blokujemy. 
 * Zwracamy status operacji. */
void tuple_space_rdp(TupleSpace* tuple_space, Tuple tuple_template);

#endif
