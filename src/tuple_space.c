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

	switch (element_type) {
	    case TupleInt: {
		TupleElement element = {
		    .type = TupleInt,
		    .data = {
			.data_int = va_arg(args, int)
		    }
		};

		tuple.elements[idx] = element;
		break;
	    }
	    case TupleFloat: {
		TupleElement element = {
		    .type = TupleFloat,
		    .data = {
			// https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
			.data_float = (float)va_arg(args, double)
		    }
		};

		tuple.elements[idx] = element;
		break;
	    }
	}
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

void tuple_print(Tuple tuple) {
    printf("(");

    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	switch (tuple.elements[idx].type) {
	    case TupleInt:
		printf("%d", tuple_get_int(tuple, idx));
		break;
	    case TupleFloat:
		printf("%g", tuple_get_float(tuple, idx));
		break;
	}
	
	if (idx < tuple.element_count - 1) printf(", ");
    }

    printf(")");
}

