#include <threads.h>

typedef struct WorkerThreadArgs {

} WorkerThreadArgs;


typedef struct ServerWorker {
    struct sockaddr remote_address;

    WorkerThreadArgs* thread_args;
    thrd_t thread;

    InboundMessage* rx_queue;
    size_t rx_queue_length;
    mtx_t rx_queue_mtx;
    cnd_t rx_queue_cnd;
} ServerWorker;

ServerWorker server_worker_new(struct sockaddr remote_address);
void server_worker_free(ServerWorker server_worker);



typedef struct ServerWorkerPool {
    ServerWorker* workers;
    size_t worker_count;
} ServerWorkerPool;


ServerWorkerPool server_worker_pool_new();
void server_worker_pool_free(ServerWorkerPool pool);

void server_worker_pool_submit_inbound_message(ServerWorkerPool* pool, InboundMessage message);



int worker_thread_fn(void* args_);

