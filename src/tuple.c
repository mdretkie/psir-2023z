#include "tuple.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// TODO: endianness, error handling

Tuple tuple_new(uint32_t element_count, ...) {
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
		element.data.data_string = alloc_string(va_arg(args, char const*));
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
    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	if (tuple.elements[idx].type == tuple_string) {
	    free(tuple.elements[idx].data.data_string);
	}
    }

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

static bool tuple_element_match(TupleElement e1, TupleElement e2) {
    /* Niezgodne typy. */
    if ((e1.type & ~tuple_element_type_template_bit) != (e2.type & ~tuple_element_type_template_bit)) return false;

    /* Obydwa elementy to template. */
    if ((e1.type & tuple_element_type_template_bit) && (e2.type & tuple_element_type_template_bit)) return false;

    /* Jeden element to template, a drugi nie. */
    if ((e1.type & tuple_element_type_template_bit) != (e2.type & tuple_element_type_template_bit)) return true;

    /* Żaden z elementów to nie template. */
    switch (e1.type & ~tuple_element_type_template_bit) {
	case tuple_int: 
	    return e1.data.data_int == e2.data.data_int;

	case tuple_float:
	    return e1.data.data_float == e2.data.data_float;

	case tuple_string:
	    return !strcmp(e1.data.data_string, e2.data.data_string);
	
	default:
	    __builtin_unreachable();
    }
}


bool tuple_match(Tuple t1, Tuple t2) {
    if (t1.element_count != t2.element_count) return false;

    for (size_t idx = 0; idx < t1.element_count; ++idx) {
	if (!tuple_element_match(t1.elements[idx], t2.elements[idx])) return false;
    }

    return true;
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


static char* tuple_element_serialise(TupleElement element, char* buffer) {
    memcpy(buffer, &element.type, sizeof(element.type));
    buffer += sizeof(element.type);

    if (element.type & tuple_element_type_template_bit) {
	return buffer;
    } else {
	switch (element.type) {
	    case tuple_int:
		memcpy(buffer, &element.data.data_int, sizeof(element.data.data_int));
		return buffer + sizeof(element.data.data_int);

	    case tuple_float: 
		memcpy(buffer, &element.data.data_float, sizeof(element.data.data_float));
		return buffer + sizeof(element.data.data_float);

	    case tuple_string: {
		/* Zero na końcu też serializujemy. */
		uint32_t string_length = strlen(element.data.data_string) + 1;

		memcpy(buffer, &string_length, sizeof(string_length));
		buffer += sizeof(string_length);

		memcpy(buffer, element.data.data_string, string_length);
		return buffer + string_length;
	    }

	    default: 
		__builtin_unreachable();
	}
    }
}


static char* tuple_element_deserialise(char* buffer, TupleElement* element) {
    memcpy(&element->type, buffer, sizeof(element->type));
    buffer += sizeof(element->type);

    if (element->type & tuple_element_type_template_bit) {
	return buffer;
    } else {
	switch (element->type) {
	    case tuple_int:
		memcpy(&element->data.data_int, buffer, sizeof(element->data.data_int));
		return buffer + sizeof(element->data.data_int);

	    case tuple_float: 
		memcpy(&element->data.data_float, buffer, sizeof(element->data.data_float));
		return buffer + sizeof(element->data.data_float);

	    case tuple_string: {
		uint32_t string_length;

		memcpy(&string_length, buffer, sizeof(string_length));
		buffer += sizeof(string_length);

		element->data.data_string = malloc(string_length);
		memcpy(element->data.data_string, buffer, string_length);

		return buffer + string_length;
	    }

	    default: 
		__builtin_unreachable();
	}
    }
}


char* tuple_serialise(Tuple tuple) {
    size_t buffer_size = sizeof(tuple.element_count);

    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	buffer_size += sizeof(tuple.elements[idx].type);

	if (!(tuple.elements[idx].type & tuple_element_type_template_bit)) {
	    switch (tuple.elements[idx].type) {
		case tuple_int: 
		    buffer_size += sizeof(tuple.elements[idx].data.data_int);
		    break;

		case tuple_float: 
		    buffer_size += sizeof(tuple.elements[idx].data.data_float);
		    break;

		case tuple_string: 
		    buffer_size += sizeof(uint32_t) + strlen(tuple.elements[idx].data.data_string) + 1;
		    break;

		default: __builtin_unreachable();
	    }
	}
    }

    char* buffer = malloc(buffer_size);
    char* result_buffer = buffer;

    memcpy(buffer, &tuple.element_count, sizeof(tuple.element_count));
    buffer += sizeof(tuple.element_count);

    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	buffer = tuple_element_serialise(tuple.elements[idx], buffer);
    }

    return result_buffer;
}


Tuple tuple_deserialise(char* buffer) {
    Tuple tuple;

    memcpy(&tuple.element_count, buffer, sizeof(tuple.element_count));
    buffer += sizeof(tuple.element_count);

    tuple.elements = malloc(tuple.element_count * sizeof(TupleElement));

    for (size_t idx = 0; idx < tuple.element_count; ++idx) {
	buffer = tuple_element_deserialise(buffer, &tuple.elements[idx]);
    }

    return tuple;
}



void tuple_println(Tuple tuple) {
    tuple_print(tuple);
    printf("\n");
}
