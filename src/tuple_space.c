#include "tuple_space.h"
#include <stdlib.h>
#include <stdio.h>

Tuple tuple_new(size_t element_count, ...) {
    va_list args;
    va_start(args, element_count);

    Tuple tuple = {
	.element_count = element_count,
	.elements = malloc(element_count * sizeof(TupleElement))
    };

    for (size_t idx = 0; idx < element_count; ++idx) {
	TupleElementType element_type = va_arg(args, TupleElementType);

	TupleElement element = {
	    .type = element_type,
	};

	switch (element_type) {
	    case tuple_int:
		element.data.data_int = va_arg(args, int);
		break;

	    case tuple_float: 
		// https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
		element.data.data_float = (float)va_arg(args, double);
		break;

	    case tuple_string:
		element.data.data_string = va_arg(args, char const*);
		break;

	    case tuple_int_template:
	    case tuple_float_template:
	    case tuple_string_template:
		break;
	}

	tuple.elements[idx] = element;
    }

    va_end(args);

    return tuple;
}

void tuple_free(Tuple tuple) {
    free(tuple.elements);
}

int tuple_get_int(Tuple tuple, size_t index) {
    return tuple.elements[index].data.data_int;
}

float tuple_get_float(Tuple tuple, size_t index) {
    return tuple.elements[index].data.data_float;
}

char const* tuple_get_string(Tuple tuple, size_t index) {
    return tuple.elements[index].data.data_string;
}

void tuple_print(Tuple tuple) {
    printf("(");

    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	switch (tuple.elements[idx].type) {
	    case tuple_int:
		printf("%d", tuple_get_int(tuple, idx));
		break;

	    case tuple_float:
		printf("%g", tuple_get_float(tuple, idx));
		break;

	    case tuple_string:
		printf("\"%s\"", tuple_get_string(tuple, idx));
		break;

	    case tuple_int_template:
		printf("int?");
		break;

	    case tuple_float_template:
		printf("float?");
		break;

	    case tuple_string_template:
		printf("string?");
		break;
	}
	
	if (idx < tuple.element_count - 1) printf(", ");
    }

    printf(")");
}

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

void tuple_space_out(TupleSpace* tuple_space, Tuple tuple) { 
    mtx_lock(&tuple_space->tuples_mtx);
    tuple_space->tuple_count += 1;
    tuple_space->tuples = realloc(tuple_space->tuples, tuple_space->tuple_count * sizeof(Tuple));
    tuple_space->tuples[tuple_space->tuple_count - 1] = tuple;
    mtx_unlock(&tuple_space->tuples_mtx);
}

void tuple_space_in(TupleSpace* tuple_space, Tuple tuple_template) {
    mtx_lock(&tuple_space->tuples_mtx);

    mtx_unlock(&tuple_space->tuples_mtx);
}

void tuple_space_inp(TupleSpace* tuple_space, Tuple tuple_template) {
    mtx_lock(&tuple_space->tuples_mtx);

    mtx_unlock(&tuple_space->tuples_mtx);
}

void tuple_space_rd(TupleSpace* tuple_space, Tuple tuple_template) {
    mtx_lock(&tuple_space->tuples_mtx);

    mtx_unlock(&tuple_space->tuples_mtx);
}

void tuple_space_rdp(TupleSpace* tuple_space, Tuple tuple_template) {
    mtx_lock(&tuple_space->tuples_mtx);

    mtx_unlock(&tuple_space->tuples_mtx);
}
