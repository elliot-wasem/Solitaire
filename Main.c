#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

#include "Card.h"
#include "Deck.h"
#include "GameState.h"

#define DECK_POS        0, 35
#define SOL_STACK_0_POS 0, 0
#define SOL_STACK_1_POS 0, 7
#define SOL_STACK_2_POS 0, 14
#define SOL_STACK_3_POS 0, 21

#define WORK_STACK0_POS 5, 0
#define WORK_STACK1_POS 5, 7
#define WORK_STACK2_POS 5, 14
#define WORK_STACK3_POS 5, 21
#define WORK_STACK4_POS 5, 28
#define WORK_STACK5_POS 5, 35
#define WORK_STACK6_POS 5, 42

void init_game(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks);
void draw_screen(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_keypress(char c, Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_selection(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_up(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_down(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_left(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void handle_right(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void print_state(GameState state);
bool game_complete(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state);
void draw_win_splashscreen();
void draw_help_menu();

int main(int argc, char *argv[]) {

    CardStack solution_stacks[4] = {
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 }
    };
    CardStack working_stacks[7] = {
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 },
        { .num_cards=0 }
    };

    GameState state = { .spot = WORKING_0, .index = 0, .saved_spot = NO_SPOT, .saved_index = 0, .help_menu_up = false };

    const DeckFunctions *dfuncs = get_deck_functions();

    Deck deck = dfuncs->fresh_deck();

    init_game(&deck, solution_stacks, working_stacks);

    char c = '\0';
    bool is_game_complete = false;

    while (!is_game_complete && c != 'q') {
        draw_screen(&deck, solution_stacks, working_stacks, &state);
        c = getch();
        handle_keypress(c, &deck, solution_stacks, working_stacks, &state);
        is_game_complete = game_complete(&deck, solution_stacks, working_stacks, &state);
    }

    draw_screen(&deck, solution_stacks, working_stacks, &state);

    if (is_game_complete) {
        draw_win_splashscreen();
        getch();
    }

    endwin();

    return 0;
}

// initializes the state of the game
void init_game(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks) {
    const DeckFunctions      *dfuncs = get_deck_functions();
    const CardStackFunctions *sfuncs = get_stack_functions();

    // necessary for unicode display
    setlocale(LC_ALL, "");

    // shuffle
    dfuncs->shuffle(deck);

    // initialize working stacks by taking from deck
    for (int i = 6; i >= 0; i--) {
        for (int j = i; j < 7; j++) {
            Card c = dfuncs->remove_card(deck);
            c.is_visible = false;
            sfuncs->add_to_stack(&working_stacks[j], c);
        }
    }
    for (int i = 0; i < 7; i++) {
        working_stacks[i].cards[working_stacks[i].num_cards-1].is_visible = true;
    }

    /* initialize screen */
    initscr();

    /* enables colors */
    start_color();

    /* initially refreshes screen, emptying it */
    refresh();

    /* keypresses will not be displayed on screen */
    noecho();

    /* hide cursor */
    curs_set(0);

    /* sets color pairs to numbers defined above */
    init_pair(RED,    COLOR_RED,    COLOR_BLACK);
    init_pair(GREEN,  COLOR_GREEN,  COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BLUE,   COLOR_BLUE,   COLOR_BLACK);
    init_pair(RESET,  COLOR_WHITE,  COLOR_BLACK);
}
// draws the screen of the game
void draw_screen(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardFunctions      *cfuncs = get_card_functions();
    const CardStackFunctions *sfuncs = get_stack_functions();
    if (state->help_menu_up) {
        draw_help_menu();
        return;
    }

    /* overwrites all characters on screen with blanks */
    erase();
    int color = RESET;

    if (solution_stacks[SOLUTION_0].num_cards) {
        if (state->spot == SOLUTION_0) {
            color = GREEN;
        } else if (state->saved_spot == SOLUTION_0) {
            color = YELLOW;
        }
        cfuncs->draw(sfuncs->top(solution_stacks[0]), SOL_STACK_0_POS, state->spot == SOLUTION_0 || state->saved_spot == SOLUTION_0, color);
    } else {
        cfuncs->draw_empty(SOL_STACK_0_POS, state->spot == SOLUTION_0);
    }
    if (solution_stacks[SOLUTION_1].num_cards) {
        if (state->spot == SOLUTION_1) {
            color = GREEN;
        } else if (state->saved_spot == SOLUTION_1) {
            color = YELLOW;
        }
        cfuncs->draw(sfuncs->top(solution_stacks[1]), SOL_STACK_1_POS, state->spot == SOLUTION_1 || state->saved_spot == SOLUTION_1, color);
    } else {
        cfuncs->draw_empty(SOL_STACK_1_POS, state->spot == SOLUTION_1);
    }
    if (solution_stacks[SOLUTION_2].num_cards) {
        if (state->spot == SOLUTION_2) {
            color = GREEN;
        } else if (state->saved_spot == SOLUTION_2) {
            color = YELLOW;
        }
        cfuncs->draw(sfuncs->top(solution_stacks[2]), SOL_STACK_2_POS, state->spot == SOLUTION_2 || state->saved_spot == SOLUTION_2, color);
    } else {
        cfuncs->draw_empty(SOL_STACK_2_POS, state->spot == SOLUTION_2);
    }
    if (solution_stacks[SOLUTION_3].num_cards) {
        if (state->spot == SOLUTION_3) {
            color = GREEN;
        } else if (state->saved_spot == SOLUTION_3) {
            color = YELLOW;
        }
        cfuncs->draw(sfuncs->top(solution_stacks[3]), SOL_STACK_3_POS, state->spot == SOLUTION_3 || state->saved_spot == SOLUTION_3, color);
    } else {
        cfuncs->draw_empty(SOL_STACK_3_POS, state->spot == SOLUTION_3);
    }

    sfuncs->draw(working_stacks[0], WORK_STACK0_POS, state->spot == WORKING_0, state->saved_spot == WORKING_0, *state);
    sfuncs->draw(working_stacks[1], WORK_STACK1_POS, state->spot == WORKING_1, state->saved_spot == WORKING_1, *state);
    sfuncs->draw(working_stacks[2], WORK_STACK2_POS, state->spot == WORKING_2, state->saved_spot == WORKING_2, *state);
    sfuncs->draw(working_stacks[3], WORK_STACK3_POS, state->spot == WORKING_3, state->saved_spot == WORKING_3, *state);
    sfuncs->draw(working_stacks[4], WORK_STACK4_POS, state->spot == WORKING_4, state->saved_spot == WORKING_4, *state);
    sfuncs->draw(working_stacks[5], WORK_STACK5_POS, state->spot == WORKING_5, state->saved_spot == WORKING_5, *state);
    sfuncs->draw(working_stacks[6], WORK_STACK6_POS, state->spot == WORKING_6, state->saved_spot == WORKING_6, *state);

    const DeckFunctions      *dfuncs = get_deck_functions();
    dfuncs->draw(*deck, DECK_POS, *state);

    mvprintw(4, 40, "h: help");

    // DEBUG ONLY
    // dfuncs->display(*deck, 0, 110);
    // for (int i = 0; i < 4; i++) {
    //     sfuncs->display(solution_stacks[i], i*5+20, 70);
    // }
    // for (int i = 0; i < 4; i++) {
    //     sfuncs->display(working_stacks[i], i*5+20, 90);
    // }
    // for (int i = 4; i < 7; i++) {
    //     sfuncs->display(working_stacks[i], (i-4)*5+20, 140);
    // }
}
// key press handler
void handle_keypress(char c, Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    if (state->help_menu_up) {
        if (c == 'x') {
            state->help_menu_up = false;
        } else {
            return;
        }
    }
    switch (c) {
        case 'h':
            state->help_menu_up = true;
            break;
        case 'w':
            handle_up(deck, solution_stacks, working_stacks, state);
            break;
        case 'a':
            handle_left(deck, solution_stacks, working_stacks, state);
            break;
        case 'd':
            handle_right(deck, solution_stacks, working_stacks, state);
            break;
        case 's':
            handle_down(deck, solution_stacks, working_stacks, state);
            break;
        case 'f':
            get_deck_functions()->flip(deck);
            state->saved_spot = NO_SPOT;
            state->saved_index = 0;
            break;
        case 'c':
            state->saved_spot = NO_SPOT;
            state->saved_index = 0;
            break;
        case ' ':
            handle_selection(deck, solution_stacks, working_stacks, state);
            break;
        case 'q':
            break;
        default:
            break;
    }
}
// handles a player pressing space to make a selection
void handle_selection(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    const CardFunctions *cfuncs = get_card_functions();
    const DeckFunctions *dfuncs = get_deck_functions();
    if (state->saved_spot == NO_SPOT) {
        state->saved_spot  = state->spot;
        state->saved_index = state->index;
    } else {
        switch (state->spot) {
        case DECK_STACK:
            state->saved_spot = state->spot;
            state->saved_index = state->index;
            break;
        case WORKING_0:
        case WORKING_1:
        case WORKING_2:
        case WORKING_3:
        case WORKING_4:
        case WORKING_5:
        case WORKING_6:
            switch(state->saved_spot) {
            case SOLUTION_0:
            case SOLUTION_1:
            case SOLUTION_2:
            case SOLUTION_3:
            {
                // solution to working
                CardStack *from_stack = &solution_stacks[state->saved_spot];
                unsigned int from_idx = from_stack->num_cards-1;
                CardStack *to_stack = &working_stacks[state->spot-WORKING_0];
                if (sfuncs->is_empty(*to_stack)) {
                    // empty target stack
                    if (from_stack->cards[from_idx].value != VALUE_KING) {
                        // not king, can't put it in an empty spot
                        state->spot = state->saved_spot;
                        state->index = state->saved_index;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    }
                } else {
                    // nonempty target stack
                    Card from_card = from_stack->cards[from_idx];
                    Card to_card = sfuncs->top(*to_stack);
                    if (cfuncs->is_stackable_regular(to_card, from_card)) {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->index++;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        state->saved_spot = state->spot;
                        state->saved_index = state->index;
                    }
                }
                break;
            }
            case WORKING_0:
            case WORKING_1:
            case WORKING_2:
            case WORKING_3:
            case WORKING_4:
            case WORKING_5:
            case WORKING_6:
            {
                // working to working
                CardStack *from_stack = &working_stacks[state->saved_spot-WORKING_0];
                unsigned int from_idx = state->saved_index;
                CardStack *to_stack = &working_stacks[state->spot-WORKING_0];
                if (sfuncs->is_empty(*to_stack)) {
                    // empty target stack
                    if (from_stack->cards[from_idx].value != VALUE_KING) {
                        // not king, can't put it in an empty spot
                        state->spot = state->saved_spot;
                        state->index = state->saved_index;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    }
                } else {
                    // nonempty target stack
                    Card from_card = from_stack->cards[from_idx];
                    Card to_card = sfuncs->top(*to_stack);
                    if (cfuncs->is_stackable_regular(to_card, from_card)) {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->index++;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        state->saved_spot = state->spot;
                        state->saved_index = state->index;
                    }

                }
                break;
            }
            case DECK_STACK:
            {
                // deck to working
                CardStack *work_stack = &working_stacks[state->spot-WORKING_0];
                if (sfuncs->is_empty(*work_stack)) {
                    // empty work stack
                    if (deck->discard[deck->num_cards_discard-1].value != VALUE_KING) {
                        // not a king, so go back to deck stack
                        state->spot = DECK_STACK;
                        state->index = 0;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        // is a king, so move it
                        deck->discard[deck->num_cards_discard-1].is_visible = true;
                        sfuncs->add_to_stack(work_stack, dfuncs->remove_from_stack(deck));
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    }
                } else {
                    // nonempty work stack
                    Card working_top = sfuncs->top(*work_stack);
                    if (cfuncs->is_stackable_regular(working_top, deck->discard[deck->num_cards_discard-1])) {
                        sfuncs->add_to_stack(work_stack, dfuncs->remove_from_stack(deck));
                        state->index++;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        // not compatible, stay on new stack
                        state->saved_spot = state->spot;
                        state->saved_index = state->index;
                    }
                }
                break;
            }
            default:
                break;
            }
            break;
        case SOLUTION_0:
        case SOLUTION_1:
        case SOLUTION_2:
        case SOLUTION_3:
            switch(state->saved_spot) {
            case WORKING_0:
            case WORKING_1:
            case WORKING_2:
            case WORKING_3:
            case WORKING_4:
            case WORKING_5:
            case WORKING_6:
            {
                // working to solution
                CardStack *from_stack = &working_stacks[state->saved_spot-WORKING_0];
                unsigned int from_idx = state->saved_index;
                CardStack *to_stack = &solution_stacks[state->spot];
                if (sfuncs->is_empty(*to_stack)) {
                    // empty target stack
                    if (from_stack->cards[from_idx].value != VALUE_ACE) {
                        // not ace, can't put it in an empty spot
                        state->spot = state->saved_spot;
                        state->index = state->saved_index;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    }
                } else {
                    // nonempty target stack
                    Card from_card = from_stack->cards[from_idx];
                    Card to_card = sfuncs->top(*to_stack);
                    if (cfuncs->is_stackable_solution(to_card, from_card)) {
                        sfuncs->move_to_stack(to_stack, from_stack, from_idx);
                        state->index++;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        state->spot = state->saved_spot;
                        state->index = state->saved_index;
                        //state->saved_spot = state->spot;
                        //state->saved_index = state->index;
                    }
                }
                break;
            }
            case DECK_STACK:
            {
                // deck to solution
                CardStack *to_stack = &solution_stacks[state->spot];
                if (sfuncs->is_empty(*to_stack)) {
                    // empty target stack
                    if (deck->discard[deck->num_cards_discard-1].value != VALUE_ACE) {
                        // not ace, can't put it in an empty spot
                        state->spot = state->saved_spot;
                        state->index = state->saved_index;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        sfuncs->add_to_stack(to_stack, dfuncs->remove_from_stack(deck));
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    }
                } else {
                    // nonempty target stack
                    Card from_card = deck->discard[deck->num_cards_discard-1];
                    Card to_card = sfuncs->top(*to_stack);
                    if (cfuncs->is_stackable_solution(to_card, from_card)) {
                        sfuncs->add_to_stack(to_stack, dfuncs->remove_from_stack(deck));
                        state->index++;
                        state->saved_spot = NO_SPOT;
                        state->saved_index = 0;
                    } else {
                        state->saved_spot = state->spot;
                        state->saved_index = state->index;
                    }
                }
                break;
            }
            break;
            default:
                state->saved_spot = state->spot;
                state->saved_index = state->index;
                break;
            }
        default:
            break;
        }
    }
}
// handles the player pressing w to move up
void handle_up(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    switch (state->spot) {
        // cannot move up from these
        case DECK_STACK:
        case SOLUTION_0:
        case SOLUTION_1:
        case SOLUTION_2:
        case SOLUTION_3:
            break;
        case WORKING_0:
        case WORKING_1:
        case WORKING_2:
        case WORKING_3:
        case WORKING_4:
        {
            // tries to move upward to the solution stack above it
            unsigned int stack_index = state->spot - WORKING_0;
            Card card = working_stacks[stack_index].cards[state->index];
            if (state->index == sfuncs->lowest_visible_index(working_stacks[stack_index])) {
                // adjust for WORKING_4 being to the side of SOLUTION_3
                if (stack_index == SOLUTION_3+1) { stack_index--; }
                if (solution_stacks[SOLUTION_0+stack_index].num_cards != 0 || state->saved_spot != NO_SPOT) {
                    state->spot = SOLUTION_0+stack_index;
                } else {
                    for (int i = SOLUTION_0; i <= SOLUTION_3; i++) {
                        if (solution_stacks[i].num_cards != 0) {
                            state->spot = i;
                            state->index = 0;
                            break;
                        }
                    }
                }
            } else if (card.is_visible) {
                Card prev_card = working_stacks[stack_index].cards[state->index-1];
                if (prev_card.is_visible) {
                    state->index--;
                } else {
                    state->spot = SOLUTION_0+stack_index;
                    // adjust for WORKING_4 being to the side of SOLUTION_3
                    if (stack_index == 4) { state->spot--; };
                }
            }
            break;
        }
        case WORKING_5:
        case WORKING_6:
        {
            unsigned int stack_index = state->spot - WORKING_0;
            Card card = working_stacks[stack_index].cards[state->index];
            if (state->index == 0) {
                if (deck->num_cards_discard != 0) {
                    state->spot = DECK_STACK;
                }
            } else if (card.is_visible) {
                Card prev_card = working_stacks[stack_index].cards[state->index-1];
                if (prev_card.is_visible) {
                    state->index--;
                } else {
                    if (deck->num_cards_discard != 0) {
                        state->spot = DECK_STACK;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}
// handles the player pressing s to move down
void handle_down(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    switch (state->spot) {
        // tries to move below
        case DECK_STACK:
        {
            // moves to one of the two right-most working stacks to the lowest (visually highest)
            // visible index
            if (!sfuncs->is_empty(working_stacks[5])) {
                state->spot  = WORKING_5;
                state->index = sfuncs->lowest_visible_index(working_stacks[5]);
            } else if (!sfuncs->is_empty(working_stacks[6])) {
                state->spot  = WORKING_6;
                state->index = sfuncs->lowest_visible_index(working_stacks[6]);
            } else if (state->saved_spot != NO_SPOT) {
                // if neither 5 nor 6 have cards and
                // if a selection has been made, then moves to 5 even if its empty
                state->spot  = WORKING_5;
                state->index = sfuncs->lowest_visible_index(working_stacks[5]);
            }
            break;
        }
        case SOLUTION_0:
        case SOLUTION_1:
        case SOLUTION_2:
        case SOLUTION_3:
        {
            // tries moving to working stack immediately below to visually highest/numerically
            // lowest index
            if (!sfuncs->is_empty(working_stacks[state->spot]) || state->saved_spot != NO_SPOT) {
                state->spot = state->spot + WORKING_0;
                state->index = sfuncs->lowest_visible_index(working_stacks[state->spot-WORKING_0]);
            } else {
                // if that fails, goes to the ordinally lowest stack
                sfuncs->go_to_lowest_stack(solution_stacks, working_stacks, state);
                state->index = sfuncs->lowest_visible_index(working_stacks[state->spot-WORKING_0]);
            }
            break;
        }
        case WORKING_0:
        case WORKING_1:
        case WORKING_2:
        case WORKING_3:
        case WORKING_4:
        case WORKING_5:
        case WORKING_6:
        {
            // tries to move downward on the stack its on
            unsigned int which_stack = state->spot - WORKING_0;
            if (state->index < sfuncs->highest_visible_index(working_stacks[which_stack])) {
                state->index++;
            }
            break;
        }
        default:
            break;
    }
}
// handles the player pressing a to move left
void handle_left(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    switch (state->spot) {
        // moves 1 left if possible, otherwise trying more
        case DECK_STACK:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_3]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_3;
                break;
            }
        // moves 1 left if possible, otherwise trying more
        case SOLUTION_3:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_2]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_2;
                break;
            }
        // moves 1 left if possible, otherwise trying more
        case SOLUTION_2:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_1]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_1;
                break;
            }
        // moves 1 left if possible, otherwise trying more
        case SOLUTION_1:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_0]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_0;
                break;
            }
        // moves 1 left if possible, otherwise not moving
        case SOLUTION_0:
            break;
        case WORKING_6:
        case WORKING_5:
        case WORKING_4:
        case WORKING_3:
        case WORKING_2:
        case WORKING_1:
        case WORKING_0:
        {
            do {
                // move left until a stack with 1 or more is found, unless a selection has been made, in
                // which case it will also move onto empty spots
                state->spot = state->spot == WORKING_0 ? WORKING_6 : state->spot-1;
            } while (state->saved_spot == NO_SPOT && sfuncs->is_empty(working_stacks[state->spot-WORKING_0]));
            // gets the index of the stack landed upon
            int stack_idx = state->spot-WORKING_0;
            // gets the highest and lowest visible indexes in the stack
            int highest_idx = sfuncs->highest_visible_index(working_stacks[stack_idx]);
            int lowest_idx = sfuncs->lowest_visible_index(working_stacks[stack_idx]);
            // if index is outside range of low-high, goes to the closest end
            if (state->index < lowest_idx) {
                state->index = lowest_idx;
            } else if (state->index > highest_idx) {
                state->index = highest_idx;
            }
            break;
        }
        default:
            break;
    }
}
// handles the player pressing d to move right
void handle_right(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    switch (state->spot) {
        // moves 1 right if possible, otherwise trying more
        case SOLUTION_0:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_1]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_1;
                break;
            }
        // moves 1 right if possible, otherwise trying more
        case SOLUTION_1:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_2]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_2;
                break;
            }
        // moves 1 right if possible, otherwise trying more
        case SOLUTION_2:
            if (!sfuncs->is_empty(solution_stacks[SOLUTION_3]) || state->saved_spot != NO_SPOT) {
                state->spot = SOLUTION_3;
                break;
            }
        // moves 1 right if possible, otherwise not moving
        case SOLUTION_3:
            if (deck->num_cards_discard != 0) {
                state->spot = DECK_STACK;
                break;
            }
        case DECK_STACK:
            break;
        case WORKING_6:
        case WORKING_5:
        case WORKING_4:
        case WORKING_3:
        case WORKING_2:
        case WORKING_1:
        case WORKING_0:
        {
            do {
                // move right until a stack with 1 or more is found, unless a selection has been made, in
                // which case it will also move onto empty spots
                state->spot = state->spot == WORKING_6 ? WORKING_0 : state->spot+1;
            } while (state->saved_spot == NO_SPOT && sfuncs->is_empty(working_stacks[state->spot-WORKING_0]));
            // gets the index of the stack landed upon
            int stack_idx = state->spot-WORKING_0;
            // gets the highest and lowest visible indexes in the stack
            int highest_idx = sfuncs->highest_visible_index(working_stacks[stack_idx]);
            int lowest_idx = sfuncs->lowest_visible_index(working_stacks[stack_idx]);
            // if index is outside range of low-high, goes to the closest end
            if (state->index < lowest_idx) {
                state->index = lowest_idx;
            } else if (state->index > highest_idx) {
                state->index = highest_idx;
            }
            break;
        }
        default:
            break;
    }
}
// prints the state of the game at the bottom of the screen
void print_state(GameState state) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    char *spot;
    char *saved_spot;
    switch(state.spot) {
        case SOLUTION_0:
            spot = "SOLUTION_0";
            break;
        case SOLUTION_1:
            spot = "SOLUTION_1";
            break;
        case SOLUTION_2:
            spot = "SOLUTION_2";
            break;
        case SOLUTION_3:
            spot = "SOLUTION_3";
            break;
        case WORKING_0:
            spot = "WORKING_0";
            break;
        case WORKING_1:
            spot = "WORKING_1";
            break;
        case WORKING_2:
            spot = "WORKING_2";
            break;
        case WORKING_3:
            spot = "WORKING_3";
            break;
        case WORKING_4:
            spot = "WORKING_4";
            break;
        case WORKING_5:
            spot = "WORKING_5";
            break;
        case WORKING_6:
            spot = "WORKING_6";
            break;
        case DECK_STACK:
            spot = "DECK_STACK";
            break;
        default:
            spot = "NO_SPOT";
            break;
    }
    switch(state.saved_spot) {
        case SOLUTION_0:
            saved_spot = "SOLUTION_0";
            break;
        case SOLUTION_1:
            saved_spot = "SOLUTION_1";
            break;
        case SOLUTION_2:
            saved_spot = "SOLUTION_2";
            break;
        case SOLUTION_3:
            saved_spot = "SOLUTION_3";
            break;
        case WORKING_0:
            saved_spot = "WORKING_0";
            break;
        case WORKING_1:
            saved_spot = "WORKING_1";
            break;
        case WORKING_2:
            saved_spot = "WORKING_2";
            break;
        case WORKING_3:
            saved_spot = "WORKING_3";
            break;
        case WORKING_4:
            saved_spot = "WORKING_4";
            break;
        case WORKING_5:
            saved_spot = "WORKING_5";
            break;
        case WORKING_6:
            saved_spot = "WORKING_6";
            break;
        case DECK_STACK:
            saved_spot = "DECK_STACK";
            break;
        default:
            saved_spot = "NO_SPOT";
            break;
    }
    mvprintw(max_y-2, 0, "SPOT:      %15s, INDEX:       %d", spot, state.index, max_x);
    mvprintw(max_y-1, 0, "SAVED SPOT:%15s, SAVED INDEX: %d", saved_spot, state.saved_index, max_x);
}

// returns whether or not the game is complete
bool game_complete(Deck *deck, CardStack *solution_stacks, CardStack *working_stacks, GameState *state) {
    return solution_stacks[SOLUTION_0].num_cards == 13
        && solution_stacks[SOLUTION_1].num_cards == 13
        && solution_stacks[SOLUTION_2].num_cards == 13
        && solution_stacks[SOLUTION_3].num_cards == 13;
}
void draw_win_splashscreen() {
    mvprintw(3, 6, "╔════════════════════════════════════╗");
    mvprintw(4, 6, "║                                    ║");
    mvprintw(5, 6, "║              WINNER!!              ║");
    mvprintw(6, 6, "║                                    ║");
    mvprintw(7, 6, "╚════════════════════════════════════╝");
}
void draw_help_menu() {
    mvprintw( 1, 2, "╔══════════════════════════════════════════════════╗");
    mvprintw( 2, 2, "║                                     x: exit help ║");
    mvprintw( 3, 2, "║ Controls:                                        ║");
    mvprintw( 4, 2, "║ ──────────────────────────────────────────────── ║");
    mvprintw( 5, 2, "║ w:      up                                       ║");
    mvprintw( 6, 2, "║ a:      left                                     ║");
    mvprintw( 7, 2, "║ s:      down                                     ║");
    mvprintw( 8, 2, "║ d:      right                                    ║");
    mvprintw( 9, 2, "║ f:      flip from deck to discard                ║");
    mvprintw(10, 2, "║ space:  select                                   ║");
    mvprintw(11, 2, "║ q:      quit game                                ║");
    mvprintw(12, 2, "║                                                  ║");
    mvprintw(13, 2, "║ Indicators:                                      ║");
    mvprintw(14, 2, "║ ──────────────────────────────────────────────── ║");
    mvprintw(15, 2, "║ yellow border:      selected                     ║");
    mvprintw(16, 2, "║ green border:       current position             ║");
    mvprintw(17, 2, "║ x on card:          empty spot                   ║");
    mvprintw(18, 2, "║ 4 symbols on card:  card present but not visible ║");
    mvprintw(19, 2, "║ ──────────────────────────────────────────────── ║");
    mvprintw(20, 2, "║                                                  ║");
    mvprintw(21, 2, "║ Author: Elliot Wasem        Github: elliot-wasem ║");
    mvprintw(22, 2, "╚══════════════════════════════════════════════════╝");



}
