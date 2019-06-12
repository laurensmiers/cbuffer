#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cbuffer.h"

struct cbuffer {
    void *data;
    void *wp;
    void *rp;
    size_t count;
    size_t nr_elements;
    size_t size_of_element;
};

struct cbuffer *cbuffer_init(void *data, size_t nr_elements, size_t size_of_element)
{
    struct cbuffer *cbuffer = (struct cbuffer *)malloc(sizeof(struct cbuffer));

    memset(cbuffer, 0, sizeof(struct cbuffer));

    cbuffer->data = data;
    cbuffer->wp = cbuffer->data;
    cbuffer->rp = cbuffer->data;
    cbuffer->nr_elements = nr_elements;
    cbuffer->size_of_element = size_of_element;
    cbuffer->count = 0;

    return cbuffer;
}

void *cbuffer_get_write_pointer(struct cbuffer *cbuffer)
{
    return cbuffer->wp;
}

void cbuffer_signal_element_written(struct cbuffer *cbuffer)
{
    cbuffer->wp = (void *)((uint8_t *)(cbuffer->wp) + cbuffer->size_of_element);

    /* TODO: wraparound */
}

void *cbuffer_get_read_pointer(struct cbuffer *cbuffer)
{
    return cbuffer->rp;
}

void cbuffer_signal_element_read(struct cbuffer *cbuffer)
{
    cbuffer->rp = (void *)((uint8_t *)(cbuffer->rp) + cbuffer->size_of_element);

    /* TODO: wraparound */
}

void cbuffer_destroy(struct cbuffer *cbuffer)
{
    free(cbuffer);

    cbuffer = NULL;
}
