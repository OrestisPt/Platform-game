#include "acutest.h"
#include "set_utils.h"

int compare_ints(Pointer a, Pointer b) {
    if(*(int*)a < *(int*)b) return -1;
    if(*(int*)a > *(int*)b) return 1;
    return 0;
}

Pointer create_int(int value) {
    int *p = malloc(sizeof(int));
    *p = value;
    return p;
}

void test_set_utils() {
    Set set = set_create(compare_ints, free);

    Pointer test = set_find_eq_or_greater(set, create_int(1));
    TEST_ASSERT(test == NULL);

    set_insert(set, create_int(1));
    test = set_find_eq_or_greater(set, create_int(1));
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(*(int*)test == 1);

    set_insert(set, create_int(5));
    test = set_find_eq_or_greater(set, create_int(2));
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(*(int*)test == 5);

    test = set_find_eq_or_smaller(set, create_int(2));
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(*(int*)test == 1);

    test = set_find_eq_or_greater(set, create_int(6));
    TEST_ASSERT(test == NULL);

    test = set_find_eq_or_smaller(set, create_int(0));
    TEST_ASSERT(test == NULL);

    set_destroy(set);

    set = set_create((CompareFunc)strcmp, NULL);
    test = set_find_eq_or_greater(set, create_int(1));
    TEST_ASSERT(test == NULL);

    set_insert(set, "aaa");
    test = set_find_eq_or_greater(set, "aaa");
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(strcmp((char*)test, "aaa") == 0);

    set_insert(set, "bbb");
    test = set_find_eq_or_greater(set, "aab");
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(strcmp((char*)test, "bbb") == 0);

    test = set_find_eq_or_smaller(set, "aab");
    TEST_ASSERT(test != NULL);
    TEST_ASSERT(strcmp((char*)test, "aaa") == 0);

    test = set_find_eq_or_greater(set, "ccc");
    TEST_ASSERT(test == NULL);

    test = set_find_eq_or_smaller(set, "aa");
    TEST_ASSERT(test == NULL);
}

TEST_LIST = {
    { "set_utils", test_set_utils },
    { NULL, NULL }
};