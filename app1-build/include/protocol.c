#include "protocol.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "arduino.h"
#ifndef PSIR_ARDUINO
#include <stdatomic.h>
#endif


uint32_t message_next_id() {
    #ifdef PSIR_ARDUINO
    static uint32_t id = 0;
    return id++;
    #else
    static atomic_uint_least32_t id = 0;
    return atomic_fetch_add_explicit(&id, 1, memory_order_relaxed);
    #endif
}


char* message_serialise_and_free(Message message) {
    char* buffer_start = (char*)malloc(message_serialised_length(&message));
    char* buffer = buffer_start;

    buffer = serialise_u32(buffer, message.id);
    buffer = serialise_u32(buffer, message.type);

    switch (message.type) {
	case message_ack: 
            buffer = serialise_u32(buffer, message.data.ack.message_id);
	    break;

	case message_tuple_space_insert_request:
	    buffer = tuple_serialise(&message.data.tuple_space_insert_request.tuple, buffer);

	    tuple_free(message.data.tuple_space_insert_request.tuple);
	    break;

	case message_tuple_space_get_request:
	    buffer = tuple_serialise(&message.data.tuple_space_get_request.tuple_template, buffer);
            buffer = serialise_u32(buffer, message.data.tuple_space_get_request.blocking_mode);
            buffer = serialise_u32(buffer, message.data.tuple_space_get_request.remove_policy);

	    tuple_free(message.data.tuple_space_get_request.tuple_template);
	    break;

	case message_tuple_space_get_reply:
            buffer = serialise_u32(buffer, message.data.tuple_space_get_reply.result.status);
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

    buffer = deserialise_u32(buffer, &message.id);
    //Serial.print("Message id ");
    //Serial.println(message.id);
    buffer = deserialise_u32(buffer, &message.type);
    //Serial.print("Message type ");
    //Serial.println(message.type);

    switch (message.type) {
	case message_ack: 
            buffer = deserialise_u32(buffer, &message.data.ack.message_id);
            //Serial.print("Message ack ");
            //Serial.println(message.data.ack.message_id);
	    break;

	case message_tuple_space_insert_request:
	    buffer = tuple_deserialise(&message.data.tuple_space_insert_request.tuple, buffer);
	    break;

	case message_tuple_space_get_request:
	    buffer = tuple_deserialise(&message.data.tuple_space_get_request.tuple_template, buffer);
            buffer = deserialise_u32(buffer, &message.data.tuple_space_get_request.blocking_mode);
            buffer = deserialise_u32(buffer, &message.data.tuple_space_get_request.remove_policy);
	    break;

	case message_tuple_space_get_reply:
            buffer = deserialise_u32(buffer, &message.data.tuple_space_get_reply.result.status);
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
	sizeof((uint32_t)message->id) + 
	sizeof((uint32_t)message->type);

    switch (message->type) {
	case message_ack: 
	    size += sizeof((uint32_t)message->data.ack.message_id); 
	    break;

	case message_tuple_space_insert_request:
	    size += tuple_serialised_length(&message->data.tuple_space_insert_request.tuple);
	    break;

	case message_tuple_space_get_request:
	    size += 
                tuple_serialised_length(&message->data.tuple_space_get_request.tuple_template) +
		sizeof((uint32_t)message->data.tuple_space_get_request.blocking_mode) +
		sizeof((uint32_t)message->data.tuple_space_get_request.remove_policy);
	    break;

	case message_tuple_space_get_reply:
	     size += 
                 tuple_serialised_length(&message->data.tuple_space_get_reply.result.tuple) +
		sizeof((uint32_t)message->data.tuple_space_get_reply.result.status);
	    break;
    }

    return size;
}


char const* message_to_string_short(Message const* message) {
    #ifdef PSIR_ARDUINO
    static char buffer[64];
    #else
    static thread_local char buffer[2048];
    #endif

    char const* type_name =
	message->type == message_ack ? "ACK": 
	message->type == message_tuple_space_insert_request ? "TUPLE SPACE INSERT REQUEST":
	message->type == message_tuple_space_get_request ? "TUPLE SPACE GET REQUEST":
	message->type == message_tuple_space_get_reply ? "TUPLE SPACE GET REPLY": 
	"INVALID";

    switch (message->type) {
	case message_ack: {
            #ifdef PSIR_ARDUINO
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %u", (unsigned)message->id, type_name, (unsigned)message->data.ack.message_id); 
            #else
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %u", message->id, type_name, message->data.ack.message_id); 
            #endif
	    break;

	case message_tuple_space_insert_request:
            #ifdef PSIR_ARDUINO
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s", (unsigned)message->id, type_name, tuple_to_string(&message->data.tuple_space_insert_request.tuple)); 
            #else
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_insert_request.tuple)); 
            #endif
	    break;
        }

	case message_tuple_space_get_request: {
            (void)0;

            char const* blocking_mode = 
                message->data.tuple_space_get_request.blocking_mode == tuple_space_blocking ? "blocking":
                message->data.tuple_space_get_request.blocking_mode == tuple_space_nonblocking ? "nonblocking":
                "invalid blocking mode";

            char const* remove_policy = 
                message->data.tuple_space_get_request.remove_policy == tuple_space_remove ? "remove":
                message->data.tuple_space_get_request.remove_policy == tuple_space_keep ? "keep":
                "invalid remove policy";

            #ifdef PSIR_ARDUINO
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s %s", (unsigned)message->id, type_name, tuple_to_string(&message->data.tuple_space_get_request.tuple_template), blocking_mode, remove_policy); 
            #else
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_get_request.tuple_template), blocking_mode, remove_policy); 
            #endif
	    break;
        }

	case message_tuple_space_get_reply: {
            (void)0;
            
            char const* status = 
                message->data.tuple_space_get_reply.result.status == tuple_space_success ? "success":
                message->data.tuple_space_get_reply.result.status == tuple_space_failure ? "failure":
                "invalid status";

            #ifdef PSIR_ARDUINO
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s", (unsigned)message->id, type_name, tuple_to_string(&message->data.tuple_space_get_reply.result.tuple), status); 
            #else
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s: %s %s", message->id, type_name, tuple_to_string(&message->data.tuple_space_get_reply.result.tuple), status); 
            #endif
	    break;
        }

	default: {
            #ifdef PSIR_ARDUINO
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s", (unsigned)message->id, type_name);
            #else
	    snprintf(buffer, sizeof(buffer), "(id: %u) %s", message->id, type_name);
            #endif
	    break;
        }
    }

    return buffer;
}

