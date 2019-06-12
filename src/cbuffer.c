#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cbuffer.h"

#define GET_POINTER_VAL(_base, _increment, _stepsize) \
    (void *)((uint8_t *)(_base) + (_stepsize) * (_increment));

struct cbuffer {
    void *data;
    void *wp;
    void *rp;
    size_t size;
    size_t size_of_element;
    size_t count;
};

struct cbuffer *cbuffer_init(void *data, size_t size, size_t size_of_element)
{
    struct cbuffer *cbuffer = (struct cbuffer *)malloc(sizeof(struct cbuffer));

    memset(cbuffer, 0, sizeof(struct cbuffer));

    cbuffer->data = data;
    cbuffer->wp = cbuffer->data;
    cbuffer->rp = cbuffer->data;
    cbuffer->size = size;
    cbuffer->size_of_element = size_of_element;
    cbuffer->count = 0;

    return cbuffer;
}

void *cbuffer_get_write_pointer(struct cbuffer *cbuffer)
{
    if (cbuffer->count == cbuffer->size) {
        return NULL;
    }

    return cbuffer->wp;
}

void cbuffer_signal_element_written(struct cbuffer *cbuffer)
{
    void *endptr = GET_POINTER_VAL(cbuffer->data, cbuffer->size_of_element, cbuffer->size);

    cbuffer->wp = GET_POINTER_VAL(cbuffer->wp, cbuffer->size_of_element, 1);

    if (endptr == cbuffer->wp) {
        cbuffer->wp = cbuffer->data;
    }

    cbuffer->count++;
}

void *cbuffer_get_read_pointer(struct cbuffer *cbuffer)
{
    if (0 == cbuffer->count) {
        return NULL;
    }

    return cbuffer->rp;
}

void cbuffer_signal_element_read(struct cbuffer *cbuffer)
{
    void *endptr = GET_POINTER_VAL(cbuffer->data, cbuffer->size_of_element, cbuffer->size);

    cbuffer->rp = GET_POINTER_VAL(cbuffer->rp, cbuffer->size_of_element, 1);

    if (endptr == cbuffer->rp) {
        cbuffer->rp = cbuffer->data;
    }

    cbuffer->count--;
}

void cbuffer_destroy(struct cbuffer *cbuffer)
{
    free(cbuffer);

    cbuffer = NULL;
}
