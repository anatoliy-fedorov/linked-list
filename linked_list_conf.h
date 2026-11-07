#ifndef LIST_CONF_H
#define LIST_CONF_H


struct list_elem {
    int value;
    ssize_t next;
    ssize_t prev;
};

struct linked_list {
    ssize_t head;
    ssize_t tail;
    list_elem* list_array;
    size_t size;
    ssize_t first_free;
    ssize_t count_of_elem;
};

enum errors_list {
    EL_NO_ERRORS,
    EL_LIST_CREATE_FAILED,
    EL_LIST_ARRAY_CREATE_FAILED,
    EL_HEAD_NOT_CORRECT,
    EL_TAIL_NOT_CORRECT,
    EL_EXIST_CYCLE,
    EL_LOG_FILE_NOT_OPEN,
    EL_NOT_EXIST_FREE_ELEM,
    EL_NOT_CORRECT_SIZE,
    EL_ERROR_HEAD_OR_TAIL,
    EL_NOT_CORRECT_POP_INDEX,
    EL_ERRORS_COUNT
};

const char* el_errors_message[EL_ERRORS_COUNT] {
    "No errors",
    "Struct \"linked_list\" was not create",
    "Array \"list_array\" in struct \"linked_list\" was not create",
    "Parameter head is not correct",
    "Parameter tail is not correct",
    "In your list exist cycle",
    "Log file not open",
    "In list not exists free elems",
    "Not correct size of list",
    "Head or tail of list is not correct",
    "Pop index not correct"
};

const size_t MIN_SIZE = 10;
const int POISON_VALUE = -1;
const size_t SIZE_OF_BUFFER = 1000;
FILE* log_file = fopen("log_file.html", "w");
const char GREEN_COLOR[] = "green";
const char GREY_COLOR[] = "grey";
const char DEFAULT_COLOR[] = "white";
const char COLOR_FOR_EDGE[] = "blue";
const char COLOR_ZERO[] = "yellow";
const char COLOR_FLAGS[] = "purple";

#endif
