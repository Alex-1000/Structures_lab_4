#include "linked_list.h"
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <conio.h>
#define print _cputws
#define printf _cprintf_s
#define get _getwch
#define clear _clrscr
#else
#define _XOPEN_SOURCE 700
#include <ncurses.h>
#define print addwstr
#define printf printw
wchar_t get() {
    wchar_t ch;
    if (get_wch(&ch) == KEY_CODE_YES)
        return '\0';
    return ch;
}
int gotoxy(int x, int y) { return move(y, x); }
int wherex() { return getcurx(stdscr); }
int wherey() { return getcury(stdscr); }
#define NCURSES
#endif

// Список команд
const wchar_t* HELP = L"1 - Начать работу со списком\n"
                      L"2 - Сделать список пустым\n"
                      L"3 - Показать длину списка\n"
                      L"4 - Установить указатель в начало списка\n"
                      L"5 - Находится ли указатель в начале/конце списка?\n"
                      L"6 - Передвинуть указатель вправо\n"
                      L"7 - Показать значение элемента за указателем\n"
                      L"8 - Удалить элемент списка за указателем\n"
                      L"9 - Взять в буффер элемент списка за указателем\n"
                      L"A - Изменить значение элемента за указателем\n"
                      L"B - Добавить элемент за указателем\n"
                      L"C - Распечатать список\n"
                      L"D - Закончить работу со списком\n"
                      L"E - Закончить работу программы\n";

// Команда повтора списка
const wchar_t* HELP_REPEAT = L"F - Вывести список команд\n";

const wchar_t* WARNING_SYMBOL = L"/!\\ ";

// Сообщение о том, что указатель находится в конце
const wchar_t* PTR_IN_END = L"Указатель находится в конце списка\n";

// Сообщение о том, что список не инициализирован
const wchar_t* NOT_INITIALIZED = L"Работа со списком не начата\n";
const size_t NOT_INITIALIZED_size = 29 * sizeof(wchar_t);

const wchar_t* INVALID_PTR = L"Указатель находится за пределами строки\n";
const size_t INVALID_PTR_size = 41 * sizeof(wchar_t);

wchar_t* indicator(Node* start, Node* node) {
    wchar_t* str;
    if (start == NULL) {
        str = (wchar_t*)malloc(NOT_INITIALIZED_size);
        memcpy(str, NOT_INITIALIZED, NOT_INITIALIZED_size);
        return str;
    }
    size_t dist = LinkedList_dist(start, node);
    size_t len = LinkedList_length(start);
    if (dist == SIZE_MAX || len < dist) {
        str = (wchar_t*)malloc(INVALID_PTR_size);
        memcpy(str, INVALID_PTR, INVALID_PTR_size);
    } else {
        str = (wchar_t*)malloc(sizeof(wchar_t) * (dist + 3));
        for (size_t i = 0; i < dist; i++)
            str[i] = ' ';
        str[dist] = '^';
        str[dist + 1] = '\n';
        str[dist + 2] = '\0';
    }
    return str;
}

void update_terminal(Node* start, Node* ptr, wchar_t buffer) {
    wchar_t* str = LinkedList_toString(start);
    printf("%ls\n", str);
    free(str);
    str = indicator(start, ptr);
    print(str);
    free(str);
    if (buffer == '\0')
        gotoxy(wherex(), wherey() + 1);
    else
        printf("%ls%c\n", L"Буффер: ", buffer);
}

void reset() {
    clear();
    gotoxy(0, 0);
}

int main() {
    wchar_t ch;
    Node* start = LinkedList_newEmpty();
    Node* ptr = start;
    wchar_t buffer = '\0';
    int stop = 0;
    int show_help = 1;

    setlocale(LC_ALL, "ru_RU.UTF-8");

#ifdef NCURSES
    initscr();
    noecho();
#endif

    while (!stop) {
        print(L"Выберите команду...\n");
        if (show_help) {
            print(HELP);
            show_help = 0;
        }
        print(HELP_REPEAT);
#ifdef NCURSES
        refresh();
#endif
        ch = towupper(get());
        switch (ch) {
        case '1':
            if (start != NULL)
                LinkedList_empty(start);
            else
                start = LinkedList_newEmpty();
            ptr = start;
            buffer = '\0';
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case '2':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            LinkedList_empty(start);
            ptr = start;
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case '3':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            printf("%ls%zu\n", L"Длина списка: ", LinkedList_length(start));
            update_terminal(start, ptr, buffer);
            break;
        case '4':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            ptr = start;
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case '5':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr == start)
                print(L"Указатель в начале списка\n");
            else if (ptr->next == NULL)
                print(PTR_IN_END);
            else
                print(L"Указатель в середине списка\n");
            update_terminal(start, ptr, buffer);
            break;
        case '6':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->next == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_END);
            } else {
                ptr = ptr->next;
            }
            update_terminal(start, ptr, buffer);
            break;
        case '7':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->next == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_END);
            } else {
                printf("%ls%c\n", L"Значение за указателем: ", ptr->symbol);
            }
            update_terminal(start, ptr, buffer);
            break;
        case '8':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->next == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_END);
            } else {
                ptr = LinkedList_removeNext(ptr);
            }
            update_terminal(start, ptr, buffer);
            break;
        case '9':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->next == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_END);
            } else {
                buffer = ptr->next->symbol;
                ptr = LinkedList_removeNext(ptr);
            }
            update_terminal(start, ptr, buffer);
            break;
        case 'A':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            print(L"Введите новый символ:\n");
            update_terminal(start, ptr, buffer);
            ch = get();
            ptr->symbol = ch;
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case 'B':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            print(L"Введите новый символ:\n");
            update_terminal(start, ptr, buffer);
            ch = get();
            if (start->symbol == '\0')
                start->symbol = ch;
            else
                ptr = LinkedList_insertChar(ptr, ch);
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case 'C':
            if (start == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            update_terminal(start, ptr, buffer);
            break;
        case 'D':
            reset();
            LinkedList_empty(start);
            free(start);
            start = NULL;
            ptr = NULL;
            buffer = '\0';
            print(L"Работа со списком завершена\n");
            break;
        case 'Q':
        case 'E':
            reset();
            update_terminal(start, ptr, buffer);
            stop = 1;
            if (start != NULL) {
                LinkedList_empty(start);
                free(start);
            }
            break;
        case 'F':
            reset();
            show_help = 1;
            update_terminal(start, ptr, buffer);
            break;
        default:
            reset();
            print(L"Команда не распознана\n");
            update_terminal(start, ptr, buffer);
        }
    }

#ifdef NCURSES
    endwin();
#endif

    return 0;
}
