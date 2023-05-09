#include "set_utils.h"
#include <stdlib.h>

Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer foundvalue = malloc(sizeof(*value));
    if (set_size(set) == 0){
        foundvalue = NULL;
    }
    else if(set_find_node(set, value) == SET_EOF){
        set_insert(set, value);
        if(set_next(set, set_find_node(set, value)) == SET_EOF){
            set_remove(set, value);
            free(foundvalue);
            return NULL;
        }
        foundvalue = set_node_value(set, set_next(set, set_find_node(set, value)));
        set_remove(set, value);
    }
    else{
        foundvalue = set_node_value(set, set_find_node(set, value));
    }
    return foundvalue;
}


Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer foundvalue = malloc(sizeof(*value));
    if (set_size(set) == 0){
        foundvalue = NULL;
    }
    else if(set_find_node(set, value) == SET_EOF){
        set_insert(set, value);
        if(set_previous(set, set_find_node(set, value)) == SET_BOF){
            set_remove(set, value);
            free(foundvalue);
            return NULL;
        }
        foundvalue = set_node_value(set, set_previous(set, set_find_node(set, value)));
        set_remove(set, value);
    }
    else{
        foundvalue = set_node_value(set, set_find_node(set, value));
    }
    return foundvalue;
}