#include "Card.h"
#include "GameState.h"
#include <ncursesw/ncurses.h>
#include <locale.h>

// card functions
void draw_card(Card, int y, int x, bool, int color);
void draw_blank_card(int y, int x);
void print_card(Card);
const int *suit_string(SUIT);
const char *value_string(VALUE);
int is_stackable_regular(Card old_card, Card new_card);
int is_stackable_solution(Card old_card, Card new_card);
const char *suit_color(SUIT suit);
bool equal(Card, Card);
void draw_empty_card(int y, int x, bool);

// stack functions
void add_to_stack(CardStack *, Card);
Card remove_from_stack(CardStack *);
void move_to_stack(CardStack *to, CardStack *from, unsigned int index);
Card top(CardStack);
void draw_stack(CardStack, int y, int x, bool selected_stack, bool saved_stack, GameState state);
void display_stack(CardStack, int y, int x);
bool is_empty(CardStack);
unsigned int highest_visible_index(CardStack);
unsigned int lowest_visible_index(CardStack);
void go_to_lowest_stack(CardStack *, CardStack *, GameState *);

// string constants for suits, colors, and values
const int *SUIT_STR[5] = { L"♠", L"♦", L"♣", L"♥", NULL };
const char *COLOR_STRS[6] = {
  // red,        green       yellow      blue        reset
    "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[0m", NULL};
const char *VALUE_STR[14] = {
    "A", "2", "3", "4", "5", "6", "7",
    "8", "9", "10", "J", "Q", "K", NULL
};

// initializer for card function handler
const CardFunctions card_functions = {
    .print=print_card,
    .suit_string=suit_string,
    .value_string=value_string,
    .is_stackable_regular=is_stackable_regular,
    .is_stackable_solution=is_stackable_solution,
    .draw=draw_card,
    .draw_blank=draw_blank_card,
    .equal=equal,
    .draw_empty=draw_empty_card
};

// initializer for card stack function handler
const CardStackFunctions card_stack_functions = {
    .add_to_stack=add_to_stack,
    .remove_from_stack=remove_from_stack,
    .move_to_stack=move_to_stack,
    .top=top,
    .draw=draw_stack,
    .display=display_stack,
    .is_empty=is_empty,
    .highest_visible_index=highest_visible_index,
    .lowest_visible_index=lowest_visible_index,
    .go_to_lowest_stack=go_to_lowest_stack
};

// returns string representation of a suit
const int *suit_string(SUIT suit) {
    return SUIT_STR[suit];
}
// returns string representation of a value
const char *value_string(VALUE value) {
    return VALUE_STR[value];
}
// returns a pointer to the card function handler
const CardFunctions *get_card_functions() {
    // set locale so that unicode works
    setlocale(LC_ALL, "");
    return &card_functions;
}
// returns true if new_card can go on top of bottom_card if the bottom_card is
// on a solution stack
int is_stackable_solution(Card bottom_card, Card new_card) {
    return new_card.value == bottom_card.value+1 && new_card.suit == bottom_card.suit;
}
// returns true if new_card can go on top of bottom_card if the bottom_card is
// on a regular stack
int is_stackable_regular(Card bottom_card, Card new_card) {
    if (new_card.value == bottom_card.value-1) {
        switch(bottom_card.suit) {
            case DIAMOND:
            case HEART:
                return new_card.suit == CLUB || new_card.suit == SPADE;
            case CLUB:
            case SPADE:
                return new_card.suit == DIAMOND || new_card.suit == HEART;
            default:
                return false;
        }
    }
    return false;
}
// returns a color string based on suit
const char *suit_color(SUIT suit) {
    if (suit == HEART || suit == DIAMOND) {
        return COLOR_STRS[0];
    } else {
        return COLOR_STRS[4];
    }
}
// prints a simple string of the card
void print_card(Card card) {
    printf("%s%s%ls\033[0m", value_string(card.value), suit_color(card.suit), suit_string(card.suit));
}
// draws a card on the screen. Takes into account whether or not the color needs to be different.
void draw_card(Card card, int y, int x, bool selected, int color) {
    if (!card.is_visible) {
        draw_blank_card(y, x);
        return;
    }
    if (selected) {
        attron(COLOR_PAIR(color));
    }
    mvprintw(y,   x, "┌────┐");
    mvprintw(y+1, x, "│");
    if (selected) {
        attroff(COLOR_PAIR(color));
    }
    printw("%2s", value_string(card.value));
    if (card.suit == DIAMOND || card.suit == HEART) {
        attron(COLOR_PAIR(RED));
    }
    printw("%ls", suit_string(card.suit));
    if (card.suit == DIAMOND || card.suit == HEART) {
        attroff(COLOR_PAIR(RED));
    }
    if (selected) {
        attron(COLOR_PAIR(color));
    }
    printw(" │");
    mvprintw(y+2, x, "│    │");
    mvprintw(y+3, x, "└────┘");
    if (selected) {
        attroff(COLOR_PAIR(color));
    }
}
// draws a blank card
void draw_blank_card(int y, int x) {
    mvprintw(y,   x, "┌────┐");
    mvprintw(y+1, x, "│♠  ♦│");
    mvprintw(y+2, x, "│♥  ♣│");
    mvprintw(y+3, x, "└────┘");
}

// adds a card to the stack
void add_to_stack(CardStack *stack, Card card) {
    stack->cards[stack->num_cards++] = card;
}
// removes a card from the stack and returns it
Card remove_from_stack(CardStack *stack) {
    return stack->cards[stack->num_cards--];
}
// move cards from "index" to the end from the "from" stack to the "to" stack
void move_to_stack(CardStack *to, CardStack *from, unsigned int index) {
    for (int i = index; i < from->num_cards; i++) {
        add_to_stack(to, from->cards[i]);
    }
    from->num_cards = index;

    if (index > 0) {
        from->cards[index-1].is_visible = true;
    }
}
// returns whatever card is at the top of the stack
Card top(CardStack stack) {
    return stack.cards[stack.num_cards-1];
}
// returns a pointer to the handler for the stack functions
const CardStackFunctions *get_stack_functions() {
    return &card_stack_functions;
}
// returns whether or not the two cards are equivalent
bool equal(Card card1, Card card2) {
    return card1.suit == card2.suit && card1.value == card2.value;
}
// draws the stack on the screen
void draw_stack(CardStack stack, int y, int x, bool selected_stack, bool saved_stack, GameState state) {
    const CardFunctions *cfuncs = get_card_functions();
    if (stack.num_cards == 0) {
        cfuncs->draw_empty(y, x, selected_stack);
        return;
    }

    for (int i = 0, row = 0; i < stack.num_cards; i++) {
        bool selected = selected_stack && state.index == i;
        bool saved_selected = saved_stack && state.saved_index == i;
        int color = RESET;
        if (selected) {
            color = GREEN;
        } else if (saved_selected) {
            color = YELLOW;
        }
        cfuncs->draw(stack.cards[i], y+row, x, selected || saved_selected, color);
        if (stack.cards[i].is_visible) {
            row += 2;
        } else {
            row += 1;
        }
    }
}
// draws an empty card spot
void draw_empty_card(int y, int x, bool selected) {
    if (selected) {
        attron(COLOR_PAIR(GREEN));
    }
    mvprintw(y,   x, "┌────┐");
    mvprintw(y+1, x, "│ ╲╱ │");
    mvprintw(y+2, x, "│ ╱╲ │");
    mvprintw(y+3, x, "└────┘");
    if (selected) {
        attroff(COLOR_PAIR(GREEN));
    }
}
// returns whether or not a stack is empty
bool is_empty(CardStack stack) {
    return stack.num_cards == 0;
}
// returns the highest index of all visible cards in the stack
unsigned int highest_visible_index(CardStack stack) {
    return stack.num_cards-1;
}
// returns the lowest index of all visible cards in the stack
unsigned int lowest_visible_index(CardStack stack) {
    if (is_empty(stack)) {
        return 0;
    }
    for (int i = 0; i < stack.num_cards; i++) {
        if (stack.cards[i].is_visible) {
            return i;
        }
    }
    return stack.num_cards-1;
}
// displays the contents of the stack at given x y coordinates
void display_stack(CardStack stack, int y, int x) {
    const CardFunctions *cfuncs = get_card_functions();
    mvprintw(y, x, "Num cards: %d", stack.num_cards);
    for (int i = 0; i < stack.num_cards; i++) {
        cfuncs->draw(stack.cards[i], y+1, x+i*6, false, RESET);
    }
}
// finds the ordinally lowest working stack
void go_to_lowest_stack(CardStack *solution_stack, CardStack *working_stack, GameState *state) {
    const CardStackFunctions *sfuncs = get_stack_functions();
    for (int i = WORKING_0; i <= WORKING_6; i++) {
        if (!sfuncs->is_empty(working_stack[i-WORKING_0])) {
            state->spot = i;
            state->index = sfuncs->highest_visible_index(working_stack[i-WORKING_0]);
            state->saved_spot = NO_SPOT;
            state->saved_index = 0;
            return;
        }
    }
}
