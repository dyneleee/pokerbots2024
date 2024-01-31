// #include <iostream>
//
// #include "equity.h"
// #include "equityguesser.h"
//
// int main() {
//     Hand hand;
//     hand.cards[0] = FIVE_SPADES;
//     hand.cards[1] = ACE_DIAMONDS;
//     hand.cards[2] = ACE_CLUBS;
//     std::vector<Hand> range;
//     for (int i = TEN_HEARTS; i < ACE_HEARTS; i++) {
//         for (int j = i + 1; j < ACE_HEARTS; j++) {
//             Hand temp;
//             temp.cards[0] = Card(i);
//             temp.cards[1] = Card(j);
//             temp.cards[2] = NULL_CARD;
//             range.push_back(temp);
//         }
//     }
//     std::cout << range.size() << std::endl;
//     std::vector<Card> board;
//     board.push_back(SIX_SPADES);
//     board.push_back(SEVEN_SPADES);
//     board.push_back(EIGHT_SPADES);
//     // get_rvr_equity(range, range, board);
//     // std::cout << "Equity: " << range[0].equity << std::endl;
//
//     std::vector<Hand> range2;
//     range2.push_back(hand);
//     guess_equities(range2, board);
//     std::cout << "Equity: " << range2[0].flush_draw << " " << range2[0].straight_draw << " " << range2[0].low_pair
//               << std::endl;
//     guess_equities(range, board);
//     for (Hand& hand : range) {
//         std::cout << get_rank(hand.cards[0]) << " " << get_rank(hand.cards[1]) << " " << hand.flush_draw << " " << hand.straight_draw << " " << hand.low_pair << std::endl;
//     }
//     guess_draw_potential(range, range2, 5 - board.size());
//     std::cout << range2[0].draw_potential << std::endl;
// }
