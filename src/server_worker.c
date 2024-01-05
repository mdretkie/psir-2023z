#include "server_worker.h"

ServerWorker server_worker_new(struct sockaddr remote_address) {
    ServerWorker worker;

    worker.remote_address = remote_address;
    worker.thread_args = malloc(sizeof(WorkerThreadArgs));
    thrd_create(&worker.thread, worker_thread_fn, worker.thread_args);
    worker.rx_queue = NULL;
    worker.rx_queue_length = 0;
    mtx_init(&worker.rx_queue_mtx, mtx_plain);
    cnd_init(&worker.rx_queue_cnd);

    return worker;
}


void server_worker_free(ServerWorker worker) {
    thrd_join(worker.thread, NULL)
    free(worker.thread_args);
    free(worker.rx_queue);
    mtx_destroy(&worker.rx_queue_mtx);
    cnd_destroy(&worker.rx_queue_cnd);
}


ServerWorkerPool server_worker_pool_new() {
    ServerWorkerPool pool {
        .workers = NULL,
        .worker_count = 0,
    };
}

void server_worker_pool_free(ServerWorkerPool pool) {
    free(pool.workers);
}

void server_worker_pool_submit_inbound_message(ServerWorkerPool* pool, InboundMessage message) {
    for (size_t i = 0; i < pool->worker_count; ++i) {
        if (memcmp(&message.sender_address, pool->workers[i].remote_address, sizeof(struct sockaddr)) == 0) {
            mtx_lock(&pool->workers[i].rx_queue_mx);

            pool->workers[i].rx_queue = realloc(pool->workers[i].rx_queue, (pool->workers[i].rx_queue_length + 1) * sizeof(InboundMessage));
            pool->workers[i].rx_queue_length += 1;
            pool->workers[i].rx_queue[pool->workers[i].rx_queue_length - 1] = message;

            mtx_unlock(&pool->workers[i].rx_queue_mx);
            cnd_broadcast(&pool->workers[i].rx_queue_cnd);
            return;
        }
    }


}

int worker_thread_fn(void* args_) {
    args = *(WorkerThreadArgs*)(args_);

    return 0;
}

