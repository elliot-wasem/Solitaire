#ifndef __DECK_H__
#define __DECK_H__
#include "Card.h"
#include "GameState.h"

// struct to hold up to 52 cards
typedef struct {
    Card cards[52];
    unsigned int num_cards;
    Card discard[52];
    unsigned int num_cards_discard;
} Deck;

// handler struct for all functions related to decks
typedef struct {
    void (*print)(Deck);
    Deck (*fresh_deck)(void);
    void (*display)(Deck, int y, int x);
    void (*shuffle)(Deck *);
    void (*flip)(Deck *);
    void (*draw)(Deck, int y, int x, GameState);
    Card (*remove_card)(Deck *);
    Card (*remove_from_stack)(Deck *);
} DeckFunctions;

const DeckFunctions *get_deck_functions();

#endif /* __DECK_H__ */
