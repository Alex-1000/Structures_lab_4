#include "linked_two_way_list.h"
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

enum {
    INIT = 'O',
    EMPTY = 'G',
    LENGTH = 'L',
    START = 'Z',
    END = 'C',
    AT_BORDER = 'X',
    LEFT = 'A',
    RIGHT = 'D',
    LEFT_VALUE = 'S',
    RIGHT_VALUE = 'F',
    LEFT_DEL = 'W',
    RIGHT_DEL = 'E',
    LEFT_CUT = 'R',
    RIGHT_CUT = 'T',
    LEFT_EDIT = '1',
    RIGHT_EDIT = '2',
    LEFT_ADD = '3',
    RIGHT_ADD = '4',
    PRINT = 'P',
    RESET = 'K',
    EXIT = 'Q',
    PRINT_HELP = 'H'
};

// Список команд
const wchar_t* HELP = L"O - Начать работу со списком\n"
                      L"G - Сделать список пустым\n"
                      L"L - Показать длину списка\n"
                      L"Z - Установить указатель в начало списка\n"
                      L"C - Установить указатель в конец списка\n"
                      L"X - Находится ли указатель в начале/конце списка?\n"
                      L"A - Передвинуть указатель влево\n"
                      L"D - Передвинуть указатель вправо\n"
                      L"S - Показать значение элемента перед указателем\n"
                      L"F - Показать значение элемента за указателем\n"
                      L"W - Удалить элемент списка перед указателем\n"
                      L"E - Удалить элемент списка за указателем\n"
                      L"R - Взять в буффер элемент списка перед указателем\n"
                      L"T - Взять в буффер элемент списка за указателем\n"
                      L"1 - Изменить значение элемента перед указателем\n"
                      L"2 - Изменить значение элемента за указателем\n"
                      L"3 - Добавить элемент перед указателем\n"
                      L"4 - Добавить элемент за указателем\n"
                      L"P - Распечатать список\n"
                      L"K - Закончить работу со списком\n"
                      L"Q - Закончить работу программы\n";

// Команда повтора списка
const wchar_t* HELP_REPEAT = L"H - Вывести список команд\n";

const wchar_t* WARNING_SYMBOL = L"/!\\ ";

const wchar_t* PTR_IN_START = L"Указатель находится в начале списка\n";

const wchar_t* PTR_IN_END = L"Указатель находится в конце списка\n";

// Сообщение о том, что список не инициализирован
const wchar_t* NOT_INITIALIZED = L"Работа со списком не начата\n";
const size_t NOT_INITIALIZED_size = 29 * sizeof(wchar_t);

const wchar_t* INVALID_PTR = L"Указатель находится за пределами строки\n";
const size_t INVALID_PTR_size = 41 * sizeof(wchar_t);

wchar_t* indicator(TwoWayNode* start, TwoWayNode* TwoWayNode) {
    wchar_t* str;
    if (start == NULL) {
        str = (wchar_t*)malloc(NOT_INITIALIZED_size);
        memcpy(str, NOT_INITIALIZED, NOT_INITIALIZED_size);
        return str;
    }
    size_t dist = LinkedTwoWayList_dist(start, TwoWayNode);
    size_t len = LinkedTwoWayList_length(start);
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

void update_terminal(TwoWayNode* ptr, wchar_t buffer) {
    TwoWayNode* start = LinkedTwoWayList_start(ptr);
    wchar_t* str = LinkedTwoWayList_toString(start);
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
    TwoWayNode* ptr = LinkedTwoWayList_newEmpty();
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
        case INIT:
            if (ptr != NULL)
                LinkedTwoWayList_empty(ptr);
            else
                ptr = LinkedTwoWayList_newEmpty();
            buffer = '\0';
            reset();
            update_terminal(ptr, buffer);
            break;
        case EMPTY:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            LinkedTwoWayList_empty(ptr);
            reset();
            update_terminal(ptr, buffer);
            break;
        case LENGTH:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            printf("%ls%zu\n", L"Длина списка: ", LinkedTwoWayList_length(ptr));
            update_terminal(ptr, buffer);
            break;
        case START:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            ptr = LinkedTwoWayList_start(ptr);
            reset();
            update_terminal(ptr, buffer);
            break;
        case END:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            ptr = LinkedTwoWayList_end(ptr);
            reset();
            update_terminal(ptr, buffer);
            break;
        case AT_BORDER:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL)
                print(PTR_IN_START);
            else if (ptr->next == NULL)
                print(PTR_IN_END);
            else
                print(L"Указатель в середине списка\n");
            update_terminal(ptr, buffer);
            break;
        case LEFT:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_START);
            } else {
                ptr = ptr->prev;
            }
            update_terminal(ptr, buffer);
            break;
        case RIGHT:
            if (ptr == NULL) {
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
            update_terminal(ptr, buffer);
            break;
        case LEFT_VALUE:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_START);
            } else {
                printf("%ls%c\n", L"Значение перед указателем: ",
                       ptr->prev->symbol);
            }
            update_terminal(ptr, buffer);
            break;
        case RIGHT_VALUE:
            if (ptr == NULL) {
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
                printf("%ls%c\n", L"Значение за указателем: ",
                       ptr->next->symbol);
            }
            update_terminal(ptr, buffer);
            break;
        case LEFT_DEL:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_START);
            } else {
                ptr = LinkedTwoWayList_removePrevious(ptr);
            }
            update_terminal(ptr, buffer);
            break;
        case RIGHT_DEL:
            if (ptr == NULL) {
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
                ptr = LinkedTwoWayList_removeNext(ptr);
            }
            update_terminal(ptr, buffer);
            break;
        case LEFT_CUT:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_START);
            } else {
                buffer = ptr->prev->symbol;
                LinkedTwoWayList_removePrevious(ptr);
            }
            update_terminal(ptr, buffer);
            break;
        case RIGHT_CUT:
            if (ptr == NULL) {
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
                LinkedTwoWayList_removeNext(ptr);
            }
            update_terminal(ptr, buffer);
            break;
        case LEFT_EDIT:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            if (ptr->prev == NULL) {
                print(WARNING_SYMBOL);
                print(PTR_IN_START);
            } else {
                print(L"Введите новый символ:\n");
                update_terminal(ptr, buffer);
                ch = get();
                ptr->prev->symbol = ch;
                reset();
            }
            update_terminal(ptr, buffer);
            break;
        case RIGHT_EDIT:
            if (ptr == NULL) {
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
                print(L"Введите новый символ:\n");
                update_terminal(ptr, buffer);
                ch = get();
                ptr->next->symbol = ch;
                reset();
            }
            update_terminal(ptr, buffer);
            break;
        case LEFT_ADD:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            print(L"Введите новый символ:\n");
            update_terminal(ptr, buffer);
            ch = get();
            if (ptr->symbol == '\0')
                ptr->symbol = ch;
            else
                ptr = LinkedTwoWayList_insertChar(ptr, ch);
            reset();
            update_terminal(ptr, buffer);
            break;
        case RIGHT_ADD:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            print(L"Введите новый символ:\n");
            update_terminal(ptr, buffer);
            ch = get();
            if (ptr->symbol == '\0')
                ptr->symbol = ch;
            else
                ptr = LinkedTwoWayList_appendChar(ptr, ch);
            reset();
            update_terminal(ptr, buffer);
            break;
        case PRINT:
            if (ptr == NULL) {
                reset();
                print(WARNING_SYMBOL);
                print(NOT_INITIALIZED);
                break;
            }
            reset();
            update_terminal(ptr, buffer);
            break;
        case RESET:
            reset();
            LinkedTwoWayList_empty(ptr);
            free(ptr);
            ptr = NULL;
            print(L"Работа со списком завершена\n");
            break;
        case EXIT:
            reset();
            update_terminal(ptr, buffer);
            stop = 1;
            if (ptr != NULL) {
                LinkedTwoWayList_empty(ptr);
                free(ptr);
            }
            break;
        case PRINT_HELP:
            reset();
            show_help = 1;
            update_terminal(ptr, buffer);
            break;
        default:
            reset();
            print(L"Команда не распознана\n");
            update_terminal(ptr, buffer);
        }
    }

#ifdef NCURSES
    endwin();
#endif

    return 0;
}
