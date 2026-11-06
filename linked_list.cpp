#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>
#include <stdint.h>
#include <graphviz/cgraph.h>
#include "linked_list_conf.h"

errors_list init_linked_list    (linked_list*);
errors_list verify_list         (linked_list*);
void        list_dump           (linked_list*, errors_list);
int         is_exist_cycle      (linked_list*);
errors_list init_free_elems     (linked_list*);
errors_list insert              (linked_list*, int, int);
errors_list pop                 (linked_list*, int);
ssize_t     find_free_index     (linked_list*);
ssize_t     find_index          (linked_list*, ssize_t);
errors_list resize_array        (linked_list*);
errors_list add                 (linked_list*, int);
char*       make_html_list_node (Agraph_t*, list_elem*, size_t);
void        print_picture       (linked_list* list);
void        make_nodes          (Agraph_t*, Agnode_t**, linked_list*);
void        make_edges          (Agraph_t*, Agnode_t**, linked_list*);
FILE*       get_output_stream   ();
int         get_size_of_file    (const char*);
void        write_in_html       ();

int main() {

    linked_list list = {};
    init_linked_list(&list);

    for(int i = 1; i < 13; i++) {
        add(&list, i*10);
    }
    insert(&list, 35, 3);
    insert(&list, 39, 3);
    list_dump(&list, EL_NO_ERRORS);

    pop(&list, 3);
    pop(&list, 4);
    list_dump(&list, EL_NO_ERRORS);

    return 0;
}

errors_list init_linked_list(linked_list* list){
    assert(list);

    list->head = 0;
    list->tail = 0;

    list_elem* temp_array = (list_elem*)calloc(MIN_SIZE, sizeof(list_elem));
    if (temp_array == NULL) list_dump(list, EL_LIST_ARRAY_CREATE_FAILED);
    assert(temp_array);
    list->list_array = temp_array;

    list->size = MIN_SIZE;
    init_free_elems(list);
    list->count_of_elem = 0;

    assert(!verify_list(list));
    return verify_list(list);
}

errors_list init_free_elems(linked_list* list) {
    assert(list);

    if (list->list_array == NULL) list_dump(list, EL_LIST_ARRAY_CREATE_FAILED);
    assert(list->list_array);

    list->list_array[0].next = 0;
    list->list_array[0].value = POISON_VALUE;
    list->list_array[0].prev = 0;

    for (size_t index = 1; index < list->size; index++) {
        list->list_array[index].next = index + 1;
        list->list_array[index].value = POISON_VALUE;
        list->list_array[index].prev = index - 1;
    }

    list->list_array[list->size - 1].next = 0;

    list->first_free = 1;
    return verify_list(list);
}

errors_list verify_list(linked_list* list) {
    if (list == NULL) {
        list_dump(list, EL_LIST_CREATE_FAILED);
        return EL_LIST_CREATE_FAILED;
    }
    if (list->first_free <= 0) {
        list_dump(list, EL_NOT_EXIST_FREE_ELEM);
        return EL_NOT_EXIST_FREE_ELEM;
    }
    if (list->list_array == NULL) {
        list_dump(list, EL_LIST_ARRAY_CREATE_FAILED);
        return EL_LIST_ARRAY_CREATE_FAILED;
    }
    if (list->size < MIN_SIZE || list->count_of_elem >= (ssize_t)list->size) {
        list_dump(list, EL_NOT_CORRECT_SIZE);
        return EL_NOT_CORRECT_SIZE;
    }
    if (list->head < 0 || list->tail < 0) {
        list_dump(list, EL_ERROR_HEAD_OR_TAIL);
        return EL_ERROR_HEAD_OR_TAIL;
    }
    if (is_exist_cycle(list)) {
        list_dump(list, EL_EXIST_CYCLE);
        return EL_EXIST_CYCLE;
    }
    if (log_file == NULL) {
        list_dump(list, EL_LOG_FILE_NOT_OPEN);
        return EL_LOG_FILE_NOT_OPEN;
    }
    return EL_NO_ERRORS;
}

int is_exist_cycle(linked_list* list) {
    assert(list);

    ssize_t start_index = list->tail;
    for(ssize_t iterator = 0; iterator < list->count_of_elem - 1; iterator++) {
        start_index = list->list_array[start_index].next;
    }

    return (start_index == list->head) ? 0 : 1;
}

void list_dump(linked_list* list, errors_list error_code) {

    FILE* stream = get_output_stream();
    fprintf(stream, "<pre>\n");
    fprintf(stream, "%s\n", el_errors_message[error_code]);

    if (error_code == EL_LIST_CREATE_FAILED) {
        fprintf(stream, "<pre><hr>\n<hr>\n");
        fflush(stream);
        return;
    }
    fprintf(stream, "tail <%lld> head <%lld>\n", list->tail, list->head);
    fprintf(stream, "size <%lld>\n", list->size);
    fprintf(stream, "first free <%lld>\n", list->first_free);
    fprintf(stream, "count elem <%lld>\n", list->count_of_elem);
    fprintf(stream, "<pre>\n");
    if (error_code != EL_LIST_ARRAY_CREATE_FAILED && error_code != EL_LIST_CREATE_FAILED) {
        print_picture(list);
    }
    fprintf(stream, "<hr>\n");
    fprintf(stream, "\n\n");
    fflush(stream);
}

errors_list insert(linked_list* list, int value, int index) {
    assert(list);

    if (index >= list->count_of_elem || index < 0) {
        list_dump(list, EL_NOT_CORRECT_POP_INDEX);
        return EL_NOT_CORRECT_POP_INDEX;
    }

    ssize_t next_free = find_free_index(list);
    ssize_t real_index = find_index(list, index);

    list->list_array[next_free].value = value;

    list->list_array[next_free].next = real_index;
    if(real_index != list->tail) list->list_array[list->list_array[real_index].prev].next = next_free;

    list->list_array[next_free].prev = list->list_array[real_index].prev;
    list->list_array[real_index].prev = next_free;

    list->count_of_elem++;
    if (real_index == list->tail) list->tail = next_free;
    return verify_list(list);
}

errors_list add(linked_list* list, int value) {
    assert(list);

    ssize_t next_free = find_free_index(list);

    list->list_array[next_free].value = value;

    list->list_array[next_free].prev = list->head;
    if (list->head != 0) list->list_array[list->head].next = next_free;
    list->list_array[next_free].next = 0;

    if(list->count_of_elem == 0) list->tail = next_free;
    list->head = next_free;
    list->count_of_elem++;
    return verify_list(list);
}

errors_list pop(linked_list* list, int index) {
    assert(list);

    if (index >= list->count_of_elem || index < 0) {
        list_dump(list, EL_NOT_CORRECT_POP_INDEX);
        return EL_NOT_CORRECT_POP_INDEX;
    }

    ssize_t real_index = find_index(list, index);
    ssize_t prev = list->list_array[real_index].prev;
    ssize_t next = list->list_array[real_index].next;

    if (real_index == list->tail) list->tail = next;
    if (real_index == list->head) list->head = prev;

    if (prev != 0) list->list_array[prev].next = next;
    if (next != 0) list->list_array[next].prev = prev;

    list->list_array[real_index].value = POISON_VALUE;
    list->list_array[real_index].prev  = 0;
    list->list_array[real_index].next  = list->first_free;

    list->first_free = real_index;
    list->count_of_elem--;
    return verify_list(list);
}

ssize_t find_index(linked_list* list, ssize_t index) {
    assert(list);

    ssize_t start_index = list->tail;
    for (ssize_t iterator = 0; iterator < index; iterator++) {
        start_index = list->list_array[start_index].next;
    }
    return start_index;
}

ssize_t find_free_index (linked_list* list) {
    assert(list);

    ssize_t free_index = list->first_free;
    list->first_free = list->list_array[list->first_free].next;

    if (list->first_free == 0) {
        resize_array(list);
    }
    return free_index;
}

errors_list resize_array(linked_list* list) {
    assert(list);

    list_elem* temp_array = (list_elem*)realloc(list->list_array, (list->size * 2) * sizeof(list_elem));
    if (temp_array == NULL) {
        list_dump(list, EL_LIST_ARRAY_CREATE_FAILED);
        return EL_LIST_ARRAY_CREATE_FAILED;
    }

    list->list_array = temp_array;
    for (size_t index = list->size; index < list->size * 2; index++) {
        list->list_array[index].next = index + 1;
        list->list_array[index].value = POISON_VALUE;
        list->list_array[index].prev = index - 1;
    }
    list->list_array[list->size * 2 - 1].next = 0;

    list->first_free = list->size;
    list->size = list->size * 2;
    return verify_list(list);
}


char* make_html_list_node (Agraph_t* graph, list_elem* elem, size_t index) {
    assert(elem);
    assert(graph);

    char temp_buffer[SIZE_OF_BUFFER] = "";
    sprintf_s(temp_buffer, SIZE_OF_BUFFER,
        "<TABLE BORDER= \"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
        "<TR><TD COLSPAN= \"2\">index: %lld</TD></TR>"
        "<TR><TD COLSPAN= \"2\">value: %d</TD></TR>"
        "<TR><TD>prev: %lld</TD><TD>next: %lld</TD></TR>"
      "</TABLE>", index, elem->value, elem->prev, elem->next);

    return agstrdup_html(graph, temp_buffer);
}

void print_picture (linked_list* list) {
    assert(list);

    Agraph_t* graph = agopen("test_graph", Agdirected, NULL);
    Agnode_t** nodes = (Agnode_t**)calloc(list->size, sizeof(Agnode_t*));
    make_nodes(graph, nodes, list);
    make_edges(graph, nodes, list);

    agsafeset(graph, "rankdir", "LR", "");

    FILE* file = fopen("graph.dot", "w");
    agwrite(graph, file);
    fclose(file);

    write_in_html();

    static int counter = 1;
    counter++;
    agclose(graph);
}

void write_in_html() {
    system("dot -Tsvg graph.dot -o graph.svg");
    FILE* stream = get_output_stream();
    FILE* svg = fopen("graph.svg", "r");

    size_t size_of_file = get_size_of_file("graph.svg");
    char* temp_buffer = (char*)calloc(size_of_file, sizeof(char));
    size_t real_count = fread(temp_buffer, 1, size_of_file, svg);
    fwrite(temp_buffer, 1, real_count, stream);
    fclose(svg);
    free(temp_buffer);
}

void make_nodes(Agraph_t* graph, Agnode_t** nodes, linked_list* list) {
    assert(graph);
    assert(nodes);

    for(size_t index = 0 ; index < list->size; index++) {
        char name[32];
        snprintf(name, sizeof(name), "index_%lld", index);
        Agnode_t* node = agnode(graph, name, 1);

        agsafeset(node, "shape", "plaintext", "");
        agsafeset(node, "label", make_html_list_node(graph, &list->list_array[index], index), "");
        nodes[index] = node;
    }
}

void make_edges (Agraph_t* graph, Agnode_t** nodes, linked_list* list) {
    assert(graph);
    assert(nodes);

    for(size_t index = 0; index < list->size; index++) {
        ssize_t next = list->list_array[index].next;
        ssize_t prev = list->list_array[index].prev;

        Agedge_t* edge_next = agedge(graph, nodes[index], nodes[next], NULL, 1);
        agsafeset(edge_next, "color", COLOR_FOR_NEXT, "");
        agsafeset(edge_next, "constraint", "false", "");
        Agedge_t* edge_prev = agedge(graph, nodes[index], nodes[prev], NULL, 1);
        agsafeset(edge_prev, "color", COLOR_FOR_PREV, "");
        agsafeset(edge_prev, "constraint", "false", "");
    }
    for(size_t index = 0; index < list->size - 1; index++) {
        Agedge_t* edge = agedge(graph, nodes[index], nodes[index + 1], NULL, 1);
        agsafeset(edge, "style", "invis", "");
    }
}

FILE* get_output_stream () {
    if (log_file != NULL) {
        return log_file;
    }
    return stdout;
}

int get_size_of_file(const char* name_file) {
    struct stat inf_file = {};
    stat(name_file, &inf_file);
    return inf_file.st_size;
}


