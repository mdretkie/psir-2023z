#include <stdio.h>
#include "tuple_space.h"

int main() {
    Tuple tuple = tuple_new(3, TupleInt, 5, TupleFloat, -3.2, TupleInt, 10);
    tuple_print(tuple);
    tuple_free(tuple);
    puts("ok");
}
