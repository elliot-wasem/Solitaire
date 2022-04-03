#include "Deck.h"
#include "Card.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void print_deck(Deck);
Deck get_fresh_deck(void);
void display_deck(Deck deck, int y, int x);
void shuffle(Deck *);
void flip(Deck *);
void draw_deck(Deck deck, int y, int x, GameState);
Card remove_card(Deck *);
Card remove_from_discard(Deck *);

DeckFunctions deck_functions = {
    .print=print_deck,
    .fresh_deck=get_fresh_deck,
    .display=display_deck,
    .shuffle=shuffle,
    .flip=flip,
    .draw=draw_deck,
    .remove_card=remove_card,
    .remove_from_stack=remove_from_discard
};

// prints the contents of a deck in text format
void print_deck(Deck deck) {
    const CardFunctions *cfuncs = get_card_functions();
    printf("Num cards: %d\n", deck.num_cards);
    for (unsigned int i = 0; i < deck.num_cards; i++) {
        if (i > 0 && i % NUM_VALUES == 0) {
            printf("\n");
        }
        cfuncs->print(deck.cards[i]);
    }
    printf("\n");
}

// gives an ordered deck of 52 cards
Deck get_fresh_deck(void) {
    Deck deck = { .num_cards=0, .num_cards_discard=0 };
    for (SUIT suit = SPADE; suit < NUM_SUITS; suit++) {
        for (VALUE value = VALUE_ACE; value < NUM_VALUES; value++) {
            deck.cards[deck.num_cards++] = (Card){
                .suit=suit, .value=value, .is_visible=true
            };
        }
    }
    return deck;
}

// returns pointer to the handler for deck functions
const DeckFunctions *get_deck_functions() {
    return &deck_functions;
}

// displays deck and discard stack and its contents
void display_deck(Deck deck, int y, int x) {
    const CardFunctions *cfuncs = get_card_functions();
    unsigned int row = 0, col = 0;
    for (unsigned int i = 0; i < deck.num_cards; i++) {
        cfuncs->draw(deck.cards[i], y + row * 4, x + col * 6, false, RESET);
        col++;
        if (col == 13) {
            col = 0;
            row++;
        }

    }
    col = 0;
    row += 2;
    for (unsigned int i = 0; i < deck.num_cards_discard; i++) {
        cfuncs->draw(deck.discard[i], y + row * 4, x + col * 6, false, RESET);
        col++;
        if (col == 13) {
            col = 0;
            row++;
        }
    }
}

// shuffles the cards that are in the deck
void shuffle(Deck *deck) {
    Card temp_buf[52];
    srand(time(NULL));

    for (int cards_left = deck->num_cards; cards_left > 0; cards_left--) {
        int card_to_remove = rand() % cards_left;
        temp_buf[cards_left-1] = deck->cards[card_to_remove];
        for (int i = card_to_remove; i < cards_left; i++) {
            deck->cards[i] = deck->cards[i+1];
        }
    }

    for (int i = 0; i < deck->num_cards; i++) {
        deck->cards[i] = temp_buf[i];
    }
}

// flips a card from the deck to the discard pile, recycling the discard into
// the deck if necessary
void flip(Deck * deck) {
    if (deck->num_cards == 0) {
        while (deck->num_cards_discard) {
            deck->cards[deck->num_cards++] = deck->discard[--deck->num_cards_discard];
        }
    }
    deck->discard[deck->num_cards_discard++] = deck->cards[--deck->num_cards];
}

// draws the deck for the game
void draw_deck(Deck deck, int y, int x, GameState state) {
    const CardFunctions *cfuncs = get_card_functions();
    if (deck.num_cards_discard) {
        int color = RESET;
        if (state.spot == DECK_STACK) {
            color = GREEN;
        } else if (state.saved_spot == DECK_STACK) {
            color = YELLOW;
        }
        cfuncs->draw(deck.discard[deck.num_cards_discard-1], y, x, state.spot == DECK_STACK || state.saved_spot == DECK_STACK, color);
    } else {
        cfuncs->draw_empty(y, x, false);
    }
    if (deck.num_cards) {
        cfuncs->draw_blank(y, x+7);
    } else {
        cfuncs->draw_empty(y, x+7, false);
    }
}
// removes a card from the top of the deck, returning it
Card remove_card(Deck *deck) {
    return deck->cards[--deck->num_cards];
}
// removes a card from the top of the discard pile, returning it
Card remove_from_discard(Deck *deck) {
    return deck->discard[--deck->num_cards_discard];
}
