//////////////////////////////////////////////////////////////////
//
// Unit tests για το set_utils.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "set_utils.h"

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

void traverse_func(Set set, Pointer value) {
    int* val = (int*)value;
    printf("%d ", *val);
}

void test_set_from_vector(void) {
    Vector vec = vector_create(0, NULL);

    int values[] = {5, 1, 3, 7, 2};
    for(int i = 0; i < 5; i++) vector_insert_last(vec, &values[i]);

    Set set = set_from_vector(vec, compare_ints);
    TEST_CHECK(set_size(set) == 5);
    
    for(int i = 0; i < 5; i++) TEST_CHECK(set_find(set, &values[i]) != NULL);

    set_destroy(set);
    vector_destroy(vec);
}

void test_set_to_vector(void) {
    Set set = set_create(compare_ints, NULL);

    int values[] = {5, 1, 3, 7, 2};
    for(int i = 0; i < 5; i++) set_insert(set, &values[i]);

    Vector vec = set_to_vector(set);
    TEST_CHECK(vector_size(vec) == 5);

    int expected_values[] = {1, 2, 3, 5, 7};
    for(int i = 0; i < 5; i++) TEST_CHECK(*(int*)vector_get_at(vec, i) == expected_values[i]);

    vector_destroy(vec);
    set_destroy(set);
}

void test_set_traverse(void) {
    Set set = set_create(compare_ints, NULL);

    int values[] = {5, 1, 3, 7, 2};
    for(int i = 0; i < 5; i++) set_insert(set, &values[i]);

    set_traverse(set, traverse_func);
    printf("\n");

    set_destroy(set);
}

void test_set_merge(void) {
    Set set1 = set_create(compare_ints, NULL);
    Set set2 = set_create(compare_ints, NULL);
    int values1[] = {1, 3, 5};
    int values2[] = {2, 4, 6};

    for(int i = 0; i < 3; i++) {
        set_insert(set1, &values1[i]);
        set_insert(set2, &values2[i]);
    }

    Set merged_set = set_merge(set1, set2, compare_ints);
    TEST_CHECK(set_size(merged_set) == 6);

    int expected_values[] = {1, 2, 3, 4, 5, 6};
    SetNode node = set_first(merged_set);
    for(int i = 0; i < 6; i++) {
        TEST_CHECK(*(int*)set_node_value(merged_set, node) == expected_values[i]);
        node = set_next(merged_set, node);
    }

    set_destroy(set1);
    set_destroy(set2);
    set_destroy(merged_set);
}

void test_set_find_k_smallest(void) {
    Set set = set_create(compare_ints, NULL);
    
    int values[] = {5, 1, 3, 7, 2};
    for(int i = 0; i < 5; i++) set_insert(set, &values[i]);

    int k = 2;
    Pointer value = set_find_k_smallest(set, k);
    TEST_CHECK(*(int*)value == 3);

    set_destroy(set);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_set_import_from_vector",	test_set_from_vector },
	{ "test_set_export_to_vector",		test_set_to_vector },
	{ "test_set_traverse",				test_set_traverse },
	{ "test_set_merge",					test_set_merge },
	{ "test_set_find_k_smallest",		test_set_find_k_smallest },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
}; 
