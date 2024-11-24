#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#define fileno _fileno
#define read _read
#else
#include <unistd.h>
#endif

#define DEFAULT_ALLOC_SIZE 4 // Размер выделения памяти под строку по умолчанию
#define DEFAULT_BUFFER 5 // Размер буфера для чтения файла по умолчанию

// Элемент двусвязного списка
struct TwoWayNode {
    wchar_t symbol;
    struct TwoWayNode* prev;
    struct TwoWayNode* next;
};

// typedef позволяет не набирать [struct] TwoWayNode

// Элемент двусвязного списка
typedef struct TwoWayNode TwoWayNode;

// Создать новый элемент двусвязного списка с указателями
TwoWayNode* LinkedTwoWayList_newNode(char symbol, TwoWayNode* previous,
                                     TwoWayNode* next) {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->symbol = symbol;
    node->prev = previous;
    node->next = next;
    return node;
}
// Создать новый элемент двусвязного списка
TwoWayNode* LinkedTwoWayList_newEmpty() {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->symbol = '\0';
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// Найти начало двусвязного списка
TwoWayNode* LinkedTwoWayList_start(TwoWayNode* node) {
    if (node == NULL)
        return NULL;
    while (node->prev != NULL)
        node = node->prev;
    return node;
}
// Вставить элемент после указанного (двусвязный список)
void LinkedTwoWayList_insert(TwoWayNode* previous, TwoWayNode* node) {
    if (previous == NULL || node == NULL)
        return;
    TwoWayNode* next = previous->next;

    previous->next = node;
    node->prev = previous;

    next->prev = node;
    node->next = next;
}
// Вставить символ после указанного (двусвязный список)
// Возвращает новый элемент
TwoWayNode* LinkedTwoWayList_insertChar(TwoWayNode* previous, wchar_t symbol) {
    TwoWayNode* node = LinkedTwoWayList_newNode(
        symbol, previous, previous == NULL ? NULL : previous->next);
    if (previous != NULL) {
        node = LinkedTwoWayList_newNode(symbol, previous, previous->next);
        previous->next = node;
        if (node->next != NULL)
            node->next->prev = node;
    } else {
        node = LinkedTwoWayList_newEmpty();
        node->symbol = symbol;
    }
    return node;
}
// Удалить следующий элемент (двусвязный список)
// Возвращает новый следующий элемент
TwoWayNode* LinkedTwoWayList_removeNext(TwoWayNode* previous) {
    if (previous == NULL || previous->next == NULL)
        return NULL;
    TwoWayNode* next = previous->next->next;
    free(previous->next);
    previous->next = next;
    if (next != NULL)
        next->prev = previous;
    return next;
}
// Удалить предыдущий элемент (двусвязный список)
// Возвращает новый предыдущий элемент
TwoWayNode* LinkedTwoWayList_removePrevious(TwoWayNode* next) {
    if (next == NULL || next->prev == NULL)
        return NULL;
    TwoWayNode* previous = next->prev->prev;
    free(next->prev);
    if (previous != NULL)
        previous->next = next;
    next->prev = previous;
    return previous;
}
// Удалить данный элемент (двусвязный список)
// Возвращает следующий элемент
TwoWayNode* LinkedTwoWayList_remove(TwoWayNode* node) {
    TwoWayNode* next = NULL;
    if (node == NULL)
        return NULL;
    if (node->prev != NULL)
        node->prev->next = node->next;
    if (node->next != NULL) {
        next = node->next;
        node->next->prev = node->prev;
    }
    free(node);
    return next;
}

// Прочитать файл в двусвязный список, используя буфер
TwoWayNode* LinkedTwoWayList_fromFile(FILE* file, const size_t buffer_size,
                                      const wchar_t stop) {
    TwoWayNode* start = LinkedTwoWayList_newEmpty();
    TwoWayNode* end = start;
    TwoWayNode* second;
    int file_desc = fileno(file);
    wchar_t* buffer = (char*)malloc(sizeof(char) * buffer_size);
    int found_stop = 0;
    int read_size;

    while ((read_size = read(file_desc, buffer, buffer_size)) > 0) {
        for (size_t i = 0; i < read_size; i++) {
            if (buffer[i] == stop) {
                found_stop = 1;
                break;
            }
            end = LinkedTwoWayList_insertChar(end, buffer[i]);
        }
        if (found_stop)
            break;
    }

    free(buffer);

    if (start->next == NULL)
        return start; // Строка нулевой длины, ничего не прочитано
    // Строка не нулевой длины, первый элемент пустой, строка начинается с
    // следующего элемента
    second = start->next;
    second->prev = NULL;
    free(start);
    return second;
}

// Преобразование двусвязного списка в строку
// На вход передаётся начало списка
char* LinkedTwoWayList_toString(TwoWayNode* node) {
    size_t alloc_size = DEFAULT_ALLOC_SIZE;
    size_t str_size = 0;
    wchar_t* str = (char*)malloc(sizeof(char) * alloc_size);
    while (node != NULL) {
        str[str_size++] = node->symbol;
        if (str_size >= alloc_size) {
            alloc_size *= 2;
            str = (char*)realloc(str, alloc_size);
        }
        node = node->next;
    }
    str[str_size++] = '\0';
    return (char*)realloc(str, str_size);
}
// Преобразование строки в двусвязный список
TwoWayNode* LinkedTwoWayList_fromString(char* str) {
    TwoWayNode* start = LinkedTwoWayList_newEmpty();
    TwoWayNode* node = start;
    for (char* c = str; *c != '\0'; c++)
        node = LinkedTwoWayList_insertChar(node, *c);
    if (start->next == NULL)
        return NULL;
    TwoWayNode* second = start->next;
    free(start);
    return second;
}
