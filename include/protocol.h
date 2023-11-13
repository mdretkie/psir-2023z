#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "tuple.h"

typedef enum MessageType {
    message_ack,
    message_tuple_space_insert_request,
    message_tuple_space_get_request,
    message_tuple_space_get_reply,
} MessageType;

typedef struct MessageAck {
    uint32_t message_id;
} MessageAck;

typedef struct MessageTupleSpaceInsertRequest {
    Tuple tuple;
} MessageTupleSpaceInsertRequest;

typedef struct MessageTupleSpaceGetRequest {
    Tuple tuple_template;
    TupleSpaceOperationBlockingMode blocking_mode; 
    TupleSpaceOperationRemovePolicy remove_policy;
} MessageTupleSpaceGetRequest;

typedef struct MessageTupleSpaceGetReply {
    TupleSpaceOperationResult result;
} MessageTupleSpaceGetReply;

typedef union MessageData {
    MessageAck ack;
    MessageTupleSpaceInsertRequest tuple_space_insert_request;
    MessageTupleSpaceGetRequest tuple_space_get_request;
    MessageTupleSpaceGetReply tuple_space_get_reply;
} MessageData;

typedef struct Message {
    uint32_t id;
    MessageType type;
    uint32_t data_length;
    MessageData data;
} Message;

#endif
