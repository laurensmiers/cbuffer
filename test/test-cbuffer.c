#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "cbuffer.h"

#define NR_OF_TEST_ELEMENTS 5

static int _test_data[NR_OF_TEST_ELEMENTS];

void test_initialization(void ** state)
{
    struct cbuffer *cbuffer = cbuffer_init(_test_data, NR_OF_TEST_ELEMENTS, sizeof(_test_data[0]));

    assert_non_null(cbuffer);

    cbuffer_destroy(cbuffer);
}

void test_adding_one_element(void ** state)
{
    struct cbuffer *cbuffer = (struct cbuffer *)*state;

    int *wp = cbuffer_get_write_pointer(cbuffer);

    assert_non_null(wp);
    assert_ptr_equal(&_test_data[0], wp);

    *wp = 1000;

    cbuffer_signal_element_written(cbuffer);

    int *rp = cbuffer_get_read_pointer(cbuffer);

    assert_non_null(rp);
    assert_ptr_equal(&_test_data[0], rp);

    assert_int_equal(*rp, 1000);

    cbuffer_signal_element_read(cbuffer);
}

void test_full_write_and_readout(void **state)
{
    struct cbuffer *cbuffer = (struct cbuffer *)*state;

    for(size_t i = 0; i < NR_OF_TEST_ELEMENTS; i++) {
        int *wp = cbuffer_get_write_pointer(cbuffer);
        assert_ptr_equal(&_test_data[i], wp);

        *wp = i + 1;
        cbuffer_signal_element_written(cbuffer);

        int *rp = cbuffer_get_read_pointer(cbuffer);
        assert_ptr_equal(&_test_data[i], rp);
        assert_int_equal(*rp, i + 1);

        cbuffer_signal_element_read(cbuffer);
    }
}

int setup (void ** state)
{
    struct cbuffer *cbuffer = cbuffer_init(_test_data, NR_OF_TEST_ELEMENTS, sizeof(_test_data[0]));

    *state = cbuffer;

    return 0;
}

int teardown (void ** state)
{
    struct cbuffer *cbuffer = (struct cbuffer *)*state;

    cbuffer_destroy(cbuffer);

    return 0;
}

int main(void)
{
    const struct CMUnitTest test_init [] =
    {
        cmocka_unit_test(test_initialization),
    };
    const struct CMUnitTest test_operations [] =
    {
        cmocka_unit_test_setup_teardown(test_adding_one_element, setup, teardown),
        cmocka_unit_test_setup_teardown(test_full_write_and_readout, setup, teardown),
    };

    int count_fail_tests = cmocka_run_group_tests(test_init, NULL, NULL);
    count_fail_tests += cmocka_run_group_tests(test_operations, NULL, NULL);

    return count_fail_tests;
}
