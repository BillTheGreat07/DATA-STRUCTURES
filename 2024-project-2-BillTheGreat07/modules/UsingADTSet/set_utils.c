///////////////////////////////////////////////////////////
//
// Υλοποίηση του set_utils με γενικό τρόπο, χρησιμοποιώντας
// μια οποιαδήποτε υλοποίηση του ADT Set.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>

#include "set_utils.h"




Set set_from_vector(Vector vec, CompareFunc compare) {
    Set set = set_create(compare, NULL);
    int size = vector_size(vec);
    for(int i = 0; i < size; i++) {
        Pointer value = vector_get_at(vec, i);
        set_insert(set, value);
    }
    return set;
}

Vector set_to_vector(Set set) {
    Vector vec = vector_create(0, NULL);
    for(SetNode node = set_first(set); node != SET_EOF; node = set_next(set, node)) {
        Pointer value = set_node_value(set, node);
        vector_insert_last(vec, value);
    }
    return vec;
}

void set_traverse(Set set, TraverseFunc f) {
    for(SetNode node = set_first(set); node != SET_EOF; node = set_next(set, node)) {
        Pointer value = set_node_value(set, node);
        f(set, value);
    }
}

Set set_merge(Set set1, Set set2, CompareFunc compare) {
    Set new_set = set_create(compare, NULL);

    for(SetNode node = set_first(set1); node != SET_EOF; node = set_next(set1, node)) set_insert(new_set, set_node_value(set1, node));
    for(SetNode node = set_first(set2); node != SET_EOF; node = set_next(set2, node)) set_insert(new_set, set_node_value(set2, node));

    return new_set;
}

Pointer set_find_k_smallest(Set set, int k) {
    SetNode node = set_first(set);
    for(int i = 0; i < k; i++) node = set_next(set, node);

    return set_node_value(set, node);
}