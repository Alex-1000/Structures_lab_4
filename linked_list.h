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

struct Node {
    char symbol;
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
Node* LinkedList_insertChar(Node* previous, char symbol) {
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
// Удалить все элементы после данного
Node* LinkedList_empty(Node* node) {
    Node* next;
    for (Node* n = node->next; node != NULL; n = next) {
        next = n->next;
        free(n);
    }
    return node;
}

// Найти последний элемент списка
Node* LinkedList_lastNode(Node* node) {
    Node* end;
    for (end = node; end->next != NULL; end = end->next)
        ;
    return end;
}

// Прочитать файл в односвязный список, используя буфер
Node* LinkedList_fromFile(FILE* file, const size_t buffer_size,
                          const char stop) {
    Node* start = LinkedList_newEmpty();
    Node* end = start;
    int file_desc = fileno(file);
    char* buffer = (char*)malloc(sizeof(char) * buffer_size);
    int found_stop = 0;
    int read_size;

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
char* LinkedList_toString(Node* node) {
    size_t alloc_size = DEFAULT_ALLOC_SIZE;
    size_t str_size = 0;
    char* str = (char*)malloc(sizeof(char) * alloc_size);
    for (; node != NULL; node = node->next) {
        str[str_size++] = node->symbol;
        if (str_size >= alloc_size) {
            alloc_size *= 2;
            str = (char*)realloc(str, alloc_size);
        }
    }
    str[str_size++] = '\0';
    return (char*)realloc(str, str_size);
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
