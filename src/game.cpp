#include <colors.hpp>
#include <game.hpp>
#include <functions.hpp>

#include <cinttypes>
#include <iostream>
#include <string>

#include <ncurses.h>

Game::Game() : cursor_y(0), cursor_x(0), is_running(false),
    move_number(1), chosen_piece_x(0), chosen_piece_y(0),
    is_chosen(false), last_charged_pawn_x(0), last_charged_pawn_move_number(0),
    is_white_castle_valid(true), is_white_long_castle_valid(true),
    is_black_castle_valid(true), is_black_long_castle_valid(true),
    game_ended(false) {
    initscr();

    if (!has_colors()) {
        endwin();
        std::cout << "Error: terminal doesnt support colors.\n";
        exit(1);
    }

    if (!can_change_color()) {
        endwin();
        std::cout << "Error: terminal doesnt support color change.\n";
        exit(1);
    }

    noecho();
    cbreak();
    curs_set(0);

    keypad(stdscr, true);
    nodelay(stdscr, false);

    start_color();
    use_default_colors();

    setup_colors();

    char start_position[] =
         "rnbqkbnr"
         "pppppppp"
         "........"
         "........"
         "........"
         "........"
         "PPPPPPPP"
         "RNBQKBNR";

    for (uint8_t y = 0; y < 8; y++)
        for (uint8_t x = 0; x < 8; x++)
            game_buffer[y][x] = start_position[y * 8 + x];
}

Game::~Game() {
    endwin();
}

bool Game::is_line_free() const {
    int8_t x0 = cursor_x;
    int8_t y0 = cursor_y;
    int8_t x1 = chosen_piece_x;
    int8_t y1 = chosen_piece_y;

    int8_t dx = abs<int8_t>(x1 - x0);
    int8_t sx = (x0 < x1) ? 1 : -1;
    int8_t dy = -abs<int8_t>(y1 - y0);
    int8_t sy = (y0 < y1) ? 1 : -1;
    int8_t err = dx + dy;

    while (true) {
        if (!(x0 == cursor_x && y0 == cursor_y) && !(x0 == x1 && y0 == y1)) {
            if (x0 < 0 || x0 > 7 || y0 < 0 || y0 > 7)
                return false;

            if (game_buffer[y0][x0] != '.')
                return false;
        }

        if (x0 == x1 && y0 == y1) break;
        int8_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }

    return true;
}

bool Game::is_move_valid() {
    if (chosen_piece_x == cursor_x && chosen_piece_y == cursor_y)
        return false;

    if (game_buffer[cursor_y][cursor_x] != '.' &&
        is_lower(game_buffer[chosen_piece_y][chosen_piece_x]) ==
        is_lower(game_buffer[cursor_y][cursor_x]))
        return false;

    switch (game_buffer[chosen_piece_y][chosen_piece_x]) {
        case 'R': {
            bool is_valid = (chosen_piece_x == cursor_x || chosen_piece_y == cursor_y) &&
                is_line_free();

            if (is_valid && chosen_piece_x == 7 && chosen_piece_y == 7)
                is_white_castle_valid = false;
            
            if (is_valid && chosen_piece_x == 0 && chosen_piece_y == 7)
                is_white_long_castle_valid = false;

            return is_valid;
        }
        case 'r': {
            bool is_valid = (chosen_piece_x == cursor_x || chosen_piece_y == cursor_y) &&
                is_line_free();

            if (is_valid && chosen_piece_x == 7 && chosen_piece_y == 0)
                is_black_castle_valid = false;
            
            if (is_valid && chosen_piece_x == 0 && chosen_piece_y == 0)
                is_black_long_castle_valid = false;

            return is_valid;
        }
        case 'B':
        case 'b':
            if (abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) ==
                abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y))
                return is_line_free();
            break;
        case 'N':
        case 'n':
            return (abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 2 &&
                abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y) == 1) ||
                (abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y) == 2 &&
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 1);
        case 'Q':
        case 'q':
            if (chosen_piece_x == cursor_x || chosen_piece_y == cursor_y ||
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) ==
                abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y))
                return is_line_free();
            break;
        case 'K': {
            if (cursor_x == 6 && cursor_y == 7 &&
                is_white_castle_valid &&
                game_buffer[7][5] == '.' &&
                game_buffer[7][6] == '.') {
                game_buffer[7][5] = 'R';
                game_buffer[7][7] = '.';
                return true;
            }

            if (cursor_x == 2 && cursor_y == 7 &&
                is_white_long_castle_valid &&
                game_buffer[7][1] == '.' &&
                game_buffer[7][2] == '.' &&
                game_buffer[7][3] == '.') {
                game_buffer[7][3] = 'R';
                game_buffer[7][0] = '.';
                return true;
            }

            bool is_valid = abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) < 2 &&
                abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y) < 2;
            
            if (is_valid) {
                is_white_castle_valid = false;
                is_white_long_castle_valid = false;
            }

            return is_valid;
        }
        case 'k': {
            if (cursor_x == 6 && cursor_y == 0 &&
                is_white_castle_valid &&
                game_buffer[0][5] == '.' &&
                game_buffer[0][6] == '.') {
                game_buffer[0][5] = 'r';
                game_buffer[0][7] = '.';
                return true;
            }

            if (cursor_x == 2 && cursor_y == 0 &&
                is_white_long_castle_valid &&
                game_buffer[0][1] == '.' &&
                game_buffer[0][2] == '.' &&
                game_buffer[0][3] == '.') {
                game_buffer[0][3] = 'r';
                game_buffer[0][0] = '.';
                return true;
            }

            bool is_valid = abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) < 2 &&
                abs<int8_t>((int8_t)chosen_piece_y - (int8_t)cursor_y) < 2;
            
            if (is_valid) {
                is_black_castle_valid = false;
                is_black_long_castle_valid = false;
            }

            return is_valid;
        }
        case 'P':
            if (cursor_y == 4 && chosen_piece_y == 6 &&
                chosen_piece_x == cursor_x &&
                game_buffer[5][cursor_x] == '.' && game_buffer[4][cursor_x] == '.') {
                last_charged_pawn_x = cursor_x;
                last_charged_pawn_move_number = move_number;
                return true;
            }

            if (last_charged_pawn_move_number == move_number - 1 &&
                cursor_y == 2 && chosen_piece_y == 3 &&
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 1 &&
                cursor_x == last_charged_pawn_x &&
                is_lower(game_buffer[3][cursor_x])) {
                game_buffer[3][cursor_x] = '.';
                return true;
            }

            return (chosen_piece_y - cursor_y == 1 && chosen_piece_x == cursor_x &&
                game_buffer[cursor_y][cursor_x] == '.') ||
                (chosen_piece_y - cursor_y == 1 &&
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 1 &&
                is_lower(game_buffer[cursor_y][cursor_x]));

        case 'p':
            if (cursor_y == 3 && chosen_piece_y == 1 &&
                chosen_piece_x == cursor_x &&
                game_buffer[2][cursor_x] == '.' && game_buffer[3][cursor_x] == '.') {
                last_charged_pawn_x = cursor_x;
                last_charged_pawn_move_number = move_number;
                return true;
            }

            if (last_charged_pawn_move_number == move_number - 1 &&
                cursor_y == 5 && chosen_piece_y == 4 &&
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 1 &&
                cursor_x == last_charged_pawn_x &&
                !is_lower(game_buffer[4][cursor_x])) {
                game_buffer[4][cursor_x] = '.';
                return true;
            }

            return (cursor_y - chosen_piece_y == 1 && chosen_piece_x == cursor_x &&
                game_buffer[cursor_y][cursor_x] == '.') ||
                (cursor_y - chosen_piece_y == 1 &&
                abs<int8_t>((int8_t)chosen_piece_x - (int8_t)cursor_x) == 1 &&
                !is_lower(game_buffer[cursor_y][cursor_x]));
    }
    return false;
}

void Game::draw() const {
    int winsizex, winsizey;
    getmaxyx(stdscr, winsizey, winsizex);

    for (uint8_t y = 0; y < winsizey; y++)
        for (uint8_t x = 0; x < winsizex; x++)
            mvaddch(y, x, ' ' | get_pair(COLOR_BLACK, COLOR_BLACK));

    if (winsizex < 9 || winsizey < 10) {
        refresh();
        return;
    }

    int startcoordx = (winsizex - 8) / 2;
    int startcoordy = (winsizey - 8) / 2;

    for (uint8_t y = 0; y < 8; y++)
        mvaddch(startcoordy + y, startcoordx - 1,
            (y + '1') | get_pair(COLOR_WHITE, COLOR_BLACK));
    
    for (uint8_t x = 0; x < 8; x++)
        mvaddch(startcoordy + 8, startcoordx + x,
            (x + 'a') | get_pair(COLOR_WHITE, COLOR_BLACK));
    
    mvaddch(startcoordy + 8, startcoordx - 1,
        '.' | ((move_number % 2 == 0) ?
            get_pair(COLOR_RED, COLOR_RED) :
            get_pair(COLOR_BLUE, COLOR_BLUE)));

    std::string move_number_string = "move:" + std::to_string(move_number);

    mvprintstr(startcoordy - 1, startcoordx - 1,
        move_number_string.c_str(), move_number_string.size(),
        A_BOLD | get_pair(COLOR_WHITE, COLOR_BLACK));

    for (uint8_t y = 0; y < 8; y++)
        for (uint8_t x = 0; x < 8; x++)
            mvaddch(startcoordy + y, startcoordx + x,
                game_buffer[y][x] | A_BOLD
                | get_pair(
                    (is_chosen && chosen_piece_x == x && chosen_piece_y == y) ?
                        COLOR_GREEN : (cursor_x == x && cursor_y == y) ?
                            COLOR_GREEN : (game_buffer[y][x] == '.') ?
                                ((y * 9 + x) % 2 == 0 ? COLOR_BLUE : COLOR_RED)
                                    : (is_lower(game_buffer[y][x]) ?
                                        COLOR_BLACK : COLOR_WHITE),
                    ((y * 9 + x) % 2 == 0 ? COLOR_BLUE : COLOR_RED))
                | ((cursor_y == y && cursor_x == x) ?
                    A_BLINK : 0));
    
    refresh();
}

void Game::draw_end_screen() const {
    int winsizex, winsizey;
    getmaxyx(stdscr, winsizey, winsizex);

    for (uint8_t y = 0; y < winsizey; y++)
        for (uint8_t x = 0; x < winsizex; x++)
            mvaddch(y, x, ' ' | get_pair(COLOR_BLACK, COLOR_BLACK));
    
    mvprintstr(winsizey / 2, (winsizex - 10) / 2, (move_number % 2 == 1) ?
        "WHITE wins" : "BLACK wins", 10, A_BOLD | get_pair(COLOR_WHITE, COLOR_BLACK));

    refresh();
}

void Game::input() {
    switch (getch()) {
        case 'Q':
        case 'q':
        case 27:
            is_running = false;
            break;
        case KEY_UP:
            cursor_y = max<int8_t>(cursor_y - 1, 0);
            break;
        case KEY_DOWN:
            cursor_y = min<int8_t>(cursor_y + 1, 7);
            break;
        case KEY_LEFT:
            cursor_x = max<int8_t>(cursor_x - 1, 0);
            break;
        case KEY_RIGHT:
            cursor_x = min<int8_t>(cursor_x + 1, 7);
            break;
        case 'Z':
        case 'z':
            if (!is_chosen) {
                if (game_buffer[cursor_y][cursor_x] == '.' ||
                    (is_lower(game_buffer[cursor_y][cursor_x]) !=
                        (move_number % 2) ? false : true))
                    break;
                chosen_piece_x = cursor_x;
                chosen_piece_y = cursor_y;
                is_chosen = true;
            } else {
                if (is_move_valid()) {
                    if (game_buffer[cursor_y][cursor_x] == 'K' ||
                        game_buffer[cursor_y][cursor_x] == 'k') {
                        is_running = false;
                        game_ended = true;
                        return;
                    }

                    game_buffer[cursor_y][cursor_x] =
                        game_buffer[chosen_piece_y][chosen_piece_x];
                    game_buffer[chosen_piece_y][chosen_piece_x] = '.';
                    move_number++;
                }
                is_chosen = false;
            }
            break;
        case 'X':
        case 'x':
            is_chosen = false;
            break;
    }
}

void Game::run() {
    is_running = true;

    while (is_running) {
        draw();

        input();
    }

    if (game_ended) {
        draw_end_screen();

        getch();
    }
}
