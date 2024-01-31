#include <algorithm>
#include <bitset>
#include <iostream>
#include <random>

#include "equity.h"
#include "game.h"

const int p13[7] = {1, 13, 169, 2197, 28561, 371293, 4826809};

inline int get_digit(int number, int digit) { return (number / p13[digit]) % 13; }

int get_strength(std::vector<Card> cards) {
    int n = cards.size();

    std::sort(cards.begin(), cards.end(), [](Card a, Card b) { return a > b; });

    int card_streak = 1, straight_streak = 1;
    int prev_pair = -1, prev_trip = -1;
    int suit_counts[SUITS] = {0, 0, 0, 0};
    int strength = HIGH_CARD * p13[5];
    bool straight_flag = false;
    for (int i = 0; i < n; i++) {
        suit_counts[get_suit(cards[i])]++;
    }
    for (int i = 0; i < n; i++) {
        if (i) {
            if (get_rank(cards[i]) == get_rank(cards[i - 1])) {
                card_streak++;
            } else {
                card_streak = 1;
            }
            if (get_rank(cards[i]) == get_rank(cards[i - 1]) - 1) {
                straight_streak++;
            } else if (get_rank(cards[i]) != get_rank(cards[i - 1])) {
                straight_streak = 1;
            }
        }
        if (card_streak == 2) {
            if (prev_trip != -1) {
                strength = std::max(strength, FULL_HOUSE * p13[5] + prev_trip * p13[4] + get_rank(cards[i]) * p13[3]);
            } else if (prev_pair != -1) {
                strength = std::max(strength, TWO_PAIR * p13[5] + prev_pair * p13[4] + get_rank(cards[i]) * p13[3]);
            } else {
                prev_pair = get_rank(cards[i]);
                strength = std::max(strength, PAIR * p13[5] + get_rank(cards[i]) * p13[4]);
            }
        } else if (card_streak == 3) {
            if (prev_pair == get_rank(cards[i])) {
                prev_pair = -1;
            }
            if (prev_pair != -1) {
                strength = std::max(strength, FULL_HOUSE * p13[5] + get_rank(cards[i]) * p13[4] + prev_pair * p13[3]);
            } else {
                prev_trip = get_rank(cards[i]);
                strength = std::max(strength, TRIPS * p13[5] + get_rank(cards[i]) * p13[4]);
            }
        } else if (card_streak == 4) {
            strength = std::max(strength, QUADS * p13[5] + get_rank(cards[i]) * p13[4]);
        }
        if (straight_streak == 5) {
            straight_flag = true;
            strength = std::max(strength, STRAIGHT * p13[5] + (get_rank(cards[i]) + 4) * p13[4]);
        } else if (straight_streak == 4 && get_rank(cards[i]) == DEUCE && get_rank(cards[0]) == ACE) {
            straight_flag = true;
            strength = std::max(strength, STRAIGHT * p13[5] + FIVE * p13[4]);
        }
    }
    for (int i = 0; i < SUITS; i++) {
        if (suit_counts[i] >= 5) {
            if (straight_flag) {
                int straight_flush_streak = 0, current_rank = -1;
                bool ace = false;
                for (int j = 0; j < n; j++) {
                    if (get_suit(cards[j]) == i) {
                        if (get_rank(cards[j]) == ACE) {
                            ace = true;
                        }
                        if (get_rank(cards[j]) == current_rank - 1) {
                            straight_flush_streak++;
                            current_rank--;
                        } else if (get_rank(cards[j]) != current_rank) {
                            straight_flush_streak = 1;
                            current_rank = get_rank(cards[j]);
                        }
                        if (straight_flush_streak == 5) {
                            strength = std::max(strength, STRAIGHT_FLUSH * p13[5] + (current_rank + 4) * p13[4]);
                        } else if (straight_flush_streak == 4 && current_rank == DEUCE && ace) {
                            strength = std::max(strength, STRAIGHT_FLUSH * p13[5] + FIVE * p13[4]);
                        }
                    }
                }
            }
            strength = std::max(strength, FLUSH * p13[5]);
        }
    }

    HandType hand_type = HandType(strength / p13[5]);
    int kickers_needed = std::vector<int>{5, 3, 1, 2, 0, 5, 0, 1, 0}[hand_type];
    std::vector<int> non_kickers;
    if (hand_type == PAIR) {
        non_kickers.push_back(get_digit(strength, 4));
    } else if (hand_type == TWO_PAIR) {
        non_kickers.push_back(get_digit(strength, 4));
        non_kickers.push_back(get_digit(strength, 3));
    } else if (hand_type == TRIPS) {
        non_kickers.push_back(get_digit(strength, 4));
    } else if (hand_type == QUADS) {
        non_kickers.push_back(get_digit(strength, 4));
    }

    int curr_card = 0;
    while (kickers_needed > 0 && curr_card < n) {
        bool is_kicker = true;
        for (int i = 0; i < (int)non_kickers.size(); i++) {
            if (get_rank(cards[curr_card]) == non_kickers[i]) {
                is_kicker = false;
                break;
            }
        }
        if (is_kicker && (hand_type != FLUSH || suit_counts[get_suit(cards[curr_card])] >= 5)) {
            kickers_needed--;
            strength += get_rank(cards[curr_card]) * p13[kickers_needed];
        }
        curr_card++;
    }

    return strength;
}

void update_strengths(std::vector<Hand>& range, std::vector<Card>& board) {
    for (int i = 0; i < range.size(); i++) {
        std::vector<Card> newBoard = board;
        for (int j = 0; j < 3; j++) {
            if (range[i].cards[j] == NULL_CARD) {
                continue;
            }
            newBoard.push_back(range[i].cards[j]);
        }
        range[i].equity = get_strength(newBoard);
    }
}

const int num_simulations = 1000;

std::random_device rd;
std::mt19937 gen(rd());
std::bitset<CARDS> blocked;

void get_hvr_equity(Hand& hand, std::vector<Hand>& range, std::vector<Card>& board) {
    for (int i = 0; i < 3; i++) {
        if (hand.cards[i] == NULL_CARD) {
            continue;
        }
        blocked[hand.cards[i]] = true;
    }
    float equity = 0;
    int orig_size = board.size();
    for (int i = 0; i < num_simulations; i++) {
        for (int j = 0; j < 5 - orig_size; j++) {
            int card = gen() % CARDS;
            while (blocked[card]) {
                card = gen() % CARDS;
            }
            board.push_back(Card(card));
            blocked[card] = true;
        }
        std::vector<Card> board_copy = board;
        for (int j = 0; j < 3; j++) {
            if (hand.cards[j] == NULL_CARD) {
                continue;
            }
            board_copy.push_back(hand.cards[j]);
        }
        int strength1 = get_strength(board_copy);
        board_copy.resize(5);
        Hand& hand2 = range[gen() % range.size()];
        for (int j = 0; j < 3; j++) {
            if (hand2.cards[j] == NULL_CARD) {
                continue;
            }
            board_copy.push_back(hand2.cards[j]);
        }
        int strength2 = get_strength(board_copy);
        if (strength1 > strength2) {
            equity += 1;
        } else if (strength1 == strength2) {
            equity += 0.5;
        }
        for (int j = 0; j < 5 - orig_size; j++) {
            blocked[board.back()] = false;
            board.pop_back();
        }
    }
    hand.equity = equity / num_simulations;
    for (int i = 0; i < 3; i++) {
        if (hand.cards[i] == NULL_CARD) {
            continue;
        }
        blocked[hand.cards[i]] = false;
    }
}

void get_rvr_equity(std::vector<Hand>& range1, std::vector<Hand>& range2, std::vector<Card>& board) {
    for (int i = 0; i < (int)range1.size(); i++) {
        get_hvr_equity(range1[i], range2, board);
    }
    for (int i = 0; i < (int)range2.size(); i++) {
        get_hvr_equity(range2[i], range1, board);
    }
}
