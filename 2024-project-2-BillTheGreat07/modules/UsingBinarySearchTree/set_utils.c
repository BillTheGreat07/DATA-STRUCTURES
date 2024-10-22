///////////////////////////////////////////////////////////
//
// Υλοποίηση του set_utils για Sets βασισμένα σε Binary Search Tree.
//
///////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "set_utils.h"



// Χρησιμοποιούμε τη συγκεκριμένη υλοποίηση του UsingBinarySearchTree/ADTSet.c,
// οπότε γνωρίζουμε την ακριβή δομή για την αναπαράσταση των δεδομένων.
// Αντιγράφουμε εδώ τον ορισμό των structs ώστε να μπορούμε να προσπελάσουμε
// τα περιεχόμενά τους.

struct set {
	SetNode root;				// η ρίζα, NULL αν είναι κενό δέντρο
	int size;					// μέγεθος, ώστε η set_size να είναι Ο(1)
	CompareFunc compare;		// η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του set
};

struct set_node {
	SetNode left, right;		// Παιδιά
	Pointer value;
    int size;                   // Μέγεθος του υποδέντρου με ρίζα αυτόν τον κόμβο
};


// Βοηθητική στατική μεταβλητή CompareFunc για να χρησιμοποιήσουμε στην compare_wrapper()
static CompareFunc static_compare_func;

// Βοηθητική συνάρτηση για να χρησιμοποιήσουμε την CompareFunc σε qsort
static int compare_wrapper(const void* a, const void* b) {
    return static_compare_func(*(void**)a, *(void**)b);
}

// Βοηθητική συνάρτηση για δημιουργία balanced BST από sorted array
static SetNode create_balanced_bst_from_sorted_array(Pointer* array, int start, int end) {
    if(start > end) return NULL;

    int mid = (start + end) / 2;
    SetNode node = malloc(sizeof(*node));
    node->value = array[mid];
    node->left = create_balanced_bst_from_sorted_array(array, start, mid - 1);
    node->right = create_balanced_bst_from_sorted_array(array, mid + 1, end);
    return node;
}

Set set_from_vector(Vector vec, CompareFunc compare) {
    int size = vector_size(vec);
    Pointer* array = malloc(size * sizeof(*array));

    for(int i = 0; i < size; i++) array[i] = vector_get_at(vec, i);

    static_compare_func = compare;
    qsort(array, size, sizeof(*array), compare_wrapper);

    Set set = malloc(sizeof(*set));
    set->root = create_balanced_bst_from_sorted_array(array, 0, size - 1);
    set->size = size;
    set->compare = compare;
    set->destroy_value = NULL;

    free(array);
    return set;
}

// Βοηθητική συνάρτηση για in-order traversal και αρχικοποίηση του set
static void inorder_traverse_to_vector(SetNode node, Vector vec) {
    if (node == NULL) {
        return;
    }
    inorder_traverse_to_vector(node->left, vec);
    vector_insert_last(vec, node->value);
    inorder_traverse_to_vector(node->right, vec);
}

Vector set_to_vector(Set set) {
    Vector vec = vector_create(0, NULL);
    inorder_traverse_to_vector(set->root, vec);
    return vec;
}

// Βοηθητική συνάρτηση για in-order traversal και κλήση συνάρτησης f στα στοιχεία του set
static void inorder_traverse(SetNode node, Set set, TraverseFunc f) {
    if (node == NULL) {
        return;
    }
    inorder_traverse(node->left, set, f);
    f(set, node->value);
    inorder_traverse(node->right, set, f);
}

void set_traverse(Set set, TraverseFunc f) {
    inorder_traverse(set->root, set, f);
}

static Pointer* merge_sorted_arrays(Pointer* array1, int size1, Pointer* array2, int size2, CompareFunc compare) {
    Pointer* merged_array = malloc((size1 + size2) * sizeof(*merged_array));
    int i = 0, j = 0, k = 0;

    while(i < size1 && j < size2) {
        if(compare(array1[i], array2[j]) < 0) merged_array[k++] = array1[i++];
        else merged_array[k++] = array2[j++];
    }

    while(i < size1) merged_array[k++] = array1[i++];

    while(j < size2) merged_array[k++] = array2[j++];

    return merged_array;
}

Set set_merge(Set set1, Set set2, CompareFunc compare) {
    Vector vec1 = set_to_vector(set1);
    Vector vec2 = set_to_vector(set2);

    int size1 = vector_size(vec1);
    int size2 = vector_size(vec2);

    Pointer* array1 = malloc(size1 * sizeof(*array1));
    Pointer* array2 = malloc(size2 * sizeof(*array2));

    for(int i = 0; i < size1; i++) array1[i] = vector_get_at(vec1, i);

    for(int i = 0; i < size2; i++) array2[i] = vector_get_at(vec2, i);

    Pointer* merged_array = merge_sorted_arrays(array1, size1, array2, size2, compare);

    Set merged_set = malloc(sizeof(*merged_set));
    merged_set->root = create_balanced_bst_from_sorted_array(merged_array, 0, size1 + size2 - 1);
    merged_set->size = size1 + size2;
    merged_set->compare = compare;
    merged_set->destroy_value = NULL;

    free(array1);
    free(array2);
    free(merged_array);
    vector_destroy(vec1);
    vector_destroy(vec2);
    return merged_set;
}

Pointer set_find_k_smallest(Set set, int k) {
    SetNode node = set->root;

    while(node != NULL) {
        int left_size = node_size(node->left);
        if(k < left_size) node = node->left;
        else if (k > left_size) {
            k = k - left_size - 1;
            node = node->right;
        } else return node->value;
    }

    return NULL;
}