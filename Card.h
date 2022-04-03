#ifndef __CARD_H__
#define __CARD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GameState.h"

// quick color definitions, for ease of use.
// colors declared inside main
#define RED    1
#define GREEN  2
#define YELLOW 3
#define BLUE   4
#define RESET  5

// Maximum number of cards in a stack. That's 13 visible stacked, plus 6 hidden
#define MAX_CARDS_IN_STACK 19

typedef enum { SPADE, DIAMOND, CLUB, HEART, NUM_SUITS } SUIT;

typedef enum {
    VALUE_ACE, VALUE_2, VALUE_3, VALUE_4, VALUE_5, VALUE_6, VALUE_7,
    VALUE_8, VALUE_9, VALUE_10, VALUE_JACK, VALUE_QUEEN, VALUE_KING, NUM_VALUES
} VALUE;

typedef struct {
    SUIT  suit;
    VALUE value;
    bool is_visible;
} Card;

// represents a stack of cards
typedef struct {
    Card cards[MAX_CARDS_IN_STACK];
    unsigned int num_cards;
} CardStack;

// holds references to all functions operating on cards or about cards
typedef struct {
    void (*print)(Card);
    const int *(*suit_string)(SUIT);
    const char *(*value_string)(VALUE);
    int (*is_stackable_regular)(Card old_card, Card new_card);
    int (*is_stackable_solution)(Card old_card, Card new_card);
    void (*draw)(Card, int y, int x, bool, int color);
    void (*draw_blank)(int y, int x);
    void (*draw_empty)(int y, int x, bool);
    bool (*equal)(Card, Card);
} CardFunctions;

// holds references to all functions operating on card stacks or about card stacks
typedef struct {
    void (*add_to_stack)(CardStack *, Card);
    Card (*remove_from_stack)(CardStack *);
    void (*move_to_stack)(CardStack *to, CardStack *from, unsigned int index);
    Card (*top)(CardStack);
    void (*draw)(CardStack, int y, int x, bool, bool, GameState);
    void (*display)(CardStack, int y, int x);
    bool (*is_empty)(CardStack);
    unsigned int (*highest_visible_index)(CardStack);
    unsigned int (*lowest_visible_index)(CardStack);
    void (*go_to_lowest_stack)(CardStack *, CardStack *, GameState *);
} CardStackFunctions;

// returns pointer to card function or stack function handler
const CardFunctions *get_card_functions();
const CardStackFunctions *get_stack_functions();

#endif /* __CARD_H__ */
