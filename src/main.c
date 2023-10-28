#include <stdio.h>
#include "tuple_space.h"

int main() {
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
}
