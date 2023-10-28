#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H

#include "tuple.h"
#include <threads.h>

/* TupleSpace zarządza zasobami wszystkich krotek w tablicy tuples,
 * więc nie zwalniamy samemu krotek (również template), 
 * które zostały wstawione do TupleSpace,
 * ale zwalniamy te, które wzięliśmy z TupleSpace. */
typedef struct TupleSpace {
    size_t tuple_count;
    Tuple* tuples;
    mtx_t tuples_mtx;
} TupleSpace;

typedef enum TupleSpaceOperationBlockingMode {
    tuple_space_blocking, tuple_space_nonblocking
} TupleSpaceOperationBlockingMode;

typedef enum TupleSpaceOperationResultStatus {
    tuple_space_success, tuple_space_failure
} TupleSpaceOperationResultStatus;

typedef struct TupleSpaceOperationResult {
    TupleSpaceOperationResultStatus status;
    Tuple tuple;
} TupleSpaceOperationResult;


TupleSpace* tuple_space_new();
void tuple_space_free(TupleSpace* tuple_space);

void tuple_space_insert(TupleSpace* tuple_space, Tuple tuple);
TupleSpaceOperationResult tuple_space_remove(TupleSpace* tuple_space, Tuple tuple_template, TupleSpaceOperationBlockingMode blocking_mode);
TupleSpaceOperationResult tuple_space_read(TupleSpace* tuple_space, Tuple tuple_template, TupleSpaceOperationBlockingMode blocking_mode);

#endif
