#ifndef EQUITYGUESSER_H
#define EQUITYGUESSER_H

#include "game.h"
#include <vector>

void hand_draws(Hand hand, const std::vector<Card>& board);

void guess_equities(std::vector<Hand>& range, const std::vector<Card>& board);

void guess_draw_potential(std::vector<Hand>& range1, std::vector<Hand>& range2, int cards_left);

void improve_equity(std::vector<Hand>& myRange, std::vector<int>& myRangeOrder, std::vector<Hand>& oppRange, std::vector<int>& oppRangeOrder, int street);

#endif
