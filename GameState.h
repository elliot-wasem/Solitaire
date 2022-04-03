#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__
#include <stdbool.h>

// enum to represent a spot on the board
typedef enum {
    SOLUTION_0, SOLUTION_1, SOLUTION_2, SOLUTION_3,
    WORKING_0, WORKING_1, WORKING_2, WORKING_3, WORKING_4, WORKING_5, WORKING_6,
    DECK_STACK, NO_SPOT
} SELECTED_SPOT;

// struct to hold the state of the player's cursor and selection in the game
typedef struct {
    SELECTED_SPOT spot;
    SELECTED_SPOT saved_spot;
    unsigned int index;
    unsigned int saved_index;
    bool help_menu_up;
} GameState;

#endif /* __GAME_STATE_H__ */
