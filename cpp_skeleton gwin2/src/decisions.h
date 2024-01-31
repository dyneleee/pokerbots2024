#ifndef DECISIONS_H
#define DECISIONS_H

#include "game.h"
#include <vector>

void update_strat(std::vector<Hand>& ipRange, std::vector<Hand>& oopRange, int pot, float mdf, bool canCheck, bool isIp,
                  int street, int thisgwin);

void panic(std::vector<Hand>& ipRange, std::vector<Hand>& oopRange, bool isIp);

int update_ranges(std::vector<Hand>& range, action action1, action action2);

int update_ranges_preflop(std::vector<Hand>& range, int raises, int action, Card bigcard, Card smallcard);

void add_pairs(std::vector<Hand>& range, int index);

void add_suited(std::vector<Hand>& range, int index);

void add_offsuit(std::vector<Hand>& range, int index);

void update_auction(std::vector<Hand>& range, std::vector<Card>& board);

void add_blocker(std::vector<Hand>& range, Card card);

void panic(std::vector<Hand>& range, float mdf, bool canCheck);

#endif
