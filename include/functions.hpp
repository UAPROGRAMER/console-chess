#pragma once

#include <ncurses.h>

template<class T>
inline constexpr T max(T a, T b) { return (a > b) ? a : b; }

template<class T>
inline constexpr T min(T a, T b) { return (a < b) ? a : b; }

template<class T>
inline constexpr T abs(T a) { return (a >= 0) ? a : -a; }

inline constexpr bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline constexpr bool is_lower(char c) {
    return (c >= 'a' && c <= 'z');
}

inline void mvprintstr(int y, int x,
    const char* str, size_t size, chtype atr) {
    for (size_t i = 0; i < size; i++)
        mvaddch(y, x + i, str[i] | atr);
}