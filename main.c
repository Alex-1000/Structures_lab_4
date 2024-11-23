#include <ctype.h>
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

// Элемент односвязного списка
struct Node {
    char symbol;
    struct Node* next;
};

// Элемент двусвязного списка
struct TwoWayNode {
    char symbol;
    struct TwoWayNode* prev;
    struct TwoWayNode* next;
};

// typedef позволяет не набирать [struct] Node/TwoWayNode
// Элемент односвязного списка
typedef struct Node Node;
// Элемент двусвязного списка
typedef struct TwoWayNode TwoWayNode;

// Создать новый элемент с указателем и значением
Node* newNode(char symbol, Node* next) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->symbol = symbol;
    node->next = next;
    return node;
}
// Создать новый элемент
Node* newEmptyNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->next = NULL;
    return node;
}

// Вставить элемент после указанного
void insertToList(Node* previous, Node* node) {
    if (previous == NULL)
        return;
    Node* next = previous->next;
    previous->next = node;
    node->next = next;
}
// Вставить символ после указанного
// Возвращает новый элемент
Node* insertCharToList(Node* previous, char symbol) {
    Node* node = newNode(symbol, NULL);
    insertToList(previous, node);
    return node;
}
// Удалить следующий элемент
// Возвращает новый следующий элемент
Node* removeNextFromList(Node* previous) {
    if (previous == NULL)
        return NULL;
    Node* next = previous->next->next;
    free(previous->next);
    previous->next = next;
    return next;
}
// Удалить данный элемент
// Возвращает следующий элемент
Node* removeFromList(Node* node) {
    Node* next = node->next;
    free(node);
    return next;
}

// Создать новый элемент двусвязного списка с указателями
TwoWayNode* newTwoWayNode(char symbol, TwoWayNode* previous, TwoWayNode* next) {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->symbol = symbol;
    node->prev = previous;
    node->next = next;
    return node;
}
// Создать новый элемент двусвязного списка
TwoWayNode* newEmptyTwoWayNode() {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->symbol = '\0';
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// Найти начало двусвязного списка
TwoWayNode* findTwoWayListStart(TwoWayNode* node) {
    if (node == NULL)
        return NULL;
    while (node->prev != NULL)
        node = node->prev;
    return node;
}
// Вставить элемент после указанного (двусвязный список)
void insertToTwoWayList(TwoWayNode* previous, TwoWayNode* node) {
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
TwoWayNode* insertCharToTwoWayList(TwoWayNode* previous, char symbol) {
    TwoWayNode* node = newTwoWayNode(symbol, previous,
                                     previous == NULL ? NULL : previous->next);
    if (previous != NULL) {
        node = newTwoWayNode(symbol, previous, previous->next);
        previous->next = node;
        if (node->next != NULL)
            node->next->prev = node;
    } else {
        node = newEmptyTwoWayNode();
        node->symbol = symbol;
    }
    return node;
}
// Удалить следующий элемент (двусвязный список)
// Возвращает новый следующий элемент
TwoWayNode* removeNextFromTwoWayList(TwoWayNode* previous) {
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
TwoWayNode* removePreviousFromTwoWayList(TwoWayNode* next) {
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
TwoWayNode* removeFromTwoWayList(TwoWayNode* node) {
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

// Прочитать файл в односвязный список, используя буфер
Node* readToList(FILE* file, const size_t buffer_size, const char stop) {
    Node* start = newEmptyNode();
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
            end = insertCharToList(end, buffer[i]);
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

// Прочитать файл в двусвязный список, используя буфер
TwoWayNode* readToTwoWayList(FILE* file, const size_t buffer_size,
                             const char stop) {
    TwoWayNode* start = newEmptyTwoWayNode();
    TwoWayNode* end = start;
    TwoWayNode* second;
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
            end = insertCharToTwoWayList(end, buffer[i]);
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

// Удалить из строки (односвязного списка) цифры
// Принимает на вход указатель на старт
// Возвращает указатель на начало списка
Node* removeDigitsFromList(Node* start) {
    if (start == NULL)
        return NULL;
    while (start != NULL && isdigit(start->symbol))
        start = removeFromList(start);
    for (Node* node = start; node != NULL && node->next != NULL;
         node = node->next) {
        while (node->next != NULL && isdigit(node->next->symbol)) {
            removeNextFromList(node);
        }
    }
    return start;
}
// Удалить из строки (двусвязного списка) цифры
// Принимает на вход указатель на старт
// Возвращает указатель на начало списка
TwoWayNode* removeDigitsFromTwoWayList(TwoWayNode* start) {
    if (start == NULL)
        return NULL;
    while (start != NULL && isdigit(start->symbol))
        start = removeFromTwoWayList(start);
    for (TwoWayNode* node = start; node != NULL && node->next != NULL;
         node = node->next) {
        while (node->next != NULL && isdigit(node->next->symbol)) {
            removeNextFromTwoWayList(node);
        }
    }
    return start;
}

// Удвоить в строке (односвязном списке) символы +/-
// Принимает на вход указатель на старт
// Возвращает указатель на начало списка
Node* doubleSignsInList(Node* start) {
    for (Node* node = start; node != NULL; node = node->next) {
        if (node->symbol == '+' || node->symbol == '-') {
            node = insertCharToList(node, node->symbol);
        }
    }
    return start;
}
// Удвоить в строке (двусвязном списке) символы +/-
// Принимает на вход указатель на старт
// Возвращает переданный изначально указатель
TwoWayNode* doubleSignsInTwoWayList(TwoWayNode* start) {
    for (TwoWayNode* node = start; node != NULL; node = node->next) {
        if (node->symbol == '+' || node->symbol == '-') {
            node = insertCharToTwoWayList(node, node->symbol);
        }
    }
    return start;
}

// Преобразование односвязного списка в строку
char* listToChar(Node* node) {
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
    return realloc(str, str_size);
}
// Преобразование строки в односвязный список
Node* charToList(char* str) {
    Node* start = newEmptyNode();
    Node* node = start;
    for (char* c = str; *c != '\0'; c++)
        node = insertCharToList(node, *c);
    if (start->next == NULL)
        return NULL;
    Node* second = start->next;
    free(start);
    return second;
}

// Преобразование двусвязного списка в строку
// На вход передаётся начало списка
char* twoWayListToChar(TwoWayNode* node) {
    size_t alloc_size = DEFAULT_ALLOC_SIZE;
    size_t str_size = 0;
    char* str = (char*)malloc(sizeof(char) * alloc_size);
    while (node != NULL) {
        str[str_size++] = node->symbol;
        if (str_size >= alloc_size) {
            alloc_size *= 2;
            str = (char*)realloc(str, alloc_size);
        }
        node = node->next;
    }
    str[str_size++] = '\0';
    return realloc(str, str_size);
}
// Преобразование строки в двусвязный список
TwoWayNode* charToTwoWayList(char* str) {
    TwoWayNode* start = newEmptyTwoWayNode();
    TwoWayNode* node = start;
    for (char* c = str; *c != '\0'; c++)
        node = insertCharToTwoWayList(node, *c);
    if (start->next == NULL)
        return NULL;
    TwoWayNode* second = start->next;
    free(start);
    return second;
}

int main(int argc, char* argv[]) {
    Node* one_way_list;
    TwoWayNode* two_way_list;

    // Флаг, определяющий, передана ли строка как аргумент
    int cli = argc > 1;

    if (cli) {
        one_way_list = charToList(argv[1]);
        two_way_list = charToTwoWayList(argv[1]);
    } else {
        printf("(1/2) Введите строку:\n");
        one_way_list = readToList(stdin, DEFAULT_BUFFER, '\n');
        printf("(2/2) Введите строку:\n");
        two_way_list = readToTwoWayList(stdin, DEFAULT_BUFFER, '\n');
    }

    one_way_list = removeDigitsFromList(doubleSignsInList(one_way_list));
    char* new_str = listToChar(one_way_list);
    if (cli)
        printf("%s\n", new_str);
    else
        printf("Новая строка (односвязный список):\n%s\n", new_str);
    free(one_way_list);
    free(new_str);

    two_way_list =
        removeDigitsFromTwoWayList(doubleSignsInTwoWayList(two_way_list));
    new_str = twoWayListToChar(two_way_list);
    if (cli)
        printf("%s\n", new_str);
    else
        printf("Новая строка (двусвязный список):\n%s\n", new_str);
    free(two_way_list);
    free(new_str);

    return 0;
}
