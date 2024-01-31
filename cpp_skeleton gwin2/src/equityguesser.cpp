#include <algorithm>
#include <bitset>
#include <iostream>

#include "equityguesser.h"

void hand_draws(Hand hand, const std::vector<Card>& board) {
    int suit_counts[SUITS] = {0};
    int rank_counts[RANKS] = {0};
    std::bitset<RANKS> counted;
    int num_cards = board.size();
    int low_board = RANKS;
    // std::cout << "B" << std::endl;

    for (int i = 0; i < num_cards; i++) {
        suit_counts[get_suit(board[i])]++;
        rank_counts[get_rank(board[i])]++;
        low_board = std::min(low_board, (int)get_rank(board[i]));
    }
    // std::cout << "C" << std::endl;

    hand.flush_draw = 0;
    hand.straight_draw = 0;
    hand.low_pair = 0;
    for (int i = 0; i < 3; i++) {
        if (hand.cards[i] == NULL_CARD) {
            continue;
        }
        // std::cout << "AA" << std::endl;
        // std::cout << get_suit(hand.cards[i]) << std::endl;
        // std::cout << get_rank(hand.cards[i]) << std::endl;
        suit_counts[get_suit(hand.cards[i])]++;
        rank_counts[get_rank(hand.cards[i])]++;
    }
    // std::cout << "D" << std::endl;

    for (int i = 0; i < SUITS; i++) {
        if (suit_counts[i] == 4) {
            hand.flush_draw = RANKS - suit_counts[i];
            break;
        }
    }
    // std::cout << "E" << std::endl;
    int curr = 0;
    int curr_zero = -1;
    for (int i = -1; i < 4; i++) {
        curr += (bool)rank_counts[(i + RANKS) % RANKS];
        if (!rank_counts[(i + RANKS) % RANKS]) {
            curr_zero = i;
        }
    }
    for (int i = 4; i < RANKS + 1; i++) {
        if (curr == 4 && !counted[curr_zero]) {
            counted[curr_zero] = true;
            hand.straight_draw += 4 - (bool)(hand.flush_draw != 0);
        }
        curr -= (bool)rank_counts[(i - 5 + RANKS) % RANKS];
        curr += (bool)rank_counts[i % RANKS];
        if (!rank_counts[i % RANKS]) {
            curr_zero = i;
        }
    }
    // std::cout << "F" << std::endl;
    if (rank_counts[low_board] == 2) {
        hand.low_pair += 5;
    } else {
        for (int i = 0; i < low_board; i++) {
            if (rank_counts[i] == 2) {
                hand.low_pair += 2;
                break;
            }
        }
    }
    // std::cout << "G" << std::endl;
    for (int i = 0; i < 3; i++) {
        rank_counts[get_rank(hand.cards[i])]--;
        suit_counts[get_suit(hand.cards[i])]--;
    }
    counted.reset();
    // std::cout << "H" << std::endl;
}

void guess_equities(std::vector<Hand>& range, const std::vector<Card>& board) {
    // std::cout << "A" << std::endl;
    int suit_counts[SUITS] = {0};
    int rank_counts[RANKS] = {0};
    std::bitset<RANKS> counted;
    int num_cards = board.size();
    int low_board = RANKS;
    // std::cout << "B" << std::endl;

    for (int i = 0; i < num_cards; i++) {
        suit_counts[get_suit(board[i])]++;
        rank_counts[get_rank(board[i])]++;
        low_board = std::min(low_board, (int)get_rank(board[i]));
    }
    // std::cout << "C" << std::endl;

    for (Hand& hand : range) {
        hand.flush_draw = 0;
        hand.straight_draw = 0;
        hand.low_pair = 0;
        for (int i = 0; i < 3; i++) {
            if (hand.cards[i] == NULL_CARD) {
                continue;
            }
            // std::cout << "AA" << std::endl;
            // std::cout << get_suit(hand.cards[i]) << std::endl;
            // std::cout << get_rank(hand.cards[i]) << std::endl;
            suit_counts[get_suit(hand.cards[i])]++;
            rank_counts[get_rank(hand.cards[i])]++;
        }
        // std::cout << "D" << std::endl;

        for (int i = 0; i < SUITS; i++) {
            if (suit_counts[i] == 4) {
                hand.flush_draw = RANKS - suit_counts[i];
                break;
            }
        }
        // std::cout << "E" << std::endl;
        int curr = 0;
        int curr_zero = -1;
        for (int i = -1; i < 4; i++) {
            curr += (bool)rank_counts[(i + RANKS) % RANKS];
            if (!rank_counts[(i + RANKS) % RANKS]) {
                curr_zero = i;
            }
        }
        for (int i = 4; i < RANKS + 1; i++) {
            if (curr == 4 && !counted[curr_zero]) {
                counted[curr_zero] = true;
                hand.straight_draw += 4 - (bool)(hand.flush_draw != 0);
            }
            curr -= (bool)rank_counts[(i - 5 + RANKS) % RANKS];
            curr += (bool)rank_counts[i % RANKS];
            if (!rank_counts[i % RANKS]) {
                curr_zero = i;
            }
        }
        // std::cout << "F" << std::endl;
        if (rank_counts[low_board] == 2) {
            hand.low_pair += 5;
        } else {
            for (int i = 0; i < low_board; i++) {
                if (rank_counts[i] == 2) {
                    hand.low_pair += 2;
                    break;
                }
            }
        }
        // std::cout << "G" << std::endl;
        for (int i = 0; i < 3; i++) {
            rank_counts[get_rank(hand.cards[i])]--;
            suit_counts[get_suit(hand.cards[i])]--;
        }
        counted.reset();
        // std::cout << "H" << std::endl;
    }
    // std::cout << "I" << std::endl;
}

/**
 * IMPORTANT: make sure all hands have their draws calculated before calling this function
 */
void guess_draw_potential(std::vector<Hand>& range1, std::vector<Hand>& range2, int cards_left) {
    float range1_flush = 0, range2_flush = 0;
    float range1_straight = 0, range2_straight = 0;
    float range1_low = 0, range2_low = 0;
    auto hit_prob = [&](int num_cards) {
        float p = 1;
        int not_seen = CARDS - 2 - (5 - cards_left);
        for (int i = 0; i < cards_left; i++) {
            p *= (float)(not_seen - i - num_cards) / (not_seen - i);
        }
        return 1 - p;
    };
    for (const Hand& hand : range1) {
        range1_flush += hit_prob(hand.flush_draw);
        range1_straight += hit_prob(hand.straight_draw);
        range1_low += hit_prob(hand.low_pair);
    }
    for (const Hand& hand : range2) {
        range2_flush += hit_prob(hand.flush_draw);
        range2_straight += hit_prob(hand.straight_draw);
        range2_low += hit_prob(hand.low_pair);
    }
    range1_flush /= range1.size();
    range2_flush /= range2.size();
    range1_straight /= range1.size();
    range2_straight /= range2.size();
    range1_low /= range1.size();
    range2_low /= range2.size();
    // std::cout << range1_flush << " " << range2_flush << std::endl;
    // std::cout << range1_straight << " " << range2_straight << std::endl;
    // std::cout << range1_low << " " << range2_low << std::endl;
    for (Hand& hand : range1) {
        hand.draw_potential = 0;
        hand.draw_potential += hit_prob(hand.flush_draw) * (1 - range2_flush / 2);
        hand.draw_potential += hit_prob(hand.straight_draw) * (1 - range2_straight / 2 - range2_flush);
        hand.draw_potential += hit_prob(hand.low_pair) * (1 - range2_low / 2 - range2_straight - range2_flush);
    }
    for (Hand& hand : range2) {
        hand.draw_potential = 0;
        hand.draw_potential += hit_prob(hand.flush_draw) * (1 - range1_flush / 2);
        hand.draw_potential += hit_prob(hand.straight_draw) * (1 - range1_straight / 2 - range1_flush);
        hand.draw_potential += hit_prob(hand.low_pair) * (1 - range1_low / 2 - range1_straight - range1_flush);
    }
}


void improve_equity(std::vector<Hand>& myRange, std::vector<int>& myRangeOrder, std::vector<Hand>& oppRange, std::vector<int>& oppRangeOrder, int street){
    // Set equity based purely on strength for right now.
    int mySize = myRange.size(), oppSize = oppRange.size();
    int myIndex = mySize - 1, oppIndex = oppSize - 1;
    while (myIndex > - 1 && oppIndex > -1){
        if (myRange[myRangeOrder[myIndex]].equity > oppRange[oppRangeOrder[oppIndex]].equity){
            //My hand is stronger -> opposite of next case
            oppRange[oppRangeOrder[oppIndex]].equityImproved = myIndex / mySize;
            oppIndex--;
        } else if (myRange[myRangeOrder[myIndex]].equity < oppRange[oppRangeOrder[oppIndex]].equity) {
            //My hand is weaker -> update size of opponents range u beat and move onto ur next hand
            myRange[myRangeOrder[myIndex]].equityImproved = oppIndex / oppSize;
            myIndex--;
        } else {
            //Tie
            oppRange[oppRangeOrder[oppIndex]].equityImproved = myIndex / mySize;
            myRange[myRangeOrder[myIndex]].equityImproved = oppIndex / oppSize;
            oppIndex--;
            myIndex--;
        }
    }
    while (myIndex > -1){
        myRange[myRangeOrder[myIndex]].equityImproved = 1;
        myIndex--;
    }
    while (oppIndex < -1){
        oppRange[oppRangeOrder[oppIndex]].equityImproved = 1;
        oppIndex--;
    }

    float myRangeFlush = 0, myRangeStraight = 0, myRangeOverPair = 0;
    float oppRangeFlush = 0, oppRangeStraight = 0, oppRangeOverPair = 0;
    
    //Sum flush, straight, and overpair draws for each range
    int cards_left = 2 - street;
    auto hit_prob = [&](int num_cards) {
        float p = 1;
        int not_seen = CARDS - 2 - (5 - cards_left);
        for (int i = 0; i < cards_left; i++) {
            p *= (float)(not_seen - i - num_cards) / (not_seen - i);
        }
        return 1 - p;
    };
    for (const Hand& hand : myRange) {
        myRangeFlush += hit_prob(hand.flush_draw) / mySize;
        myRangeStraight += hit_prob(hand.straight_draw) / mySize;
        myRangeOverPair += hit_prob(hand.overpair) / mySize;
    }
    for (const Hand& hand : oppRange) {
        oppRangeFlush += hit_prob(hand.flush_draw) / oppSize;
        oppRangeStraight += hit_prob(hand.straight_draw) / oppSize;
        oppRangeOverPair += hit_prob(hand.overpair) / oppSize;
    }

    //Update equities for bofa players based on equity gaining draws (overpair, straight, flush), and defending against those with draws to better.
    //Basically you can only gain equity with equity gaining draws, and defending with better draws can only bring it to no change in equity. 
    for (Hand& hand : myRange) {
        float delta = 0;
        delta += hit_prob(hand.flush_draw) * (1 - oppRangeFlush / 2); // you get flush and beat out their flushes
        delta -= std::min(0.0f,oppRangeFlush * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) / 2)); // op gets flush and you don't beat it out
        delta += hit_prob(hand.straight_draw) * (1 - oppRangeFlush - oppRangeStraight / 2);// you get straight and beat out half of theirs
        delta -= std::min(0.0f, oppRangeStraight * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) - hit_prob(hand.straight_draw)/2)); // op gets straight and you dont beat it out
        delta += hit_prob(hand.overpair) * (1 - oppRangeFlush - oppRangeStraight - oppRangeOverPair / 2);// you get overpair and ur opponent doesnt beat out
        delta -= std::min(0.0f, oppRangeStraight * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) - hit_prob(hand.two_pair_trips) - hit_prob(hand.overpair)/2)); // opponent gets overpair and you dont beat it out
        //THIS NEEDS FIXING LOLLL so scuffed there might be some non-negligible 2nd order approx going on here
        hand.equityImproved += delta;
    }
    for (Hand& hand : oppRange) {
        float delta = 0;
        delta += hit_prob(hand.flush_draw) * (1 - myRangeFlush / 2); // you get flush and beat out their flushes
        delta -= std::min(0.0f,myRangeFlush * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) / 2)); // op gets flush and you don't beat it out
        delta += hit_prob(hand.straight_draw) * (1 - myRangeFlush - myRangeStraight / 2);// you get straight and beat out half of theirs
        delta -= std::min(0.0f, myRangeStraight * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) - hit_prob(hand.straight_draw)/2)); // op gets straight and you dont beat it out
        delta += hit_prob(hand.overpair) * (1 - myRangeFlush - myRangeStraight - myRangeOverPair / 2);// you get overpair and ur opponent doesnt beat out
        delta -= std::min(0.0f, myRangeStraight * (1 - hit_prob(hand.quads_fullhouse) - hit_prob(hand.flush_draw) - hit_prob(hand.two_pair_trips) - hit_prob(hand.overpair)/2)); // opponent gets overpair and you dont beat it out
        //THIS NEEDS FIXING LOLLL so scuffed there might be some non-negligible 2nd order approx going on here
        hand.equityImproved += delta;
    }
}