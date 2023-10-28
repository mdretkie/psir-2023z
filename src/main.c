#include <stdio.h>
#include "tuple_space.h"

int main() {
    Tuple tuple = tuple_new(
	    7, 
	    tuple_int, 5, 
	    tuple_int_template,
	    tuple_float, -3.2, 
	    tuple_float_template,
	    tuple_int, 10, 
	    tuple_string, "Text",
	    tuple_string_template
    );
    tuple_print(tuple);
    tuple_free(tuple);
}
