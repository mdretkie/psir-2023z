#include "tuple_space.h"
#include <stdio.h>
#include <stdlib.h>

TupleSpace tuple_space_new() {
    TupleSpace tuple_space;
    tuple_space.tuple_count = 0;
    tuple_space.tuples = NULL;
    mtx_init(&tuple_space.tuples_mtx, mtx_plain);
    cnd_init(&tuple_space.tuples_cnd);
    return tuple_space;
}

void tuple_space_free(TupleSpace tuple_space) {
    cnd_destroy(&tuple_space.tuples_cnd);
    mtx_destroy(&tuple_space.tuples_mtx);
    free(tuple_space.tuples);
}

void tuple_space_insert(TupleSpace* tuple_space, Tuple tuple) {
    mtx_lock(&tuple_space->tuples_mtx);
    tuple_space->tuple_count += 1;
    tuple_space->tuples = realloc(tuple_space->tuples, tuple_space->tuple_count * sizeof(Tuple));
    tuple_space->tuples[tuple_space->tuple_count - 1] = tuple;
    mtx_unlock(&tuple_space->tuples_mtx);
    cnd_broadcast(&tuple_space->tuples_cnd);
}

static Tuple tuple_space_get_helper(TupleSpace* tuple_space, size_t index, TupleSpaceOperationRemovePolicy remove_policy) {
    switch (remove_policy) {
	case tuple_space_remove: {
	    Tuple result = tuple_space->tuples[index];

	    if (tuple_space->tuple_count > 1) {
		tuple_space->tuples[index] = tuple_space->tuples[tuple_space->tuple_count - 1];
	    }

	    tuple_space->tuple_count -= 1;
	    tuple_space->tuples = realloc(tuple_space->tuples, tuple_space->tuple_count * sizeof(Tuple));

	    return result;
	}

	case tuple_space_keep: {
	    return tuple_space->tuples[index];
	}

	default:
	    __builtin_unreachable();
    }
}

TupleSpaceOperationResult tuple_space_get(TupleSpace* tuple_space, Tuple tuple_template, TupleSpaceOperationBlockingMode blocking_mode, TupleSpaceOperationRemovePolicy remove_policy) {
    TupleSpaceOperationResult result = {
	.status = tuple_space_failure,
        .tuple = tuple_new(0),
    };

    switch (blocking_mode) {
	case tuple_space_blocking: {
	    mtx_lock(&tuple_space->tuples_mtx);

	    for (;;) {
		for (size_t idx = 0; idx < tuple_space->tuple_count; ++idx) {
		    if (tuple_match(tuple_template, tuple_space->tuples[idx])) {
			result.status = tuple_space_success;
			result.tuple = tuple_space_get_helper(tuple_space, idx, remove_policy);
			break;
		    }
		}

		if (result.status == tuple_space_failure) {
		    cnd_wait(&tuple_space->tuples_cnd, &tuple_space->tuples_mtx);
		} else {
		    mtx_unlock(&tuple_space->tuples_mtx);
		    break;
		}
	    }

	    break;
	}

	case tuple_space_nonblocking: {
	    mtx_lock(&tuple_space->tuples_mtx);

	    for (size_t idx = 0; idx < tuple_space->tuple_count; ++idx) {
		if (tuple_match(tuple_template, tuple_space->tuples[idx])) {
		    result.status = tuple_space_success;
		    result.tuple = tuple_space_get_helper(tuple_space, idx, remove_policy);
		    break;
		}
	    }

	    mtx_unlock(&tuple_space->tuples_mtx);
	    break;
	}
    }

    return result;
}

