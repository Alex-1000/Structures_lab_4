#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#define fileno _fileno
#define read _read
#else
#include <unistd.h>
#endif

#define DEFAULT_ALLOC_SIZE 4 // Размер выделения памяти под строку по умолчанию
#define DEFAULT_BUFFER 5 // Размер буфера для чтения файла по умолчанию

struct Node {
    wchar_t symbol;
    struct Node* next;
};

// typedef позволяет не набирать [struct] Node

// Элемент односвязного списка
typedef struct Node Node;

// Создать новый элемент с указателем и значением
Node* LinkedList_newNode(char symbol, Node* next) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->symbol = symbol;
    node->next = next;
    return node;
}
// Создать новый элемент
Node* LinkedList_newEmpty() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->symbol = '\0';
    node->next = NULL;
    return node;
}

// Вставить элемент после указанного
void LinkedList_insert(Node* previous, Node* node) {
    if (previous == NULL)
        return;
    Node* next = previous->next;
    previous->next = node;
    node->next = next;
}
// Вставить символ после указанного
// Возвращает новый элемент
Node* LinkedList_insertChar(Node* previous, wchar_t symbol) {
    Node* node = LinkedList_newNode(symbol, NULL);
    LinkedList_insert(previous, node);
    return node;
}

// Удалить следующий элемент
// Возвращает новый следующий элемент
Node* LinkedList_removeNext(Node* previous) {
    if (previous == NULL)
        return NULL;
    Node* next = previous->next->next;
    free(previous->next);
    previous->next = next;
    return next;
}
// Удалить данный элемент
// Возвращает следующий элемент
Node* LinkedList_remove(Node* node) {
    Node* next = node->next;
    free(node);
    return next;
}
// Удалить все элементы после данного и отчистить данный элемент
Node* LinkedList_empty(Node* node) {
    Node* next;
    if (node == NULL)
        return NULL;
    if (node->next != NULL) {
        for (Node* n = node->next; next != NULL; n = next) {
            next = n->next;
            free(n);
        }
    }
    node->next = NULL;
    node->symbol = '\0';
    return node;
}

// Количество элементов списка
size_t LinkedList_length(Node* node) {
    size_t len = 0;
    if (node->symbol == '\0')
        return 0;
    for (Node* end = node; end != NULL; end = end->next)
        len++;
    return len;
}
// Кол-во элементов между двумя элементами
// Возвращает 0, если start == end
// Возвращает SIZE_MAX, если end не находится в списке справа от start
size_t LinkedList_dist(Node* start, Node* end) {
    size_t len = 0;
    Node* node;
    for (node = start; node != end && node != NULL; node = node->next)
        len++;
    if (node == NULL)
        return SIZE_MAX;
    return len;
}

// Прочитать файл в односвязный список, используя буфер
Node* LinkedList_fromFile(FILE* file, const size_t buffer_size,
                          const wchar_t stop) {
    Node* start = LinkedList_newEmpty();
    Node* end = start;
    int file_desc = fileno(file);
    wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * buffer_size);
    int found_stop = 0;
    size_t read_size;

    while ((read_size = read(file_desc, buffer, buffer_size)) > 0) {
        for (size_t i = 0; i < read_size; i++) {
            if (buffer[i] == stop) {
                found_stop = 1;
                break;
            }
            end = LinkedList_insertChar(end, buffer[i]);
        }
        if (found_stop)
            break;
    }

    free(buffer);

    if (start->next == NULL)
        return start; // Строка нулевой длины, ничего не прочитано
    // Строка не нулевой длины, первый элемент пустой, строка начинается с
    // следующего элемента
    Node* second = start->next;
    free(start);
    return second;
}

// Преобразование односвязного списка в строку
wchar_t* LinkedList_toString(Node* node) {
    size_t alloc_size = DEFAULT_ALLOC_SIZE;
    size_t str_size = 0;
    wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * alloc_size);
    for (; node != NULL; node = node->next) {
        str[str_size++] = node->symbol;
        if (str_size >= alloc_size) {
            alloc_size *= 2;
            str = (wchar_t*)realloc(str, sizeof(wchar_t) * alloc_size);
        }
    }
    str[str_size++] = '\0';
    return (wchar_t*)realloc(str, sizeof(wchar_t) * str_size);
}
// Преобразование строки в односвязный список
Node* LinkedList_fromString(char* str) {
    Node* start = LinkedList_newEmpty();
    Node* node = start;
    for (char* c = str; *c != '\0'; c++)
        node = LinkedList_insertChar(node, *c);
    if (start->next == NULL)
        return NULL;
    Node* second = start->next;
    free(start);
    return second;
}
