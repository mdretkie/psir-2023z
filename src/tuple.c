#include "tuple.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

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

int tuple_get_int(Tuple const* tuple, size_t index) {
    return tuple->elements[index].data.data_int;
}

float tuple_get_float(Tuple const* tuple, size_t index) {
    return tuple->elements[index].data.data_float;
}

char const* tuple_get_string(Tuple const* tuple, size_t index) {
    return tuple->elements[index].data.data_string;
}

static bool tuple_element_match(TupleElement const* e1, TupleElement const* e2) {
    /* Niezgodne typy. */
    if ((e1->type & ~tuple_element_type_template_bit) != (e2->type & ~tuple_element_type_template_bit)) return false;

    /* Obydwa elementy to template. */
    if ((e1->type & tuple_element_type_template_bit) && (e2->type & tuple_element_type_template_bit)) return false;

    /* Jeden element to template, a drugi nie. */
    if ((e1->type & tuple_element_type_template_bit) != (e2->type & tuple_element_type_template_bit)) return true;

    /* Żaden z elementów to nie template. */
    switch (e1->type & ~tuple_element_type_template_bit) {
	case tuple_int: 
	    return e1->data.data_int == e2->data.data_int;

	case tuple_float:
	    return e1->data.data_float == e2->data.data_float;

	case tuple_string:
	    return !strcmp(e1->data.data_string, e2->data.data_string);
	
	default:
	    __builtin_unreachable();
    }
}


bool tuple_match(Tuple const* t1, Tuple const* t2) {
    if (t1->element_count != t2->element_count) return false;

    for (size_t idx = 0; idx < t1->element_count; ++idx) {
	if (!tuple_element_match(&t1->elements[idx], &t2->elements[idx])) return false;
    }

    return true;
}



char const* tuple_to_string(Tuple const* tuple) {
    static thread_local char buffer[2048];
    int offset = snprintf(buffer, sizeof(buffer), "(");

    for (size_t idx = 0; idx < tuple->element_count; ++idx) {
	switch (tuple->elements[idx].type) {
	    case tuple_int:
                offset += snprintf(buffer + offset, sizeof(buffer), "%d", tuple_get_int(tuple, idx));
		break;

	    case tuple_float:
                offset += snprintf(buffer + offset, sizeof(buffer), "%g", tuple_get_float(tuple, idx));
		break;

	    case tuple_string:
                offset += snprintf(buffer + offset, sizeof(buffer), "\"%s\"", tuple_get_string(tuple, idx));
		break;

	    case tuple_int_template:
                offset += snprintf(buffer + offset, sizeof(buffer), "int?");
		break;

	    case tuple_float_template:
                offset += snprintf(buffer + offset, sizeof(buffer), "float?");
		break;

	    case tuple_string_template:
                offset += snprintf(buffer + offset, sizeof(buffer), "string?");
		break;
	}
	
	if (idx < tuple->element_count - 1) {
            offset += snprintf(buffer + offset, sizeof(buffer), ", ");
        }
    }

    snprintf(buffer + offset, sizeof(buffer), ")");

    return buffer;
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


static char const* tuple_element_deserialise(TupleElement* element, char const* buffer) {
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


char* tuple_serialise(Tuple const* tuple, char* buffer) {
    memcpy(buffer, &tuple->element_count, sizeof(tuple->element_count));
    buffer += sizeof(tuple->element_count);

    for (size_t idx = 0; idx < tuple->element_count; ++idx) {
	buffer = tuple_element_serialise(tuple->elements[idx], buffer);
    }

    return buffer;
}


char const* tuple_deserialise(Tuple* tuple, char const* buffer) {
    memcpy(&tuple->element_count, buffer, sizeof(tuple->element_count));
    buffer += sizeof(tuple->element_count);

    tuple->elements = malloc(tuple->element_count * sizeof(TupleElement));

    for (size_t idx = 0; idx < tuple->element_count; ++idx) {
	buffer = tuple_element_deserialise(&tuple->elements[idx], buffer);
    }

    return buffer;
}

size_t tuple_serialised_length(Tuple const* tuple) {
    size_t buffer_size = sizeof(tuple->element_count);

    for (size_t idx = 0; idx < tuple->element_count; ++idx) {
	buffer_size += sizeof(tuple->elements[idx].type);

	if (!(tuple->elements[idx].type & tuple_element_type_template_bit)) {
	    switch (tuple->elements[idx].type) {
		case tuple_int: 
		    buffer_size += sizeof(tuple->elements[idx].data.data_int);
		    break;

		case tuple_float: 
		    buffer_size += sizeof(tuple->elements[idx].data.data_float);
		    break;

		case tuple_string: 
		    buffer_size += sizeof(uint32_t) + strlen(tuple->elements[idx].data.data_string) + 1;
		    break;

		default: __builtin_unreachable();
	    }
	}
    }

    return buffer_size;
}

