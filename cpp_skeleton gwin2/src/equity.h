#ifndef EQUITY_H
#define EQUITY_H

#include <vector>

#include "game.h"

int get_strength(std::vector<Card> cards);

void update_strengths(std::vector<Hand>& range, std::vector<Card>& board);

void get_hvr_equity(Hand& hand, std::vector<Hand>& range, std::vector<Card>& board);

void get_rvr_equity(std::vector<Hand>& range1, std::vector<Hand>& range2, std::vector<Card>& board);

#endif
