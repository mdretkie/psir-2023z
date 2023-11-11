#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "tuple.h"

/*
typedef enum MessageType {
    message_ack,
    message_tuple_space_insert,
    message_tuple_space_get,
} MessageType;

typedef struct MessageAck {
    uint32_t message_id;
} MessageAck;

typedef struct MessageTupleSpaceInsert {
    Tuple tuple;
} MessageTupleSpaceInsert;

typedef struct MessageTupleSpaceGet {
    Tuple tuple_template;
    TupleSpaceOperationBlockingMode blocking_mode; 
    TupleSpaceGetPolicy remove_policy;
} MessageTupleSpaceGet;

typedef union MessageData {
    MessageAck ack;
    MessageTupleSpaceInsert tuple_space_insert;
    MessageTupleSpaceGet tuple_space_get;
} MessageData;

typedef struct Message {
    uint32_t id;
    MessageType type;
    uint32_t data_length;
    MessageData data;
} Message;
*/

#endif
