#include <skeleton/actions.h>
#include <skeleton/constants.h>
#include <skeleton/runner.h>
#include <skeleton/states.h>

#include "assert.h"
#include "decisions.h"
#include "equity.h"
#include "equityguesser.h"
#include "game.h"
#include "math.h"
#include "preflop.h"

#include <random>

int char_to_rank[256];
int char_to_suit[256];
std::string rank_to_char = "23456789TJQKA";
std::string suit_to_char = "hcds";

using namespace pokerbots::skeleton;

struct Bot {
    bool foldNow = false;
    bool bigBlind;
    int preflop_raises;
    int numRaises;
    std::vector<Hand> myRange;
    std::vector<Hand> notMyRange;
    int myBankroll;
    int cur_street;
    std::vector<Card> board;
    Hand myHand;
    int myHandIndex;
    int myHandPreflop;
    bool update_equities = false;
    int oppPrevBet;
    bool isPanic = false;
    bool oppCanCheck;
    float oppmdf;
    int gwin = 1501;
    int tempgwin = gwin;

    /*
      Called when a new round starts. Called NUM_ROUNDS times.

      @param gameState The GameState object.
      @param roundState The RoundState object.
      @param active Your player's index.
    */
    void handleNewRound(GameInfoPtr gameState, RoundStatePtr roundState, int active) {
        // int myBankroll = gameState->bankroll;  // the total number of chips you've gained or lost from the beginning
        // of the game to the start of this round float gameClock = gameState->gameClock;  // the total number of
        // seconds your bot has left to play this game int roundNum = gameState->roundNum;  // the round number from 1
        // to State.NUM_ROUNDS

        std::cout << "GWin: " << gwin << std::endl;
        auto myCards = roundState->hands[active]; // your cards
        bigBlind = (active == 1);                 // true if you are the big blind

        cur_street = 0;
        preflop_raises = int(bigBlind) - 1; // posting big blind counts as raise 1
        numRaises = 0;

        // stores ranges
        myRange = {};
        notMyRange = {};

        // hi
        myBankroll = gameState->bankroll;
        // foldNow = myBankroll >= (1000 - (float)gameState->roundNum) * 1.5 + 1 + (0.5 * (float)bigBlind);
        foldNow = (gwin <= 0);
        cur_street = 0;

        board = {};
        isPanic = false;

        std::vector<Card> actual_cards;
        for (const auto& thing : myCards) {
            if (thing.size() == 2) {
                actual_cards.push_back(make_card(Suit(char_to_suit[thing[1]]), Rank(char_to_rank[thing[0]])));
            }
        }
        myHand.cards[0] = actual_cards[0];
        myHand.cards[1] = actual_cards[1];
        myHand.cards[2] = NULL_CARD;

        if (get_rank(myHand.cards[0]) == get_rank(myHand.cards[1])) {
            myHandPreflop = 12 - get_rank(myHand.cards[0]);
        } else if (get_suit(myHand.cards[0]) == get_suit(myHand.cards[1])) {
            int small = std::min(get_rank(myHand.cards[0]), get_rank(myHand.cards[1]));
            int big = std::max(get_rank(myHand.cards[0]), get_rank(myHand.cards[1]));
            myHandPreflop = 77 - (big) * (big - 1) / 2 - small + 13;
        } else {
            int small = std::min(get_rank(myHand.cards[0]), get_rank(myHand.cards[1]));
            int big = std::max(get_rank(myHand.cards[0]), get_rank(myHand.cards[1]));
            myHandPreflop = 77 - (big) * (big - 1) / 2 - small + 91;
        }
        std::cout << "\n Round " << gameState->roundNum << ", " << gameState->gameClock << " seconds left."
                  << std::endl;

        std::cout << myHandPreflop << std::endl;

        oppCanCheck = true;
        oppmdf = 1;
        tempgwin = gwin;
    }

    /*
      Called when a round ends. Called NUM_ROUNDS times.

      @param gameState The GameState object.
      @param terminalState The TerminalState object.
      @param active Your player's index.
    */
    void handleRoundOver(GameInfoPtr gameState, TerminalStatePtr terminalState, int active) {
        int myDelta = terminalState->deltas[active]; // your bankroll change from this round
        // auto previousState = std::static_pointer_cast<const RoundState>(terminalState->previousState);  // RoundState
        // before payoffs int street = previousState->street;  // 0, 3, 4, or 5 representing when this round ended auto
        // myCards = previousState->hands[active];  // your cards auto oppCards = previousState->hands[1-active];  //
        // opponent's cards or "" if not revealed
        gwin -= myDelta;
        gwin -= bigBlind + 1;
    }

    /*
      Where the magic happens - your code should implement this function.
      Called any time the engine needs an action from your bot.

      @param gameState The GameState object.
      @param roundState The RoundState object.
      @param active Your player's index.
      @return Your action.
    */
    Action getAction(GameInfoPtr gameState, RoundStatePtr roundState, int active) {
        std::cout << "\n\nNEW CALL TO GETACTION, STREET = " << roundState->street << ", HANDINDEX = " << myHandIndex
                  << "\n";
        // May be useful, but you can choose to not use.
        auto legalActions = roundState->legalActions(); // the actions you are allowed to take
        int street = roundState->street; // 0, 3, 4, or 5 representing pre-flop, flop, turn, or river respectively
        auto myCards = roundState->hands[active];  // your cards
        auto boardCards = roundState->deck;        // the board cards
        int myPip = roundState->pips[active];      // the number of chips you have contributed to the pot this round of
                                                   // betting
        int oppPip = roundState->pips[1 - active]; // the number of chips your opponent has contributed to the pot this
                                                   // round of betting
        int myStack = roundState->stacks[active];  // the number of chips you have remaining
        int oppStack = roundState->stacks[1 - active];            // the number of chips your opponent has remaining
        std::optional<int> myBid = roundState->bids[active];      // How much you bid previously (available only after
                                                                  // auction)
        std::optional<int> oppBid = roundState->bids[1 - active]; // How much opponent bid previously (available only
                                                                  // after auction)
        int continueCost = oppPip - myPip;                        // the number of chips needed to stay in the pot
        int myContribution = STARTING_STACK - myStack;            // the number of chips you have contributed to the pot
        int oppContribution = STARTING_STACK - oppStack; // the number of chips your opponent has contributed to the pot
        int minCost = 0;
        int maxCost = 0;
        if (legalActions.find(Action::Type::RAISE) != legalActions.end()) {
            auto raiseBounds = roundState->raiseBounds(); // the smallest and largest numbers of chips for a legal
                                                          // bet/raise
            minCost = raiseBounds[0] - myPip;             // the cost of a minimum bet/raise
            maxCost = raiseBounds[1] - myPip;             // the cost of a maximum bet/raise
        }
        int effective_stack = std::min(myStack, oppStack);
        int pot = myContribution + oppContribution; // includes all bets that have been made
        float spr = effective_stack / pot;
        float geometric_sizing = (pow((float)((effective_stack * 2 + pot + continueCost) / (float)(pot + continueCost)),
                                      (1 / (6 - (float)street))) -
                                  1) /
                                 2;
        float pot_odds = continueCost / (continueCost + pot);
        float mdf = 1 / (1 + ((float)continueCost / (pot - continueCost)));
        float bluffValueRatioGeometric;
        if (continueCost == 0 && (myStack == 0|| oppStack == 0)){
            //All in;
            return {Action::Type::CHECK};
        }
        if (foldNow) {
            return {Action::Type::FOLD};
        }
        if (street == 0 && !roundState->auction) {
            preflop_raises++;
            std::cout << "preflop raises: " << preflop_raises << std::endl;
            switch (preflop_actions[preflop_raises][myHandPreflop]) {
            case 0:
                // if (continueCost == 0){
                //     //add some range handling here
                //     return {Action::Type::CHECK};
                // }
                return {Action::Type::FOLD};
            case 1:
                cur_street = 3;
                update_equities = true;
                std::cout << "updating my range as call on table " << preflop_raises << " and opponents raise as "
                          << preflop_raises - 1 << std::endl;
                myHandIndex = update_ranges_preflop(myRange, preflop_raises, 1,
                                                    std::max(myHand.cards[0], myHand.cards[1]),
                                                    std::min(myHand.cards[0], myHand.cards[1]));
                update_ranges_preflop(notMyRange, preflop_raises - 1, 2, NULL_CARD, NULL_CARD);

                return {Action::Type::CALL};
            case 2:
                preflop_raises++;
                return {Action::Type::RAISE, preflop_raise_sizes[preflop_raises - 1]};
            }
        }

        if (roundState->auction) {
            float multiplier = 2.2;
            std::cout << "my hand is2 ";
            Hand hand = myRange[myHandIndex];
            for(int i = 0; i < 3; i++) {
                std::cout << hand.cards[i] << " ";
            }
            hand_draws(hand, board);
            if(hand.flush_draw == 2) {
                multiplier += 0.01;
            }
            if(hand.flush_draw == 3) {
                std::cout << "hellohi" << std::endl;
                multiplier += 0.2;
            }
            if(hand.flush_draw == 4) {
                multiplier += 0.7;
            }
            if(hand.flush_draw == 5) {
                multiplier -= 0.5;
            }if(hand.straight_draw == 2) {
                multiplier += 0.01;
            }if(hand.straight_draw == 3) {
                multiplier += 0.3;
            }if(hand.straight_draw == 4) {
                multiplier += 0.5;
            }if(hand.straight_draw == 5) {
                multiplier -= 0.5;
            }
            int bidSize = pot * multiplier + 0.5;
            return {Action::Type::BID, std::min(bidSize, myStack)};
        }

        // for handling when we bring the next round
        if (update_equities) {
            numRaises = !bigBlind - 1;
            if (street == 3) {
                for (int i = 0; i < 3; i++) {
                    board.push_back(make_card(Suit(char_to_suit[boardCards[i][1]]),
                                              Rank(char_to_rank[boardCards[i][0]])));
                }

                if (myBid.value() >= oppBid.value()) {
                    // won auction
                    tempgwin += (int)myBid.value();
                    for (int i = 0; i < myRange.size(); i++) {
                        myRange[i].isMyHand = false;
                    }
                    update_auction(myRange,board);
                    Hand newHand;
                    newHand.isMyHand = true;
                    for (int i = 0; i < 3; i++) {
                        newHand.cards[i] = make_card(Suit(char_to_suit[myCards[i][1]]),
                                                     Rank(char_to_rank[myCards[i][0]]));
                    }
                    myRange.push_back(newHand);
                    myHandIndex = myRange.size() - 1;
                }
                if (myBid.value() <= oppBid.value()){
                    update_auction(notMyRange,board);
                }
            } else {
                board.push_back(make_card(Suit(char_to_suit[boardCards[street - 1][1]]),
                                          Rank(char_to_rank[boardCards[street - 1][0]])));
            }

            // TODO : remove blockers
            for (int i = 0; i < myRange.size(); i++) {
                for (int j = 0; j < 3; j++) {
                    if (myRange[i].cards[j] > NULL_CARD) {
                    }
                }
            }
            update_strengths(myRange, board);
            update_strengths(notMyRange, board);
            for (int i = 0; i < myRange.size(); i++) {
                for (int j = 0; j < 3; j++) {
                    if (myRange[i].cards[j] > NULL_CARD) {
                    }
                }
            }
            guess_equities(myRange, board);
            guess_equities(notMyRange, board);
            update_equities = false;
        }

        // update opponents range here, it means they called/checked, update hand equities and draw potential
        if (cur_street != street) {
            numRaises = !bigBlind - 1;
            cur_street = street;
            oppCanCheck = true;
            if (cur_street == 3) {
                std::cout << "updating my range as raise on table " << preflop_raises - 1 << " and opponents call as "
                          << preflop_raises << std::endl;
                myHandIndex = update_ranges_preflop(myRange, preflop_raises - 1, 2,
                                                    std::max(myHand.cards[0], myHand.cards[1]),
                                                    std::min(myHand.cards[0], myHand.cards[1]));

                // for (int i = 0; i < myRange.size(); i++){
                //     std::cout << myRange[i].cards[0] << " " << myRange[i].cards[1] << std::endl;
                // }
                // std::cout << myRange.size() << std::endl;
                // assert (myHandIndex != -1);
                update_ranges_preflop(notMyRange, preflop_raises, 1, NULL_CARD, NULL_CARD);
                for (int i = 0; i < 3; i++) {
                    board.push_back(make_card(Suit(char_to_suit[boardCards[i][1]]),
                                              Rank(char_to_rank[boardCards[i][0]])));
                }
                if (myBid.value() >= oppBid.value()) {
                    // won auction
                    tempgwin += myBid.value();

                    for (int i = 0; i < myRange.size(); i++) {
                        myRange[i].isMyHand = false;
                    }
                    update_auction(myRange,board);
                    Hand newHand;
                    newHand.isMyHand = true;
                    for (int i = 0; i < 3; i++) {
                        newHand.cards[i] = make_card(Suit(char_to_suit[myCards[i][1]]),
                                                     Rank(char_to_rank[myCards[i][0]]));
                    }
                    myRange.push_back(newHand);
                    myHandIndex = myRange.size() - 1;
                } 
                if (myBid.value() <= oppBid.value()) {
                    update_auction(notMyRange,board);
                }
            }
            if (cur_street == 4) {
                if (!isPanic) {
                    std::cout << "CALCULATING OPPONENTS PREVIOUS ACTION" << std::endl;
                    update_strat(notMyRange, myRange, pot, oppmdf, oppCanCheck, bigBlind, cur_street - 3, tempgwin);
                    std::cout << "UPDATING OPPONENTS RANGE WITH ACTION " << (oppCanCheck ? CHECK : CALL) << std::endl;
                    update_ranges(notMyRange, (oppCanCheck ? CHECK : CALL), (oppCanCheck ? CHECK : CALL));
                }
                if (notMyRange.size() == 0) {
                    isPanic = true;
                }
                board.push_back(make_card(Suit(char_to_suit[boardCards[3][1]]), Rank(char_to_rank[boardCards[3][0]])));
            }
            if (cur_street == 5) {
                if (!isPanic) {
                    std::cout << "CALCULATING OPPONENTS PREVIOUS ACTION" << std::endl;
                    update_strat(notMyRange, myRange, pot, oppmdf, oppCanCheck, bigBlind, cur_street - 3, tempgwin);
                    std::cout << "UPDATING OPPONENTS RANGE WITH ACTION " << (oppCanCheck ? CHECK : CALL) << std::endl;
                    update_ranges(notMyRange, (oppCanCheck ? CHECK : CALL), (oppCanCheck ? CHECK : CALL));
                }
                if (notMyRange.size() == 0) {
                    isPanic = true;
                }
                board.push_back(make_card(Suit(char_to_suit[boardCards[4][1]]), Rank(char_to_rank[boardCards[4][0]])));
            }
            update_strengths(myRange, board);
            update_strengths(notMyRange, board);
            guess_equities(myRange, board);
            guess_equities(notMyRange, board);
        }

        numRaises++;

        // Handles opponent's actions
        if (numRaises > 0 && !isPanic) {
            std::cout << "CALCULATING OPPONENTS PREVIOUS ACTION" << std::endl;
            update_strat(notMyRange, myRange, pot, oppmdf, oppCanCheck, bigBlind, street - 3, tempgwin);
            float sizing = (float)(oppPip - myPip) / (pot - oppPip);

            action action1;
            action action2;

            // action translation
            if (sizing < .2) {
                action1 = oppCanCheck ? CHECK : CALL;
                action2 = RAISE33;
            } else if (sizing < .4) {
                action1 = RAISE33;
                action2 = RAISE50;
            } else if (sizing < .75) {
                action1 = RAISE50;
                action2 = RAISE100;
            } else {
                action1 = RAISE100;
                action2 = RAISE100;
            }
            std::cout << "i think my opponent did " << action1 << std::endl;
            std::cout << "UPDATING OPPONENTS RANGE WITH ACTION " << action1 << std::endl;
            update_ranges(notMyRange, action1, action2);
        }

        if (notMyRange.size() == 0) {
            isPanic = true;
        }

        // Calculate strategy, pick out strategy for hand, update my own range, return action.
        if (!isPanic) {
            std::cout << "CALCULATING MY NEXT ACTION" << std::endl;
            update_strat(myRange, notMyRange, pot, mdf, continueCost == 0, !bigBlind, street - 3, tempgwin);
        } else {
            std::cout << "PANICKING" << std::endl;
            panic(myRange, mdf, continueCost == 0);
        }
        int ct[NUMACTIONS + 1] = {};
        for (int i = 0; i < myRange.size(); i++) {
            ct[myRange[i].strategy] += 1;
        }
        std::cout << "strategy: ";
        for (int i = 0; i < NUMACTIONS + 1; i++) {
            std::cout << ct[i] << " ";
        }
        action nextAction = myRange[myHandIndex].strategy;
        std::cout << " my hand index + range size" << myHandIndex << " " << myRange.size() << std::endl;
        std::cout << "my hand is ";
        for (int i = 0; i < 3; i++) {
            std::cout << myRange[myHandIndex].cards[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "UPDATING MY RANGE WITH ACTION " << nextAction << std::endl;
        myHandIndex = update_ranges(myRange, nextAction, nextAction);
        // std::cout << myRange[myHandIndex].equity << " my equity " << std::endl;
        std::cout << "my action is " << nextAction << std::endl;
        if (nextAction == FOLD) {
            return {Action::Type::FOLD};
        } else if (nextAction == CALL) {
            cur_street++;
            update_equities = true;
            oppCanCheck = true;
            tempgwin += continueCost;
            return {Action::Type::CALL};
        } else if (nextAction == CHECK) {
            if (!bigBlind) {
                cur_street++;
                update_equities = true;
            }
            oppCanCheck = true;
            oppmdf = 0.0;
            return {Action::Type::CHECK};
        } else if (nextAction == RAISEGWIN) {
            int sizing = tempgwin - pot;
            oppmdf = 1 / (1 + raise_sizes[nextAction - RAISE33]);
            oppCanCheck = false;
            tempgwin += std::min(sizing, myPip + maxCost) - myPip;
            return {Action::Type::RAISE, std::min(sizing, myPip + maxCost)};
        } else {
            int sizing = myPip + continueCost + (continueCost + pot) * raise_sizes[nextAction - RAISE33] + 0.5;
            std::cout << "continue cost " << continueCost << std::endl;
            std::cout << "attempting to bet " << sizing << std::endl;
            oppmdf = 1 / (1 + raise_sizes[nextAction - RAISE33]);
            oppCanCheck = false;
            tempgwin += std::min(sizing, myPip + maxCost) - myPip;
            return {Action::Type::RAISE, std::min(sizing, myPip + maxCost)};
        }
    }
};

/*
  Main program for running a C++ pokerbot.
*/
int main(int argc, char* argv[]) {
    for (int i = 0; i < rank_to_char.size(); i++) {
        char_to_rank[rank_to_char[i]] = i;
    }
    for (int i = 0; i < suit_to_char.size(); i++) {
        char_to_suit[suit_to_char[i]] = i;
    }
    auto [host, port] = parseArgs(argc, argv);
    runBot<Bot>(host, port);
    return 0;
}
