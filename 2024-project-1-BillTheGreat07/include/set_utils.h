#pragma once

#include <stdlib.h>
#include "ADTSet.h"


Pointer set_find_eq_or_greater(Set set, Pointer value) {
    SetNode node = set_find_node(set, value);
    if(node != SET_EOF) {
        return set_node_value(set, node);
    } else {
        // Εισαγωγή του value προσωρινά για την εύρεση του επόμενου μεγαλύτερου
        set_insert(set, value);
        node = set_next(set, set_find_node(set, value));
        Pointer result = (node != SET_EOF) ? set_node_value(set, node) : NULL;
        set_remove(set, value); // Αφαίρεση της προσωρινής τιμής
        return result;
    }
}

Pointer set_find_eq_or_smaller(Set set, Pointer value) {
    SetNode node = set_find_node(set, value);
    if (node != SET_EOF) {
        return set_node_value(set, node);
    } else {
        // Εισάγετε προσωρινά το value για να βρείτε το προηγούμενο μικρότερο
        set_insert(set, value);
        node = set_previous(set, set_find_node(set, value));
        Pointer result = (node != SET_EOF) ? set_node_value(set, node) : NULL;
        set_remove(set, value); // Αφαίρεση της προσωρινής τιμής
        return result;
    }
}