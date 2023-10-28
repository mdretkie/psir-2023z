#include "tuple_space.h"
#include <stdlib.h>

TupleSpace* tuple_space_new() {
    TupleSpace* tuple_space = malloc(sizeof(TupleSpace));
    tuple_space->tuple_count = 0;
    tuple_space->tuples = NULL;
    mtx_init(&tuple_space->tuples_mtx, mtx_plain);
}

void tuple_space_free(TupleSpace* tuple_space) {
    mtx_destroy(&tuple_space->tuples_mtx);
    free(tuple_space->tuples);
}

void tuple_space_insert(TupleSpace* tuple_space, Tuple tuple) {
    mtx_lock(&tuple_space->tuples_mtx);
    tuple_space->tuple_count += 1;
    tuple_space->tuples = realloc(tuple_space->tuples, tuple_space->tuple_count * sizeof(Tuple));
    tuple_space->tuples[tuple_space->tuple_count - 1] = tuple;
    mtx_unlock(&tuple_space->tuples_mtx);
}

TupleSpaceOperationResult tuple_space_remove(TupleSpace* tuple_space, Tuple tuple_template, TupleSpaceOperationBlockingMode blocking_mode) {

}

TupleSpaceOperationResult tuple_space_read(TupleSpace* tuple_space, Tuple tuple_template, TupleSpaceOperationBlockingMode blocking_mode) {

}
