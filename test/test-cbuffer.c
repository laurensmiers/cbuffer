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

void test_wraparound(void **state)
{
    struct cbuffer *cbuffer = (struct cbuffer *)*state;
    size_t index = 0;

    /* Force wraparound */
    for(size_t i = 0; i < NR_OF_TEST_ELEMENTS * 2; i++) {
        int *wp = cbuffer_get_write_pointer(cbuffer);
        assert_ptr_equal(&_test_data[index], wp);

        *wp = i + 1;
        cbuffer_signal_element_written(cbuffer);

        int *rp = cbuffer_get_read_pointer(cbuffer);
        assert_ptr_equal(&_test_data[index], rp);
        assert_int_equal(*rp, i + 1);

        cbuffer_signal_element_read(cbuffer);

        index++;

        if (index == NR_OF_TEST_ELEMENTS) {
            index = 0;
        }
    }
}

void test_overflow(void **state)
{
    /*
     * When the circular buffer is full, we should return NULL when asking a write pointer
     */
    struct cbuffer *cbuffer = (struct cbuffer *)*state;

    /* Fill the whole buffer */
    for(size_t i = 0; i < NR_OF_TEST_ELEMENTS; i++) {
        int *wp = cbuffer_get_write_pointer(cbuffer);
        assert_ptr_equal(&_test_data[i], wp);

        *wp = i + 1;
        cbuffer_signal_element_written(cbuffer);
    }

    /* Next time we ask for a write pointer, we should be notified the buffer is full */
    assert_ptr_equal(NULL, cbuffer_get_write_pointer(cbuffer));

    /* When we read again an element, we should be able to write again */
    assert_ptr_equal(&_test_data[0], cbuffer_get_read_pointer(cbuffer));
    cbuffer_signal_element_read(cbuffer);

    /* We should get a valid pointer now (and detect we wraparound) */
    assert_ptr_equal(&_test_data[0], cbuffer_get_write_pointer(cbuffer));
    cbuffer_signal_element_written(cbuffer);
}

void test_underrun(void **state)
{
    /*
     * When the circular buffer is empty, we should return NULL when asking a read pointer
     */
    struct cbuffer *cbuffer = (struct cbuffer *)*state;

    /* buffer is empty, we should not be able to get a read pointer */
    assert_ptr_equal(NULL, cbuffer_get_read_pointer(cbuffer));
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
        cmocka_unit_test_setup_teardown(test_wraparound, setup, teardown),
        cmocka_unit_test_setup_teardown(test_overflow, setup, teardown),
        cmocka_unit_test_setup_teardown(test_underrun, setup, teardown),
    };

    int count_fail_tests = cmocka_run_group_tests(test_init, NULL, NULL);
    count_fail_tests += cmocka_run_group_tests(test_operations, NULL, NULL);

    return count_fail_tests;
}
