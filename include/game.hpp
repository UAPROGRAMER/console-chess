#pragma once

#include <cinttypes>

class Game {
private:
    char game_buffer[8][8];
    uint8_t cursor_y;
    uint8_t cursor_x;

    bool is_running;

    uint16_t move_number;

    bool game_ended;

    uint8_t chosen_piece_y;
    uint8_t chosen_piece_x;

    bool is_chosen;

    uint8_t last_charged_pawn_x;

    uint16_t last_charged_pawn_move_number;

    bool is_white_castle_valid;

    bool is_white_long_castle_valid;

    bool is_black_castle_valid;

    bool is_black_long_castle_valid;

    bool is_line_free() const;

    bool is_move_valid();

    void draw() const;

    void draw_end_screen() const;

    void input();
public:
    Game();
    ~Game();

    void run();
};
