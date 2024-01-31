#ifndef GAME_H
#define GAME_H

#define SUITS 4
#define RANKS 13
#define CARDS (SUITS * RANKS)

enum Suit { HEART, CLUB, DIAMOND, SPADE };

enum Rank { DEUCE, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

enum Card {
    DEUCE_HEARTS,
    DEUCE_CLUBS,
    DEUCE_DIAMONDS,
    DEUCE_SPADES,
    THREE_HEARTS,
    THREE_CLUBS,
    THREE_DIAMONDS,
    THREE_SPADES,
    FOUR_HEARTS,
    FOUR_CLUBS,
    FOUR_DIAMONDS,
    FOUR_SPADES,
    FIVE_HEARTS,
    FIVE_CLUBS,
    FIVE_DIAMONDS,
    FIVE_SPADES,
    SIX_HEARTS,
    SIX_CLUBS,
    SIX_DIAMONDS,
    SIX_SPADES,
    SEVEN_HEARTS,
    SEVEN_CLUBS,
    SEVEN_DIAMONDS,
    SEVEN_SPADES,
    EIGHT_HEARTS,
    EIGHT_CLUBS,
    EIGHT_DIAMONDS,
    EIGHT_SPADES,
    NINE_HEARTS,
    NINE_CLUBS,
    NINE_DIAMONDS,
    NINE_SPADES,
    TEN_HEARTS,
    TEN_CLUBS,
    TEN_DIAMONDS,
    TEN_SPADES,
    JACK_HEARTS,
    JACK_CLUBS,
    JACK_DIAMONDS,
    JACK_SPADES,
    QUEEN_HEARTS,
    QUEEN_CLUBS,
    QUEEN_DIAMONDS,
    QUEEN_SPADES,
    KING_HEARTS,
    KING_CLUBS,
    KING_DIAMONDS,
    KING_SPADES,
    ACE_HEARTS,
    ACE_CLUBS,
    ACE_DIAMONDS,
    ACE_SPADES,
    NULL_CARD,
};

enum HandType { HIGH_CARD, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL_HOUSE, QUADS, STRAIGHT_FLUSH };

inline Card make_card(Suit suit, Rank rank) { return Card(SUITS * rank + suit); }

inline Suit get_suit(Card card) { return Suit(card % SUITS); }

inline Rank get_rank(Card card) { return Rank(card / SUITS); }

enum action {
    CALL,
    CHECK,
    FOLD,
    RAISE33,
    RAISE50,
    RAISE100,
    RAISEGWIN,
    NONE,
};

const float raise_sizes[3] = {0.33, 0.5, 1.0};

const int NUMACTIONS = 6;

class Hand {
  public:
    float equity;
    float equityImproved;
    float draw_potential;
    //potential to outdraw and bluff with
    int flush_draw;
    int straight_draw;
    int low_pair;
    //potential to outdraw but dont bluff with this
    int overpair = 0;

    //potential to outdraw some bluffs
    int quads_fullhouse = 0; // beats flush straight overpair
    int two_pair_trips = 0; //beats overpair

    action strategy = NONE;
    float urgency;
    Card cards[3];
    bool isMyHand = false;
};

#endif
