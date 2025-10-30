#include <colors.hpp>

#include <ncurses.h>

void setup_colors() {
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_GREEN, 0, 1000, 0);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_BLUE, 0, 0, 1000);
    init_color(COLOR_MAGENTA, 1000, 0, 1000);
    init_color(COLOR_CYAN, 0, 1000, 1000);
    init_color(COLOR_WHITE, 1000, 1000, 1000);
}

short get_pair(short fg, short bg) {
    static bool initialized_pairs[8][8] = {false};
    
    if (!initialized_pairs[fg][bg]) {
        init_pair(fg * 8 + bg + 1, fg, bg);
        initialized_pairs[fg][bg] = true;
    }

    return COLOR_PAIR(fg * 8 + bg + 1);
}
