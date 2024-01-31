#include "assert.h"
#include <algorithm>
#include <iostream>
#include <cmath>

#include "decisions.h"
#include "equityguesser.h"
#include "game.h"
#include "preflop.h"

const float URGENCY_THRESHOLD = .1;
const float NUTADVANTAGE[2] = {.1, .05}; // thresholds for OOP, IP
const float RADVANTAGE[2] = {.05, -.01}; // threholds for OOP, IP
const float OVERBET_PERCENTAGE = 0.5;    // overbets 50% of nuts
const float IPRADVANTAGE = -.01;
const float bluffValueRatio[3] = {2, 1, 0.5};
const float panic_bet = .05;
const float GWINBLUFF = 3;

void update_strat(std::vector<Hand>& actorRange, std::vector<Hand>& notActorRange, int pot, float mdf, bool canCheck,
                  bool isIp, int street, int thisgwin) {
    std::vector<int> actorHandOrders;
    std::vector<int> actorBluffOrders;
    std::vector<int> notActorHandOrders;

    guess_draw_potential(actorRange, notActorRange, 2 - street);

    float actorSize = actorRange.size(), notActorSize = notActorRange.size();

    for (int i = 0; i < actorRange.size(); i++) {
        actorHandOrders.push_back(i);
        actorBluffOrders.push_back(i);
        actorRange[i].strategy = NONE;
        actorRange[i].urgency = 0;
    }

    for (int i = 0; i < notActorRange.size(); i++) {
        notActorHandOrders.push_back(i);
    }

    std::sort(actorHandOrders.begin(), actorHandOrders.end(),
              [&](int a, int b) { return actorRange[a].equity > actorRange[b].equity; });
    std::sort(actorBluffOrders.begin(), actorBluffOrders.end(),
              [&](int a, int b) { return actorRange[a].draw_potential > actorRange[b].draw_potential; });
    std::sort(notActorHandOrders.begin(), notActorHandOrders.end(),
              [&](int a, int b) { return notActorRange[a].equity > notActorRange[b].equity; });

    improve_equity(actorRange,actorHandOrders,notActorRange,notActorHandOrders,street);
    std::sort(actorHandOrders.begin(), actorHandOrders.end(),
              [&](int a, int b) { return actorRange[a].equityImproved > actorRange[b].equityImproved; });
    std::sort(notActorHandOrders.begin(), notActorHandOrders.end(),
              [&](int a, int b) { return notActorRange[a].equityImproved > notActorRange[b].equityImproved; });


    // austin gwin case
    bool isBluffGwinMultiplier = street == 2 && pot > thisgwin;

    float buckets[2][5] = {{}, {}};

    int indexActor = actorSize - 1, indexNotActor = notActorSize - 1;
    float cumActor = 0, cumNotActor = 0;
    int flag60 = -1, flag40 = -1, flag80 = -1;

    while (indexActor > -1 && indexNotActor > -1) {
        int strengthActor = actorRange[actorHandOrders[indexActor]].equity;
        int strengthNotActor = notActorRange[notActorHandOrders[indexNotActor]].equity;
        if (strengthActor == strengthNotActor) {
            int bucket = 5 * cumActor / actorSize;
            buckets[1][std::min(4, bucket)] += 1.0 / notActorSize;

            bucket = 5 * cumNotActor / notActorSize;
            buckets[0][std::min(4, bucket)] += 1.0 / actorSize;
            if (flag80 == -1 && bucket == 4) {
                flag80 = indexActor;
            } else if (flag60 == -1 && bucket == 3) {
                flag60 = indexActor;
            } else if (flag40 == -1 && bucket == 2) {
                flag40 = indexActor;
            }
            indexNotActor -= 1;
            indexActor -= 1;

            cumNotActor += 1;
            cumActor += 1;
        } else if (strengthActor > strengthNotActor) {
            int bucket = 5 * cumActor / actorSize;
            buckets[1][std::min(4, bucket)] += 1.0 / notActorSize;

            cumNotActor += 1;
            indexNotActor -= 1;
        } else {
            int bucket = 5 * cumNotActor / notActorSize;
            buckets[0][std::min(4, bucket)] += 1.0 / actorSize;
            if (flag80 == -1 && bucket == 4) {
                flag80 = indexActor;
            } else if (flag60 == -1 && bucket == 3) {
                flag60 = indexActor;
            } else if (flag40 == -1 && bucket == 2) {
                flag40 = indexActor;
            }
            cumActor += 1;
            indexActor -= 1;
        }
    }

    buckets[0][4] += std::max(0.0f, indexActor / actorSize);
    buckets[1][4] += std::max(0.0f, indexNotActor / notActorSize);

    if (street == 2 && thisgwin / 4 < pot && thisgwin > pot) {
        float f = (float)pot / thisgwin;
        float bluffRate = (thisgwin - pot) / (2 * thisgwin - pot) / 2;
        float floor;
        if (f <= 0.9) {
            float floor = pow((1 - f), 0.5);
        } else {
            float floor = 0;
        }
        int startBetting = actorSize - 1;
        if (floor > .8) {
            startBetting = flag80;
        } else if (floor > 0.6) {
            startBetting = flag60;
        } else if (floor > 0.4) {
            startBetting = flag40;
        }
        int valueCum = 0;
        for (int i = startBetting; i > -1; i--) {
            actorRange[actorHandOrders[i]].strategy = RAISEGWIN;
            valueCum++;
        }

        for (int i = 0; i < actorSize; i++) {
            actorRange[actorBluffOrders[i]].strategy = RAISEGWIN;
            valueCum--;
            if (valueCum == 0) {
                break;
            }
        }

        for (int i = 0; i < actorSize; i++) {
            if (actorRange[i].strategy == NONE) {
                actorRange[i].strategy = canCheck ? CHECK : FOLD;
            }
        }
        return;
    }

    float foldTracker = 0; // tracks bluffs that have been moved from fold to bet.
    bool nutAdvantage = (buckets[0][4] - buckets[1][4]) > NUTADVANTAGE[isIp];
    bool rangeAdvantage = (buckets[0][4] + buckets[0][3] - buckets[1][4] - buckets[1][3]) > RADVANTAGE[isIp];
    std::cout << "equity distribution \n";
    for (int i = 0; i < 5; i++) {
        std::cout << buckets[0][i] << " " << buckets[1][i] << std::endl;
    }
    std::cout << "NUt and range advantages " << nutAdvantage << " " << rangeAdvantage << std::endl;
    int index;
    // Initialize bottom (1-mdf)% hands to fold.
    if (!canCheck) {
        float cumFold = 0;
        index = actorSize - 1;
        std::cout << "actor size " << actorSize << std::endl;
        std::cout << "must fold " << 1 - mdf << std::endl;
        while (cumFold < 1 - mdf) {
            actorRange[actorHandOrders[index]].strategy = FOLD;
            index -= 1;
            cumFold += 1 / actorSize;
        }
    }
    std::cout << "A" << std::endl;
    // strategy
    if (nutAdvantage) {
        // Value
        float overbetFreq = OVERBET_PERCENTAGE * (buckets[0][4]);
        // std::cout << "overbetting " << overbetFreq << std::endl;
        float cumFreq = 0;
        index = 0;
        while (cumFreq < overbetFreq) {
            // iterate through hands sorted by equity
            cumFreq += 1.0 / actorSize;
            actorRange[actorHandOrders[index]].strategy = RAISE100;
            index++;
        }

        // Finding Bluffs on a set ratio
        index = 0;
        cumFreq = 0;
        while (cumFreq < overbetFreq * bluffValueRatio[street] * (isBluffGwinMultiplier ? GWINBLUFF : 1)) {
            // iterate through hands sorted by bluff
            if (actorRange[actorBluffOrders[index]].strategy == FOLD) {
                foldTracker += 1;
            }
            actorRange[actorBluffOrders[index]].strategy = RAISE100;
            cumFreq += 1.0 / actorSize;

            index++;
        }
    }
    std::cout << "B" << std::endl;

    if (rangeAdvantage) {
        float cumSmallBet = 0;
        float cumMedBet = 0;
        int next_index = 0;

        for (int indexa = flag60; indexa > -1; indexa--) {
            if (actorRange[actorHandOrders[indexa]].urgency > URGENCY_THRESHOLD &&
                actorRange[actorHandOrders[indexa]].strategy == NONE) {
                cumMedBet += 1;
                actorRange[actorHandOrders[indexa]].strategy = RAISE50;
            } else if (actorRange[actorHandOrders[indexa]].strategy == NONE) {
                cumSmallBet += 1;
                actorRange[actorHandOrders[indexa]].strategy = RAISE33;
            }
        }
        std::cout << "BB" << std::endl;
        // update bluffs on a ratio. Finds best bluffs for med bet first, then picks up where left off for small bets.
        index = 0;
        float cumBluffTracker = 0;
        while (cumBluffTracker < cumMedBet * bluffValueRatio[street] * (isBluffGwinMultiplier ? GWINBLUFF : 1)) {
            if (actorRange[actorBluffOrders[index]].strategy == NONE) {
                actorRange[actorBluffOrders[index]].strategy = RAISE50;
            } else if (actorRange[actorBluffOrders[index]].strategy == FOLD) {
                foldTracker += 1;
                actorRange[actorBluffOrders[index]].strategy = RAISE50;
            }
            cumBluffTracker += 1;
            index++;
            if (index == actorSize) {
                // cant find low-equity bluffs for whatever reason
                break;
            }
        }
        std::cout << "CC" << std::endl;
        cumBluffTracker = 0;
        while (cumBluffTracker < cumSmallBet * bluffValueRatio[street] * (isBluffGwinMultiplier ? GWINBLUFF : 1)) {
            if (index == actorSize) {
                // cant find low-equity bluffs for whatever reason
                break;
            }
            if (actorRange[actorBluffOrders[index]].strategy == NONE) {
                actorRange[actorBluffOrders[index]].strategy = RAISE33;
            } else if (actorRange[actorBluffOrders[index]].strategy == FOLD) {
                foldTracker += 1;
                actorRange[actorBluffOrders[index]].strategy = RAISE33;
            }
            cumBluffTracker += 1;
            index++;
        }
        std::cout << "CCC" << std::endl;

        // checking range, reallocates hands with no strategy to fold until mdf is reached again. Allocates rest into
        // check Then begins to count checks in 40-60% range. Reallocates checks to 60-80% range.
        index = 0;
        while (foldTracker > 0 && index < actorSize) {
            if (actorRange[actorHandOrders[index]].strategy == NONE) {
                actorRange[actorHandOrders[index]].strategy = FOLD;
                foldTracker -= 1;
            }
            index++;
        }
        std::cout << "DD" << std::endl;

        float checkingFreq = 0;
        for (int i = flag60; i < flag40; i++) {
            if (actorRange[actorHandOrders[i]].strategy == NONE) {
                actorRange[actorHandOrders[i]].strategy = canCheck ? CHECK : CALL;
                checkingFreq += 1;
            }
        }
        for (int i = flag60; i > flag80; i--) {
            if (actorRange[actorHandOrders[i]].strategy == NONE) {
                actorRange[actorHandOrders[i]].strategy = canCheck ? CHECK : CALL;
                checkingFreq -= 1;
            } else if (checkingFreq > 0) {
                actorRange[actorHandOrders[i]].strategy = canCheck ? CHECK : CALL;
                checkingFreq -= 1;
            }
        }
        std::cout << "EE" << std::endl;
    }
    std::cout << "C" << std::endl;

    if (!rangeAdvantage and !nutAdvantage) {
        for (int i = 0; i < actorRange.size(); i++) {
            if (actorRange[i].strategy == NONE) {
                actorRange[i].strategy = canCheck ? CHECK : CALL;
            }
        }
    }
    std::cout << "D" << std::endl;
    // makes everything else check
    for (int i = 0; i < actorRange.size(); i++) {
        if (actorRange[i].strategy == NONE) {
            actorRange[i].strategy = canCheck ? CHECK : CALL;
        }
    }

    int ct[NUMACTIONS + 1] = {};
    for (int i = 0; i < actorSize; i++) {
        ct[actorRange[i].strategy] += 1;
    }
    std::cout << "strategy: ";
    for (int i = 0; i < NUMACTIONS + 1; i++) {
        std::cout << ct[i] << " ";
    }
}

int update_ranges(std::vector<Hand>& range, action action1, action action2) {
    int out = 0;
    int ct = 0;
    std::vector<Hand> emptyRange;
    for (int i = 0; i < range.size(); i++) {
        if (range[i].strategy == action1) {
            emptyRange.push_back(range[i]);
            ct++;
            if (range[i].isMyHand) {
                out = ct - 1;
            }
        }
    }
    if (ct == 0) {
        for (int i = 0; i < range.size(); i++) {
            if (range[i].strategy == action2) {
                emptyRange.push_back(range[i]);
                ct++;
                if (range[i].isMyHand) {
                    out = ct - 1;
                }
            }
        }
    }
    range = emptyRange;
    return out;
}

int update_ranges_preflop(std::vector<Hand>& range, int raises, int action, Card bigcard, Card smallcard) {
    for (int i = 0; i < 169; i++) {
        if (preflop_actions[raises][i] == action) {
            if (i < 13) {
                add_pairs(range, i);
            } else if (i > 90) {
                add_offsuit(range, i);
            } else {
                add_suited(range, i);
            }
        }
    }
    std::cout << "looking for " << bigcard << " " << smallcard << std::endl;
    for (int i = 0; i < range.size(); i++) {
        if (range[i].cards[0] == bigcard && range[i].cards[1] == smallcard) {
            range[i].isMyHand = true;
            return i;
        }
    }
    std::cout << bigcard << " " << smallcard << " hand not found" << std::endl;
    return -1;
}

void add_pairs(std::vector<Hand>& range, int index) {
    // std::cout << "adding " << 14-index << 14-index<< std::endl;
    Card card3 = NULL_CARD;
    int newCards = 0;
    int rank = 12 - index;
    for (int suit1 = 0; suit1 < 4; suit1++) {
        for (int suit2 = suit1 + 1; suit2 < 4; suit2++) {
            Hand newHand;
            int card1 = rank * 4 + suit2;
            int card2 = rank * 4 + suit1;
            newHand.cards[0] = (Card)card1;
            newHand.cards[1] = (Card)card2;
            newHand.cards[2] = card3;
            range.push_back(newHand);
            newCards++;
        }
    }

    assert(newCards == 6);
}

void add_suited(std::vector<Hand>& range, int index) {
    // std::cout << "adding " << indexCards[index-13][0] + 2 << indexCards[index-13][1] + 2 << "s"<<std::endl;

    int rank1 = indexCards[index - 13][0];
    int rank2 = indexCards[index - 13][1];
    Card card3 = NULL_CARD;
    int newCards = 0;
    for (int i = 0; i < 4; i++) {
        int card1 = rank1 * 4 + i;
        int card2 = rank2 * 4 + i;
        Hand newHand;
        newHand.cards[0] = (Card)card1;
        newHand.cards[1] = (Card)card2;
        newHand.cards[2] = card3;
        range.push_back(newHand);
        newCards++;
    }
    assert(newCards == 4);
}

void add_offsuit(std::vector<Hand>& range, int index) {
    // std::cout << "adding " << indexCards[index-91][0] + 2 << indexCards[index-91][1] + 2 << "o"<< std::endl;

    int rank1 = indexCards[index - 91][0];
    int rank2 = indexCards[index - 91][1];
    Card card3 = NULL_CARD;
    int newCards = 0;
    for (int suit1 = 0; suit1 < 4; suit1++) {
        for (int suit2 = suit1 + 1; suit2 < 4; suit2++) {
            int card1 = rank1 * 4 + suit1;
            int card2 = rank2 * 4 + suit2;
            Hand newHand;
            newHand.cards[0] = (Card)card1;
            newHand.cards[1] = (Card)card2;
            newHand.cards[2] = card3;
            range.push_back(newHand);
            // std::cout << "\t"<< card1 << " " << card2 << std::endl;

            card1 = rank1 * 4 + suit2;
            card2 = rank2 * 4 + suit1;
            Hand newHand2;
            newHand2.cards[0] = (Card)card1;
            newHand2.cards[1] = (Card)card2;
            newHand2.cards[2] = card3;
            range.push_back(newHand2);
            // std::cout << "\t"<< card1 << " " << card2 << std::endl;
            newCards += 2;
        }
    }
    assert(newCards == 12);
}

void update_auction(std::vector<Hand>& range, std::vector<Card>& board) {
    std::cout << "UPDATING AUCTION RANGE. CURRENT SIZE = " << range.size() << std::endl;
    std::vector<Hand> newRange;
    for (int curHand = 0; curHand < range.size(); curHand++) {
        for (int curCard = 0; curCard < NULL_CARD; curCard++) {
            Card newCard = (Card)curCard;
            bool blocked = false;
            for (int i = 0; i < board.size(); i++) {
                if (board[i] == newCard) {
                    blocked = true;
                    break;
                }
            }
            if (blocked || newCard == range[curHand].cards[0] || newCard == range[curHand].cards[1]) {
                continue;
            }
            Hand newHand;
            newHand.cards[0] = range[curHand].cards[0];
            newHand.cards[1] = range[curHand].cards[1];
            newHand.cards[2] = newCard;
            newRange.push_back(newHand);
        }
    }
    range = newRange;
    std::cout << "FINISHED UPDATING AUCTION RANGE. CURRENT SIZE = " << range.size() << std::endl;
}

void add_blocker(std::vector<Hand>& range, Card card) {
    std::vector<Hand> newRange;
    for (int i = 0; i < range.size(); i++) {
        if (range[i].cards[0] != card && range[i].cards[1] != card && range[i].cards[2] != card) {
            newRange.push_back(range[i]);
        }
    }
    range = newRange;
}

void panic(std::vector<Hand>& range, float mdf, bool canCheck) {
    float rangeSize = range.size();
    std::vector<int> actorHandOrders = {};
    for (int i = 0; i < range.size(); i++) {
        actorHandOrders.push_back(i);
        range[i].strategy = canCheck ? CHECK : CALL;
    }
    std::sort(actorHandOrders.begin(), actorHandOrders.end(),
              [&](int a, int b) { return range[a].equity > range[b].equity; });

    if (!canCheck) {
        // assign (1-mdf) hands to fold
        float ct = 0;
        int index = rangeSize - 1;
        while (ct < 1 - mdf) {
            ct += 1.0 / rangeSize;
            range[actorHandOrders[index]].strategy = FOLD;
            index -= 1;
        }
    }

    // assign top panic_bet percent of hands to raise 50%.
    float ct = 0;
    int index = 0;
    while (ct < panic_bet) {
        range[actorHandOrders[index]].strategy = RAISE50;
        ct += 1.0 / rangeSize;
        index++;
    }
}
