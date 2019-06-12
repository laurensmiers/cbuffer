#pragma once

#include <stddef.h>

struct cbuffer;

struct cbuffer *cbuffer_init(void *data, size_t nr_elements, size_t size_of_element);

void *cbuffer_get_write_pointer(struct cbuffer *cbuffer);

void cbuffer_signal_element_written(struct cbuffer *cbuffer);

void *cbuffer_get_read_pointer(struct cbuffer *cbuffer);

void cbuffer_signal_element_read(struct cbuffer *cbuffer);

void cbuffer_destroy(struct cbuffer *cbuffer);
