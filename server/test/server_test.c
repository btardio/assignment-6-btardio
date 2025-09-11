#include "unity.h"
#include "server.h" // Header for the functions being tested

void setUp(void) {
}

void tearDown(void) {
}

void test_safe_malloc(void) {
    TEST_ASSERT_EQUAL(5, 5);
    void* v = safe_malloc(100);

    TEST_ASSERT_NOT_NULL(v);
}

#ifdef APPENDWRITE
#undef APPENDWRITE
#endif

void test_(void) {

    read_from_client(NULL, NULL, 1);
    TEST_ASSERT_EQUAL(-2, -2);
}

#ifndef APPENDWRITE
#define APPENDWRITE
#endif
/*
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_subtract_negative_result);
    return UNITY_END();
}
*/
