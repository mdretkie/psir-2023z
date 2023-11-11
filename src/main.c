#include <stdio.h>
#include "tuple_space.h"
#include "common.h"


int main() {
    /*
    Tuple t1 = tuple_new(
	    2,
	    tuple_int, 5,
	    tuple_string_template
    );

    Tuple t2 = tuple_new(
	    2,
	    tuple_int, 5,
	    tuple_string, "abd"
    );
    tuple_println(t1);
    tuple_println(t2);
    printf("%d\n", tuple_match(t1, t2));

    tuple_free(t1);
    tuple_free(t2);
    */

    Tuple t = tuple_new(
        6,
        tuple_int, 5,
        tuple_string, "abcd",
        tuple_int_template,
        tuple_float, 3.14,
        tuple_string_template,
        tuple_float_template
    );
    char* buffer = tuple_serialise(t);
    tuple_println(t);
    tuple_free(t);
    Tuple tt = tuple_deserialise(buffer);
    free(buffer);
    tuple_println(tt);
    tuple_free(tt);

}
