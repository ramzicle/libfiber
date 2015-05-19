/*
 * Copyright (c) 2012-2015, Brian Watling and other contributors
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "fiber_manager.h"
#include "test_helper.h"

#define NUM_THREADS 4
#define NUM_FIBERS 10000
#define PER_FIBER_COUNT 10000

mpsc_fifo_t fifo;

void* run_function(void* param)
{
    int i;
    for(i = 0; i < PER_FIBER_COUNT; ++i) {
        fiber_manager_wait_in_mpsc_queue(fiber_manager_get(), &fifo);
    }
    return NULL;
}

int main()
{
    fiber_manager_init(NUM_THREADS);
    mpsc_fifo_init(&fifo);

    fiber_t* fibers[NUM_FIBERS];
    int i;
    for(i = 0; i < NUM_FIBERS; ++i) {
        fibers[i] = fiber_create(20000, &run_function, NULL);
    }

    fiber_yield();

    for(i = 0; i < PER_FIBER_COUNT; ++i) {
        fiber_manager_wake_from_mpsc_queue(fiber_manager_get(), &fifo, NUM_FIBERS);
    }

    for(i = 0; i < NUM_FIBERS; ++i) {
        fiber_join(fibers[i], NULL);
    }

    mpsc_fifo_destroy(&fifo);

    fiber_manager_print_stats();
    return 0;
}
