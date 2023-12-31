#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include "thread_local.h"


uint32_t message_next_id() {
    static atomic_uint_least32_t id = 0;
    return atomic_fetch_add_explicit(&id, 1, memory_order_relaxed);
}


char* message_serialise_and_free(Message message) {
    char* buffer_start = malloc(message_serialised_length(&message));
    char* buffer = buffer_start;

    memcpy(buffer, &message.id, sizeof(message.id));
    buffer += sizeof(message.id);

    memcpy(buffer, &message.type, sizeof(message.type));
    buffer += sizeof(message.type);

    switch (message.type) {
	case message_ack: 
	    memcpy(buffer, &message.data.ack.message_id, sizeof(message.data.ack.message_id));
	    buffer += sizeof(message.data.ack.message_id);
	    break;

	case message_tuple_space_insert_request:
	    buffer = tuple_serialise(&message.data.tuple_space_insert_request.tuple, buffer);

	    tuple_free(message.data.tuple_space_insert_request.tuple);
	    break;

	case message_tuple_space_get_request:
	    buffer = tuple_serialise(&message.data.tuple_space_get_request.tuple_template, buffer);
	    memcpy(buffer, &message.data.tuple_space_get_request.blocking_mode, sizeof(message.data.tuple_space_get_request.blocking_mode));
	    buffer += sizeof(message.data.tuple_space_get_request.blocking_mode);
	    memcpy(buffer, &message.data.tuple_space_get_request.remove_policy, sizeof(message.data.tuple_space_get_request.remove_policy));
	    buffer += sizeof(message.data.tuple_space_get_request.remove_policy);

	    tuple_free(message.data.tuple_space_get_request.tuple_template);
	    break;

	case message_tuple_space_get_reply:
	    memcpy(buffer, &message.data.tuple_space_get_reply.result.status, sizeof(message.data.tuple_space_get_reply.result.status));
	    buffer += sizeof(message.data.tuple_space_get_reply.result.status);
	    buffer = tuple_serialise(&message.data.tuple_space_get_reply.result.tuple, buffer);

	    tuple_free(message.data.tuple_space_get_reply.result.tuple);
	    break;
    }

    return buffer_start;
}

Message message_deserialise_no_free(char* buffer_) {
    /* Żeby nie było problemu przy przypisywaniu char const* (z tuple_deserialise()) do char*,
     * i jednocześnie potrzebujemy char*, aby użyć free(). */
    char const* buffer = buffer_;

    Message message;

    memcpy(&message.id, buffer, sizeof(message.id));
    buffer += sizeof(message.id);

    memcpy(&message.type, buffer, sizeof(message.type));
    buffer += sizeof(message.type);

    switch (message.type) {
	case message_ack: 
	    memcpy(&message.data.ack.message_id, buffer, sizeof(message.data.ack.message_id));
	    buffer += sizeof(message.data.ack.message_id);
	    break;

	case message_tuple_space_insert_request:
	    buffer = tuple_deserialise(&message.data.tuple_space_insert_request.tuple, buffer);
	    break;

	case message_tuple_space_get_request:
	    buffer = tuple_deserialise(&message.data.tuple_space_get_request.tuple_template, buffer);
	    memcpy(&message.data.tuple_space_get_request.blocking_mode, buffer, sizeof(message.data.tuple_space_get_request.blocking_mode));
	    buffer += sizeof(message.data.tuple_space_get_request.blocking_mode);
	    memcpy(&message.data.tuple_space_get_request.remove_policy, buffer, sizeof(message.data.tuple_space_get_request.remove_policy));
	    buffer += sizeof(message.data.tuple_space_get_request.remove_policy);
	    break;

	case message_tuple_space_get_reply:
	    memcpy(&message.data.tuple_space_get_reply.result.status, buffer, sizeof(message.data.tuple_space_get_reply.result.status));
	    buffer += sizeof(message.data.tuple_space_get_reply.result.status);
	    buffer = tuple_deserialise(&message.data.tuple_space_get_reply.result.tuple, buffer);
	    break;
    }

    return message;
}

Message message_deserialise_and_free(char* buffer_) {
    Message message = message_deserialise_no_free(buffer_);
    free(buffer_);
    return message;
}

size_t message_serialised_length(Message const* message) {
    size_t size = 
	sizeof(message->id) + 
	sizeof(message->type);

    switch (message->type) {
	case message_ack: 
	    size += sizeof(message->data.ack.message_id); 
	    break;

	case message_tuple_space_insert_request:
	    size += tuple_serialised_length(&message->data.tuple_space_insert_request.tuple);
	    break;

	case message_tuple_space_get_request:
	    size += tuple_serialised_length(&message->data.tuple_space_get_request.tuple_template) +
		sizeof(message->data.tuple_space_get_request.blocking_mode) +
		sizeof(message->data.tuple_space_get_request.remove_policy);
	    break;

	case message_tuple_space_get_reply:
	     size += tuple_serialised_length(&message->data.tuple_space_get_reply.result.tuple) +
		sizeof(message->data.tuple_space_get_reply.result.status);
	    break;
    }

    return size;
}

#ifndef PSIR_ARDUINO

char const* message_to_string_short(Message const* message) {
    static thread_local char buffer[2048];

    char const* type_name =
	message->type == message_ack ? "ACK": 
	message->type == message_tuple_space_insert_request ? "TUPLE SPACE INSERT REQUEST":
	message->type == message_tuple_space_get_request ? "TUPLE SPACE GET REQUEST":
	message->type == message_tuple_space_get_reply ? "TUPLE SPACE GET REPLY": 
	"INVALID";

    switch (message->type) {
	case message_ack: 
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %u", message->id, type_name, message->data.ack.message_id); 
	    break;

	case message_tuple_space_insert_request:
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_insert_request.tuple)); 
	    break;

	case message_tuple_space_get_request:
            (void)0;

            char const* blocking_mode = 
                message->data.tuple_space_get_request.blocking_mode == tuple_space_blocking ? "blocking":
                message->data.tuple_space_get_request.blocking_mode == tuple_space_nonblocking ? "nonblocking":
                "invalid blocking mode";

            char const* remove_policy = 
                message->data.tuple_space_get_request.remove_policy == tuple_space_remove ? "remove":
                message->data.tuple_space_get_request.remove_policy == tuple_space_keep ? "keep":
                "invalid remove policy";

	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_get_request.tuple_template), blocking_mode, remove_policy); 
	    break;

	case message_tuple_space_get_reply:
            (void)0;
            
            char const* status = 
                message->data.tuple_space_get_reply.result.status == tuple_space_success ? "success":
                message->data.tuple_space_get_reply.result.status == tuple_space_failure ? "failure":
                "invalid status";

	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_get_reply.result.tuple), status); 
	    break;

	default: 
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s", message->id, type_name);
	    break;
    }

    return buffer;
}

#endif
