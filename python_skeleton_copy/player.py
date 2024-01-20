'''
Simple example pokerbot, written in Python.
'''
from skeleton.actions import FoldAction, CallAction, CheckAction, RaiseAction, BidAction
from skeleton.states import GameState, TerminalState, RoundState
from skeleton.states import NUM_ROUNDS, STARTING_STACK, BIG_BLIND, SMALL_BLIND
from skeleton.bot import Bot
from skeleton.runner import parse_args, run_bot
import random
import eval7
import time
import pickle


class Player(Bot):
    '''
    A pokerbot.
    '''

    def __init__(self):
        '''
        Called when a new game starts. Called exactly once.

        Arguments:
        Nothing.

        Returns:
        Nothing.
        '''
        self.opp_holes = []
        self.opp_bids = []

        prev_time = time.time()
        with open("hand_strengths", "rb") as file:
            self.starting_strengths = pickle.load(file)
        print("pickle load:", time.time() - prev_time)
        
        rank_to_numeric = dict()

        for i in range(2,10):
            rank_to_numeric[str(i)] = i

        for num, rank in enumerate("TJQKA"): #[(0,T), (1,J), (2,Q) ...]
            rank_to_numeric[rank] = num + 10

        self.rank_to_numeric = rank_to_numeric

    def handle_new_round(self, game_state, round_state, active):
        '''
        Called when a new round starts. Called NUM_ROUNDS times.

        Arguments:
        game_state: the GameState object.
        round_state: the RoundState object.
        active: your player's index.

        Returns:
        Nothing.
        '''
        my_bankroll = game_state.bankroll  # the total number of chips you've gained or lost from the beginning of the game to the start of this round
        game_clock = game_state.game_clock  # the total number of seconds your bot has left to play this game
        round_num = game_state.round_num  # the round number from 1 to NUM_ROUNDS
        my_cards = round_state.hands[active]  # your cards
        big_blind = bool(active)  # True if you are the big blind
        pass
        print('Round '+str(round_num))
        card1 = my_cards[0]
        card2 = my_cards[1]

        rank1 = card1[0]
        rank2 = card2[0]
        suit1 = card1[1]
        suit2 = card2[1]

        self.strong_hole = False
        if rank1 == rank2 or (rank1 in "AKQJT" and rank2 in "AKQJT"):
            self.strong_hole = True

        game_clock = game_state.game_clock

        monte_carlo_iters = 100
        strength_w_auction, strength_wo_auction = self.calculate_strength(my_cards, monte_carlo_iters)
        self.strength_w_auction = strength_w_auction
        self.strength_wo_auction = strength_wo_auction

        if round_num == NUM_ROUNDS:
            print('game clock: ' + str(game_clock))

    def hand_to_strength(self, my_cards):
        card_1 = my_cards[0]
        card_2 = my_cards[1]
        #print(card_1)

        #rank_1, suit_1 = card_1
        rank_1 = str(card_1)[0]
        suit_1 = str(card_1)[1]
        #rank_2, suit_2 = card_2
        rank_2 = str(card_2)[0]
        suit_2 = str(card_2)[1]

        #print(rank_1)
        #print(suit_1)

        num_1 = self.rank_to_numeric[str(rank_1)]
        num_2 = self.rank_to_numeric[str(rank_2)]

        suited = 'o'
        if suit_1 == suit_2:
            suited = "s"

        if num_1 >= num_2:
            key = rank_1 + rank_2 + suited
        else:
            key = rank_2 + rank_1 + suited

        return self.starting_strengths[key]

    def calculate_strength(self, my_cards, iters):
        deck = eval7.Deck()
        my_cards = [eval7.Card(card) for card in my_cards]
        for card in my_cards:
            deck.cards.remove(card)
        wins_w_auction = 0
        wins_wo_auction = 0

        '''for i in range(iters):
            deck.shuffle()
            opp = 3
            community = 5
            draw = deck.peek(opp + community)
            opp_hole = draw[:opp]
            community_cards = draw[opp:]

            my_hand = my_cards + community_cards
            opp_hand = opp_hole + community_cards

            my_hand_val = eval7.evaluate(my_hand)
            opp_hand_val = eval7.evaluate(opp_hand)

            if my_hand_val > opp_hand_val:
                wins_wo_auction += 1
            if my_hand_val == opp_hand_val:
                wins_wo_auction += 0.5

        for i in range(iters):
            deck.shuffle()
            opp = 2
            community = 5
            auction = 1
            draw = deck.peek(opp + community+auction)
            opp_hole = draw[:opp]
            community_cards = draw[opp:opp+community]
            auction_card = draw[opp+community:]

            my_hand = my_cards + community_cards + auction_card
            opp_hand = opp_hole + community_cards

            my_hand_val = eval7.evaluate(my_hand)
            opp_hand_val = eval7.evaluate(opp_hand)

            if my_hand_val > opp_hand_val:
                wins_w_auction += 1
            if my_hand_val == opp_hand_val:
                wins_w_auction += 0.5

        strength_w_auction = wins_w_auction/iters
        strength_wo_auction = wins_wo_auction/iters'''
        strength_w_auction = self.hand_to_strength(my_cards)[0]
        strength_wo_auction = self.hand_to_strength(my_cards)[1]
        #print(str(strength_w_auction)+", "+str(strength_wo_auction))
        return strength_w_auction, strength_wo_auction
    
    def hand_rank(self, my_cards, board_cards):
        my_val = eval7.evaluate(my_cards+board_cards)
        #print(my_cards+board_cards)
        possible_vals = []
        cards = []
        for rank in 'AKQJT98765432':
            cards.append(rank+'s')
            cards.append(rank+'c')
            cards.append(rank+'h')
            cards.append(rank+'d')
        for i in range(len(cards)):
            for j in range(i+1, len(cards)):
                possible_hand = [eval7.Card(cards[i]), eval7.Card(cards[j])]
                val = int(eval7.evaluate(possible_hand + board_cards))
                possible_vals.append(val)
                #print(str(possible_hand)+' '+str(val))
        '''for i in range(len(cards)):
            for j in range(i+1, len(cards)):
                for k in range(j+1, len(cards)):
                    possible_hand = [eval7.Card(cards[i]), eval7.Card(cards[j]), eval7.Card(cards[k])]
                    possible_vals.append(eval7.evaluate(possible_hand + board_cards))'''
        possible_vals.sort()
        #print(possible_vals)
        if my_val in possible_vals:
            rank = possible_vals.index(my_val)/len(possible_vals)
        else:
            for i in range(len(possible_vals)):
                if possible_vals[i] >= my_val:
                    rank = i/len(possible_vals)
                    break
        return rank

    def handle_round_over(self, game_state, terminal_state, active):
        '''
        Called when a round ends. Called NUM_ROUNDS times.

        Arguments:
        game_state: the GameState object.
        terminal_state: the TerminalState object.
        active: your player's index.

        Returns:
        Nothing.
        '''
        my_delta = terminal_state.deltas[active]  # your bankroll change from this round
        previous_state = terminal_state.previous_state  # RoundState before payoffs
        street = previous_state.street  # 0, 3, 4, or 5 representing when this round ended
        my_cards = previous_state.hands[active]  # your cards
        opp_cards = previous_state.hands[1-active]  # opponent's cards or [] if not revealed
        pass
        print('-------------')
    
    def flush_draws(self, my_cards, board): 
        draws = []
        my_suits = [str(card)[1] for card in my_cards]
        board_suits = [str(card)[1] for card in board]
        total_suits = my_suits + board_suits
        
        if total_suits.count('s') >= 3 and board_suits.count('s') != total_suits.count('s'):
            if total_suits.count('s') == 3:
                draws.append(('s', 3))
            if total_suits.count('s') == 4:
                draws.append(('s', 4))
        if total_suits.count('c') >= 3 and board_suits.count('c') != total_suits.count('c'):
            if total_suits.count('c') == 3:
                draws.append(('c', 3))
            if total_suits.count('c') == 4:
                draws.append(('c', 4))
        if total_suits.count('h') >= 3 and board_suits.count('h') != total_suits.count('h'):
            if total_suits.count('h') == 3:
                draws.append(('h', 3))
            if total_suits.count('h') == 4:
                draws.append(('h', 4))
        if total_suits.count('d') >= 3 and board_suits.count('d') != total_suits.count('d'):
            if total_suits.count('d') == 3:
                draws.append(('d', 3))
            if total_suits.count('d') == 4:
                draws.append(('d', 4))
        
        return (draws)
    
    def straight_draws(self, my_cards, board):
        #return possible missing cards
        draws = []
        my_ranks = [str(card)[0] for card in my_cards]
        board_ranks = [str(card)[0] for card in board]
        total_ranks = my_ranks + board_ranks
        my_set = set(my_ranks)
        board_set = set(board_ranks)
        total_set = set(total_ranks)

        ranks = '23456789TJQKA'
        for rank in ranks:
            possible_hand = []
            possible_hand.append(rank + 's')
            for my_rank in my_set:
                possible_hand.append(my_rank + 'c')
            for board_rank in board_set:
                possible_hand.append(board_rank + 'h')
            hand = [eval7.Card(card) for card in possible_hand]
            strength = eval7.evaluate(hand)
            hand_type = eval7.handtype(strength)
            if hand_type == 'Straight':
                draws.append(rank)

        return draws


        return draws

    def get_action(self, game_state, round_state, active):
        '''
        Where the magic happens - your code should implement this function.
        Called any time the engine needs an action from your bot.

        Arguments:
        game_state: the GameState object.
        round_state: the RoundState object.
        active: your player's index.

        Returns:
        Your action.
        '''
        # May be useful, but you may choose to not use.
        my_bankroll = game_state.bankroll
        round_num = game_state.round_num
        legal_actions = round_state.legal_actions()  # the actions you are allowed to take
        street = round_state.street  # 0, 3, 4, or 5 representing pre-flop, flop, turn, or river respectively
        my_cards = round_state.hands[active]  # your cards
        board_cards = round_state.deck[:street]  # the board cards
        my_pip = round_state.pips[active]  # the number of chips you have contributed to the pot this round of betting
        opp_pip = round_state.pips[1-active]  # the number of chips your opponent has contributed to the pot this round of betting
        my_stack = round_state.stacks[active]  # the number of chips you have remaining
        opp_stack = round_state.stacks[1-active]  # the number of chips your opponent has remaining
        my_bid = round_state.bids[active]  # How much you bid previously (available only after auction)
        opp_bid = round_state.bids[1-active]  # How much opponent bid previously (available only after auction)
        continue_cost = opp_pip - my_pip  # the number of chips needed to stay in the pot
        my_contribution = STARTING_STACK - my_stack  # the number of chips you have contributed to the pot
        opp_contribution = STARTING_STACK - opp_stack  # the number of chips your opponent has contributed to the pot
        pot = my_contribution + opp_contribution
        hand_arr = my_cards + board_cards
        my_hand = [eval7.Card(card) for card in hand_arr]
        board = [eval7.Card(card) for card in board_cards]
        strength = eval7.evaluate(my_hand)
        board_strength = eval7.evaluate(board)
        board_type = eval7.handtype(board_strength)
        hand_type = eval7.handtype(strength)
        pot_odds = continue_cost/(continue_cost + pot)
        guaranteed_win = False
        #my_cards = [eval7.Card(card) for card in my_cards]
        print(str(hand_arr) + hand_type)

        if RaiseAction in legal_actions:
           min_raise, max_raise = round_state.raise_bounds()  # the smallest and largest numbers of chips for a legal bet/raise
           min_cost = min_raise - my_pip  # the cost of a minimum bet/raise
           max_cost = max_raise - my_pip  # the cost of a maximum bet/raise

        rank = self.hand_rank([eval7.Card(card) for card in my_cards], board)
        if street > 0:
            print('rank: '+str(rank))

        if 1.5*(1000-round_num+1) + 1 < my_bankroll:
            guaranteed_win = True
        if guaranteed_win:
            #keep check/folding if we have enough already to win
            if FoldAction in legal_actions:
                return FoldAction()
            if BidAction in legal_actions:
                return BidAction(0)
            if CheckAction in legal_actions:
                return CheckAction()
            
        if street == 0:
            #preflop
            strength = self.hand_to_strength(my_cards)
            avg_strength = 0.5*(strength[0] + strength[1])
            print('strength: '+ str(strength) + ', average: '+ str(avg_strength))
            action = 0 #fold=0, open=1, call 3bet = 2, 3bet=3, 4bet=4
            if avg_strength > 0.47:
                action = 1
            if avg_strength > 0.53:
                action = 2
            if avg_strength > 0.6:
                action = 3
            if avg_strength > 0.73:
                action = 4

            if action == 0:
                print('fold')
            if action == 1:
                print('open, fold to 3bet')
            if action == 2:
                print('call 3bet')
            if action == 3:
                print('3bet')
            if action == 4:
                print('4bet')

            if action == 0:
                if FoldAction in legal_actions:
                    return FoldAction()
                return CheckAction()
            if my_contribution == 1:
                return RaiseAction(10)
            if my_contribution == 2:
                if CallAction in legal_actions:
                    #opponent bet
                    if action == 1 or action == 2:
                        return CallAction()
                    return RaiseAction(0.5*(min_raise + max_raise))
                if CheckAction in legal_actions:
                    #opponent limped
                    return RaiseAction(20)
            if CallAction in legal_actions and action == 2:
                return CallAction()
            if FoldAction in legal_actions and action == 1:
                return FoldAction()
            if RaiseAction in legal_actions and action >= 3:
                return RaiseAction(0.8*min_raise + 0.2*max_raise)
        
        if street < 5:
            flush_draws = self.flush_draws(my_cards, board)
            straight_draws = self.straight_draws(my_cards, board)
            if len(flush_draws) > 0:
                print('flush draws: ' + str(flush_draws))
            if len(straight_draws) > 0:
                print('cards needed for straight: ' + str(straight_draws))
                

        strength_diff = self.strength_w_auction - self.strength_wo_auction
        #print(self.strength_w_auction)
        if BidAction in legal_actions and self.strength_w_auction > 0.7:
            #all in on bid if hole is strong enough
            return BidAction(my_stack) 
        if BidAction in legal_actions:
            #bid
            max_bid = 0.50
            min_bid = 0.15
            bid = strength_diff
            bid = min(max(min_bid, bid), max_bid)
            bid = int(bid*my_stack)
            bid = min(bid, my_stack)
            return BidAction(bid)
        
        if FoldAction in legal_actions and self.strength_wo_auction<0.3 and street==0:
            #fold preflop
            return FoldAction()
        if FoldAction in legal_actions and street==0 and opp_contribution > 2:
            #fold preflop to opponent raise
            if self.strength_wo_auction<0.35:
                return FoldAction()
        if FoldAction in legal_actions and street==3:
            #fold on flop if pot odds too high if we have nothing
            if hand_type == 'High Card' and pot_odds > 0.3:
                return FoldAction()
            if hand_type == board_type and pot_odds > 0.3:
                return FoldAction()
        if FoldAction in legal_actions and street==4:
            #fold on turn
            if hand_type == 'High Card' and pot_odds > 0.15:
                return FoldAction()
            if hand_type == board_type and pot_odds > 0.15:
                return FoldAction()
            if hand_type == 'Pair' and pot_odds > 0.25:
                return FoldAction()
        if FoldAction in legal_actions and street==5:
            #fold on river if we have nothing
            if hand_type == 'High Card' and pot_odds>0.1:
                return FoldAction()
            if hand_type == board_type and pot_odds>0.1:
                return FoldAction()
            if hand_type == 'Pair' and pot_odds>0.3:
                return FoldAction()
            if hand_type == 'Two Pair' and board_type == 'Pair' and pot_odds > 0.3:
                return FoldAction()
  
        if RaiseAction in legal_actions and street==0:
            #min raise preflop but don't keep going infinitely
            if self.strength_wo_auction>0.45 and my_contribution < 200:
                return RaiseAction(min_raise)
        #if RaiseAction in legal_actions and street==3 and len(my_cards) == 3 and self.strength_w_auction > 0.65:
            #raise if we got auction
            #return RaiseAction(max(min_raise, int(0.5*max_raise)))
        if RaiseAction in legal_actions and street==3:
            #raise if we have something on the flop
            if hand_type != 'High Card' and hand_type != board_type:
                raise_amt = 0.5*pot
                raise_amt = min(raise_amt, max_raise)
                raise_amt = max(raise_amt, min_raise)
                return RaiseAction(int(raise_amt))
        if RaiseAction in legal_actions:
            #max raise if strong hand
            raise_amt = 0.5*self.strength_wo_auction*pot
            raise_amt = max(min(raise_amt, max_raise), min_raise)
            if hand_type != 'High Card' and hand_type != 'Pair' and board_type == 'High Card':
                return RaiseAction(max_raise)
            
        if CheckAction in legal_actions:
            return CheckAction()
        return CallAction()


if __name__ == '__main__':
    run_bot(Player(), parse_args())
