package javabot;

import javabot.skeleton.Action;
import javabot.skeleton.ActionType;
import javabot.skeleton.GameState;
import javabot.skeleton.State;
import javabot.skeleton.TerminalState;
import javabot.skeleton.RoundState;
import javabot.skeleton.Bot;
import javabot.skeleton.Runner;

import java.lang.Integer;
import java.lang.String;
import java.util.*;
import java.io.*;
import static java.util.Map.entry;   

/**
 * A pokerbot.
 */
public class Player implements Bot {
    // Your instance variables go here.
    public boolean guaranteedWin = false;
    public List<Double> oppBids = new ArrayList<Double>();
    public int auctionPot = 0;
    public int[] numBets = {1, 0, 0, 0, 0, 0}; // counts big blind as a preflop bet
    public double auctionStrength = 0;
    public double noAuctionStrength = 0;
    public HashMap<String, Double> avgStrengths = new HashMap<String, Double>();
    public double preflopStrength = 0;
    public boolean preflopFold = false;

    /**
     * Called when a new game starts. Called exactly once.
     */
    public Player() {
        avgStrengths.put("AKs", 0.621875);
        avgStrengths.put("AQs", 0.616925);
        avgStrengths.put("AJs", 0.61235);
        avgStrengths.put("ATs", 0.602225);
        avgStrengths.put("A9s", 0.5806);
        avgStrengths.put("A8s", 0.583975);
        avgStrengths.put("A7s", 0.5742499999999999);
        avgStrengths.put("A6s", 0.566925);
        avgStrengths.put("A5s", 0.5622750000000001);
        avgStrengths.put("A4s", 0.5599);
        avgStrengths.put("A3s", 0.548875);
        avgStrengths.put("A2s", 0.54505);
        avgStrengths.put("KQs", 0.600175);
        avgStrengths.put("KJs", 0.5924);
        avgStrengths.put("KTs", 0.589225);
        avgStrengths.put("K9s", 0.570875);
        avgStrengths.put("K8s", 0.55345);
        avgStrengths.put("K7s", 0.5527);
        avgStrengths.put("K6s", 0.54095);
        avgStrengths.put("K5s", 0.54285);
        avgStrengths.put("K4s", 0.531425);
        avgStrengths.put("K3s", 0.519225);
        avgStrengths.put("K2s", 0.5178);
        avgStrengths.put("QJs", 0.586425);
        avgStrengths.put("QTs", 0.575275);
        avgStrengths.put("Q9s", 0.5626500000000001);
        avgStrengths.put("Q8s", 0.5404);
        avgStrengths.put("Q7s", 0.533075);
        avgStrengths.put("Q6s", 0.527825);
        avgStrengths.put("Q5s", 0.5154);
        avgStrengths.put("Q4s", 0.51225);
        avgStrengths.put("Q3s", 0.50425);
        avgStrengths.put("Q2s", 0.494475);
        avgStrengths.put("JTs", 0.568275);
        avgStrengths.put("J9s", 0.55335);
        avgStrengths.put("J8s", 0.530175);
        avgStrengths.put("J7s", 0.518175);
        avgStrengths.put("J6s", 0.5100750000000001);
        avgStrengths.put("J5s", 0.50575);
        avgStrengths.put("J4s", 0.493025);
        avgStrengths.put("J3s", 0.48287499999999994);
        avgStrengths.put("J2s", 0.47535);
        avgStrengths.put("T9s", 0.54375);
        avgStrengths.put("T8s", 0.53285);
        avgStrengths.put("T7s", 0.51505);
        avgStrengths.put("T6s", 0.49640000000000006);
        avgStrengths.put("T5s", 0.486675);
        avgStrengths.put("T4s", 0.4789);
        avgStrengths.put("T3s", 0.469);
        avgStrengths.put("T2s", 0.46745);
        avgStrengths.put("98s", 0.5277499999999999);
        avgStrengths.put("97s", 0.50425);
        avgStrengths.put("96s", 0.49485);
        avgStrengths.put("95s", 0.484325);
        avgStrengths.put("94s", 0.4679);
        avgStrengths.put("93s", 0.465675);
        avgStrengths.put("92s", 0.45065);
        avgStrengths.put("87s", 0.505325);
        avgStrengths.put("86s", 0.49515);
        avgStrengths.put("85s", 0.48219999999999996);
        avgStrengths.put("84s", 0.467225);
        avgStrengths.put("83s", 0.448125);
        avgStrengths.put("82s", 0.44495);
        avgStrengths.put("76s", 0.485475);
        avgStrengths.put("75s", 0.481025);
        avgStrengths.put("74s", 0.462475);
        avgStrengths.put("73s", 0.44115);
        avgStrengths.put("72s", 0.4289);
        avgStrengths.put("65s", 0.47697500000000004);
        avgStrengths.put("64s", 0.46009999999999995);
        avgStrengths.put("63s", 0.44430000000000003);
        avgStrengths.put("62s", 0.427625);
        avgStrengths.put("54s", 0.46545000000000003);
        avgStrengths.put("53s", 0.44435);
        avgStrengths.put("52s", 0.4386);
        avgStrengths.put("43s", 0.441825);
        avgStrengths.put("42s", 0.426225);
        avgStrengths.put("32s", 0.41307499999999997);
        avgStrengths.put("AKo", 0.6025);
        avgStrengths.put("AQo", 0.593975);
        avgStrengths.put("AJo", 0.590025);
        avgStrengths.put("ATo", 0.5824);
        avgStrengths.put("A9o", 0.5621);
        avgStrengths.put("A8o", 0.562325);
        avgStrengths.put("A7o", 0.54805);
        avgStrengths.put("A6o", 0.53715);
        avgStrengths.put("A5o", 0.541075);
        avgStrengths.put("A4o", 0.529525);
        avgStrengths.put("A3o", 0.52295);
        avgStrengths.put("A2o", 0.5168);
        avgStrengths.put("KQo", 0.578075);
        avgStrengths.put("KJo", 0.5718);
        avgStrengths.put("KTo", 0.565675);
        avgStrengths.put("K9o", 0.5412);
        avgStrengths.put("K8o", 0.525975);
        avgStrengths.put("K7o", 0.525875);
        avgStrengths.put("K6o", 0.519925);
        avgStrengths.put("K5o", 0.5113);
        avgStrengths.put("K4o", 0.49340000000000006);
        avgStrengths.put("K3o", 0.49475);
        avgStrengths.put("K2o", 0.48475);
        avgStrengths.put("QJo", 0.5577);
        avgStrengths.put("QTo", 0.549725);
        avgStrengths.put("Q9o", 0.53725);
        avgStrengths.put("Q8o", 0.512475);
        avgStrengths.put("Q7o", 0.49302500000000005);
        avgStrengths.put("Q6o", 0.49515);
        avgStrengths.put("Q5o", 0.49155000000000004);
        avgStrengths.put("Q4o", 0.48382500000000006);
        avgStrengths.put("Q3o", 0.471);
        avgStrengths.put("Q2o", 0.461075);
        avgStrengths.put("JTo", 0.54745);
        avgStrengths.put("J9o", 0.5254);
        avgStrengths.put("J8o", 0.515575);
        avgStrengths.put("J7o", 0.49577499999999997);
        avgStrengths.put("J6o", 0.474925);
        avgStrengths.put("J5o", 0.4732);
        avgStrengths.put("J4o", 0.4666);
        avgStrengths.put("J3o", 0.457575);
        avgStrengths.put("J2o", 0.4485);
        avgStrengths.put("T9o", 0.52415);
        avgStrengths.put("T8o", 0.5022249999999999);
        avgStrengths.put("T7o", 0.493);
        avgStrengths.put("T6o", 0.473475);
        avgStrengths.put("T5o", 0.4528);
        avgStrengths.put("T4o", 0.4518);
        avgStrengths.put("T3o", 0.447925);
        avgStrengths.put("T2o", 0.43167500000000003);
        avgStrengths.put("98o", 0.494575);
        avgStrengths.put("97o", 0.47985);
        avgStrengths.put("96o", 0.46765);
        avgStrengths.put("95o", 0.45210000000000006);
        avgStrengths.put("94o", 0.436875);
        avgStrengths.put("93o", 0.425475);
        avgStrengths.put("92o", 0.41385);
        avgStrengths.put("87o", 0.4808);
        avgStrengths.put("86o", 0.46304999999999996);
        avgStrengths.put("85o", 0.45285);
        avgStrengths.put("84o", 0.43727499999999997);
        avgStrengths.put("83o", 0.413075);
        avgStrengths.put("82o", 0.4033);
        avgStrengths.put("76o", 0.46585);
        avgStrengths.put("75o", 0.45035);
        avgStrengths.put("74o", 0.432075);
        avgStrengths.put("73o", 0.411725);
        avgStrengths.put("72o", 0.389);
        avgStrengths.put("65o", 0.449575);
        avgStrengths.put("64o", 0.4333);
        avgStrengths.put("63o", 0.41350000000000003);
        avgStrengths.put("62o", 0.39375);
        avgStrengths.put("54o", 0.437975);
        avgStrengths.put("53o", 0.41395000000000004);
        avgStrengths.put("52o", 0.39785);
        avgStrengths.put("43o", 0.40237500000000004);
        avgStrengths.put("42o", 0.387625);
        avgStrengths.put("32o", 0.381525);
        avgStrengths.put("AAo", 0.786125);
        avgStrengths.put("KKo", 0.755375);
        avgStrengths.put("QQo", 0.7379);
        avgStrengths.put("JJo", 0.716225);
        avgStrengths.put("TTo", 0.6899500000000001);
        avgStrengths.put("99o", 0.666275);
        avgStrengths.put("88o", 0.646225);
        avgStrengths.put("77o", 0.614125);
        avgStrengths.put("66o", 0.58665);
        avgStrengths.put("55o", 0.5687500000000001);
        avgStrengths.put("44o", 0.5385);
        avgStrengths.put("33o", 0.505775);
        avgStrengths.put("22o", 0.482675);
    }

    /**
     * Called when a new round starts. Called State.NUM_ROUNDS times.
     *
     * @param gameState The GameState object.
     * @param roundState The RoundState object.
     * @param active Your player's index.
     */

    public int rankNumeric(char rank){
        int num = 0;
        if(rank == 'A'){
            num = 12;
        }
        else if(rank == 'K'){
            num = 11;
        }
        else if(rank == 'Q'){
            num = 10;
        }
        else if(rank == 'J'){
            num = 9;
        }
        else if(rank == 'T'){
            num = 8;
        }
        else{
            num = rank - 50;
        }
        return num;
    }

    public double handStrength(List<String> myCards){
        double strength = 0;
        char rank1 = myCards.get(0).charAt(0);
        char rank2 = myCards.get(1).charAt(0);
        int numRank1 = rankNumeric(rank1);
        int numRank2 = rankNumeric(rank2);
        char suit1 = myCards.get(0).charAt(1);
        char suit2 = myCards.get(1).charAt(1);
        String key = "";

        if(numRank1 > numRank2){
            key += rank1 + "" + rank2;
        }
        else{
            key += rank2 + "" + rank1;
        }
        if(suit1 == suit2){
            key += "s";
        }
        else{
            key += "o";
        }
        strength = this.avgStrengths.get(key);
        System.out.println(key + " " + strength);
        return strength;
    }    

    public Card toCard(String card){
        int rank = this.rankNumeric(card.charAt(0));
        char suit = card.charAt(1);
        int numSuit = 0;
        if(suit == 's'){
            numSuit = Card.SPADES;
        }
        if(suit == 'c'){
            numSuit = Card.CLUBS;
        }
        if(suit == 'h'){
            numSuit = Card.HEARTS;
        }
        if(suit == 'd'){
            numSuit = Card.DIAMONDS;
        }
        return new Card(rank, numSuit);
    }

    public double handRank(List<String> myCards, List<String> boardCards){
        int wins = 0;
        int totalHands = 0;
        Hand hand = new Hand();
        Card[] newMyCards = new Card[myCards.size()];
        Card[] newBoardCards = new Card[boardCards.size()];
        Card[] myHand = new Card[myCards.size()+boardCards.size()];
        for(int i = 0; i<newMyCards.length; i++){
            Card newCard = toCard(myCards.get(i));
            newMyCards[i] = newCard;
            myHand[i] = newCard;
        }
        for(int i = 0; i<newBoardCards.length; i++){
            Card newCard = toCard(boardCards.get(i));
            newBoardCards[i] = newCard;
            myHand[myCards.size()+i] = newCard;
        }
        //System.out.println("myHand: "+Arrays.toString(myHand));
        List<String> deck = new ArrayList<String>();
        String ranks = "AKQJT98765432";
        for(int i = 0; i < ranks.length(); i++){
            deck.add(ranks.charAt(i)+"s");
            deck.add(ranks.charAt(i)+"c");
            deck.add(ranks.charAt(i)+"h");
            deck.add(ranks.charAt(i)+"d");
        }
        for(int i = 0; i < myCards.size(); i++){
            deck.remove(myCards.get(i));
        }
        for(int i = 0; i < boardCards.size(); i++){
            deck.remove(boardCards.get(i));
        }
        for(int i = 0; i < deck.size(); i++){
            for(int j = i+1; j<deck.size(); j++){
                totalHands++;
                Card[] oppCards = {toCard(deck.get(i)), toCard(deck.get(j))};
                Card[] oppHand = new Card[2 + boardCards.size()];
                oppHand[0] = oppCards[0];
                oppHand[1] = oppCards[1];
                for(int k = 0; k < boardCards.size(); k++){
                    oppHand[2+k] = newBoardCards[k];
                }
                int val = hand.battleHands(myHand, oppHand);
                //System.out.println(Arrays.toString(oppHand)+" val: "+val);
                if(val == 1){
                    wins += 1;
                }
                else if(val == 0){
                    wins += 0.5;
                }
            }
        }
        //jeval7
        //System.out.println("wins: "+wins);
        return (double)wins/totalHands;
    }
     
    public void handleNewRound(GameState gameState, RoundState roundState, int active) {
        int myBankroll = gameState.bankroll;  // the total number of chips you've gained or lost from the beginning of the game to the start of this round
        float gameClock = gameState.gameClock;  // the total number of seconds your bot has left to play this game
        int roundNum = gameState.roundNum;  // the round number from 1 to State.NUM_ROUNDS
        List<String> myCards = roundState.hands.get(active);  // your cards
        boolean bigBlind = (active == 1);  // true if you are the big blind
        this.auctionPot = 4;
        this.preflopStrength = handStrength(myCards);
        this.numBets[0] = 1;
        this.numBets[3] = 0;
        this.numBets[4] = 0;
        this.numBets[5] = 0;

        System.out.println("Round "+roundNum);
        if(roundNum == 1000){
            System.out.println("Clock: "+gameClock);
        }
    }

    /**
     * Called when a round ends. Called State.NUM_ROUNDS times.
     *
     * @param gameState The GameState object.
     * @param terminalState The TerminalState object.
     * @param active Your player's index.
     */
    public void handleRoundOver(GameState gameState, TerminalState terminalState, int active) { //done
        int myDelta = terminalState.deltas.get(active);  // your bankroll change from this round
        RoundState previousState = (RoundState)(terminalState.previousState);  // RoundState before payoffs
        int street = previousState.street;  // 0, 3, 4, or 5 representing when this round ended
        List<String> myCards = previousState.hands.get(active);  // your cards
        List<String> oppCards = previousState.hands.get(1-active);  // opponent's cards or "" if not revealed
        Integer oppBid = previousState.bids.get(1-active);
        if(oppBid != null && oppBid != -1){
            this.oppBids.add((double)oppBid/this.auctionPot);
        }
        Collections.sort(this.oppBids);

        System.out.println("--------------");
    }

    /**
     * Where the magic happens - your code should implement this function.
     * Called any time the engine needs an action from your bot.
     *
     * @param gameState The GameState object.
     * @param roundState The RoundState object.
     * @param active Your player's index.
     * @return Your action.
     */

    public List<String> flushDraws(List<String> myCards, List<String> boardCards){
        List<String> draws = new ArrayList<String>();
        int[] mySuits = new int[4]; // s, c, h, d
        int[] boardSuits = new int[4];
        int[] totalSuits = new int[4];
        for(int i = 0; i < myCards.size(); i++){
            if(myCards.get(i).charAt(1) == 's'){
                mySuits[0]++;
                totalSuits[0]++;
            }
            if(myCards.get(i).charAt(1) == 'c'){
                mySuits[1]++;
                totalSuits[1]++;
            }
            if(myCards.get(i).charAt(1) == 'h'){
                mySuits[2]++;
                totalSuits[2]++;
            }
            if(myCards.get(i).charAt(1) == 'd'){
                mySuits[3]++;
                totalSuits[3]++;
            }
        }
        for(int i = 0; i < boardCards.size(); i++){
            if(boardCards.get(i).charAt(1) == 's'){
                boardSuits[0]++;
                totalSuits[0]++;
            }
            if(boardCards.get(i).charAt(1) == 'c'){
                boardSuits[1]++;
                totalSuits[1]++;
            }
            if(boardCards.get(i).charAt(1) == 'h'){
                boardSuits[2]++;
                totalSuits[2]++;
            }
            if(boardCards.get(i).charAt(1) == 'd'){
                boardSuits[3]++;
                totalSuits[3]++;
            }
        }

        if(totalSuits[0]>=3 && totalSuits[0] > boardSuits[0]){
            if(totalSuits[0] == 3){
                draws.add("s3");
            }
            if(totalSuits[0] == 4){
                draws.add("s4");
            }
        }
        if(totalSuits[1]>=3 && totalSuits[1] > boardSuits[1]){
            if(totalSuits[1] == 3){
                draws.add("c3");
            }
            if(totalSuits[1] == 4){
                draws.add("c4");
            }
        }
        if(totalSuits[2]>=3 && totalSuits[2] > boardSuits[2]){
            if(totalSuits[2] == 3){
                draws.add("h3");
            }
            if(totalSuits[2] == 4){
                draws.add("h4");
            }
        }
        if(totalSuits[3]>=3 && totalSuits[3] > boardSuits[3]){
            if(totalSuits[3] == 3){
                draws.add("d3");
            }
            if(totalSuits[3] == 4){
                draws.add("d4");
            }
        }

        return draws;
    }

    public List<Character> straightDraws(List<String> myCards, List<String> boardCards){
        List<Character> draws = new ArrayList<Character>();
        String ranks = "A23456789TJQK";
        //boolean[] myRanks = new boolean[17]; // A, 2, 3, ... K, A
        //boolean[] boardRanks = new boolean[17];
        boolean[] totalRanks = new boolean[14];

        for(int i = 0; i < myCards.size(); i++){
            char rank = myCards.get(i).charAt(0);
            if(rank == 'T'){
                totalRanks[9] = true;
            }
            else if(rank == 'J'){
                totalRanks[10] = true;
            }
            else if(rank == 'Q'){
                totalRanks[11] = true;
            }
            else if(rank == 'K'){
                totalRanks[12] = true;
            }
            else if(rank == 'A'){
                totalRanks[0] = true;
                totalRanks[13] = true;
            }
            else{
                totalRanks[rank-49] = true;
            }
        }
        for(int i = 0; i < boardCards.size(); i++){
            char rank = boardCards.get(i).charAt(0);
            if(rank == 'T'){
                totalRanks[9] = true;
            }
            else if(rank == 'J'){
                totalRanks[10] = true;
            }
            else if(rank == 'Q'){
                totalRanks[11] = true;
            }
            else if(rank == 'K'){
                totalRanks[12] = true;
            }
            else if(rank == 'A'){
                totalRanks[0] = true;
                totalRanks[13] = true;
            }
            else{
                totalRanks[rank-49] = true;
            }
        }
        //System.out.println(Arrays.toString(totalRanks));

        for(int i = 0; i <= 12; i++){
            if(!totalRanks[i]){
                boolean[] potentialRanks = totalRanks.clone();
                potentialRanks[i] = true;
                if(i == 0){
                    potentialRanks[13] = true;
                }
                //System.out.println("i="+i+Arrays.toString(potentialRanks));

                for(int j = 0; j <= 9; j++){
                    boolean straight = true;
                    for(int k = 0; k < 5; k++){
                        if(!potentialRanks[j+k]){
                            straight = false;
                            break;
                        }
                    }
                    if(straight){
                        draws.add(ranks.charAt(i));
                        break;
                    }
                }

            }
        }

        return draws;
    }
    public Action getAction(GameState gameState, RoundState roundState, int active) {

        // May be helpful, but you can choose to not use.
        int myBankroll = gameState.bankroll;  // the total number of chips you've gained or lost from the beginning of the game to the start of this round
        float gameClock = gameState.gameClock;  // the total number of seconds your bot has left to play this game
        int roundNum = gameState.roundNum;
        Set<ActionType> legalActions = roundState.legalActions();  // the actions you are allowed to take
        int street = roundState.street;  // 0, 3, 4, or 5 representing pre-flop, flop, turn, or river respectively
        List<String> myCards = roundState.hands.get(active);  // your cards
        List<String> boardCards0 = roundState.deck;  // the board cards
        List<String> boardCards = new ArrayList<String>(boardCards0);
        int myPip = roundState.pips.get(active);  // the number of chips you have contributed to the pot this round of betting
        int oppPip = roundState.pips.get(1-active);  // the number of chips your opponent has contributed to the pot this round of betting
        int myStack = roundState.stacks.get(active);  // the number of chips you have remaining
        int oppStack = roundState.stacks.get(1-active);  // the number of chips your opponent has remaining
        Integer myBid = roundState.bids.get(active); // How much you have bid. Null or -1 until after the auction.
        Integer oppBid = roundState.bids.get(1-active); // How much your opponent has bid. Null or -1 until after auction.
        int continueCost = oppPip - myPip;  // the number of chips needed to stay in the pot
        int myContribution = State.STARTING_STACK - myStack;  // the number of chips you have contributed to the pot
        int oppContribution = State.STARTING_STACK - oppStack;  // the number of chips your opponent has contributed to the pot
        int pot = myContribution + oppContribution;
        double potOdds = (double)continueCost/(continueCost + pot);
        int minCost = 0;
        int maxCost = 0;
        String handType = ""; //jeval7
        String boardType = ""; //jeval7

        for(int i = 0; i < boardCards.size(); i++){
            if(boardCards.get(i).length() < 2){
                boardCards.remove(i);
                i--;
            }
        }

        if(legalActions.contains(ActionType.BID_ACTION_TYPE)){
            this.auctionPot = pot;
        }

        if(!this.guaranteedWin){
            if(1.5*(1000-roundNum+1) + 1 < myBankroll){
                this.guaranteedWin = true;
                System.out.println("    guaranteed win");
            }
        }
        if(this.guaranteedWin){
            //keep check/folding if we have enough already to win
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            if(legalActions.contains(ActionType.BID_ACTION_TYPE)){
                return new Action(ActionType.BID_ACTION_TYPE, 0);
            }
            if(legalActions.contains(ActionType.CHECK_ACTION_TYPE)){
                return new Action(ActionType.CHECK_ACTION_TYPE);
            }
        }

        System.out.println("Our cards: " + myCards.toString() + "Board: " + boardCards.toString() + " " + handType);

        System.out.println("Street: "+street+", previous bets: "+this.numBets[street]);

        if(legalActions.contains(ActionType.CALL_ACTION_TYPE) && myContribution>1){
            this.numBets[street] += 1;
            System.out.println("    Pot Odds: " + potOdds);
        }

        if (legalActions.contains(ActionType.RAISE_ACTION_TYPE)) {
            List<Integer> raiseBounds = roundState.raiseBounds(); // the smallest and largest numbers of chips for a legal bet/raise
            minCost = raiseBounds.get(0) - myPip; // the cost of a minimum bet/raise
            maxCost = raiseBounds.get(1) - myPip; // the cost of a maximum bet/raise
        }

        double rank = 0;
        if(street > 0){
            rank = this.handRank(myCards, boardCards);
            System.out.println("    Rank: " + rank);
        }

        double strengthDiff = 0; //change?

        if(legalActions.contains(ActionType.BID_ACTION_TYPE)){
            this.auctionPot = pot;
            double minOppBid = 0;
            if(this.oppBids.size() > 50){
                minOppBid = this.oppBids.get(0);
                for(int i = 0; i<this.oppBids.size(); i++){
                    if(this.oppBids.get(i) > 0){
                        minOppBid = this.oppBids.get(i);
                        break;
                    }
                }
            }
            if(this.preflopFold && rank < 0.47){
                return new Action(ActionType.BID_ACTION_TYPE, (int)Math.min(pot*minOppBid, myStack));
            }
            // use new function for bid amounts
        }

        if(street == 0){
            int action = 0;
            double strength = this.preflopStrength;
            if(strength > 0.47){
                action = 1;
            }
            if(strength > 0.53){
                action = 2;
            }
            if(strength > 0.60){
                action = 3;
            }
            if(strength > 0.73){
                action = 4;
            }

            if(action == 0){
                System.out.println("    Want to fold");
                this.preflopFold = true;
            }
            else if(action == 1){
                System.out.println("    Want to open but fold to 3bet");
            }
            else if(action == 2){
                System.out.println("    Want to open and call 3bet");
            }
            else if(action == 3){
                System.out.println("    Want to 3bet and call 4bet");
            }
            else if(action == 4){
                System.out.println("    Want to 4bet");
            }

            if(action == 0){
                if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                    System.out.println("    Fold");
                    return new Action(ActionType.FOLD_ACTION_TYPE);
                }
                System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
            }
            if(myContribution == 1){
                this.numBets[street] += 1;
                System.out.println("    Open to 6");
                return new Action(ActionType.RAISE_ACTION_TYPE, 6);
            }
            if(myContribution == 2){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    if(action <= 2){
                        System.out.println("    Call");
                        return new Action(ActionType.CALL_ACTION_TYPE);
                    }
                    this.numBets[street] += 1;
                    System.out.println("    Raise");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.9*minCost + 0.1*maxCost));
                }
                if(legalActions.contains(ActionType.CHECK_ACTION_TYPE)){
                    this.numBets[street] += 1;
                    System.out.println("    Raise to 20");
                    return new Action(ActionType.RAISE_ACTION_TYPE, 20);
                }
            }
            if(legalActions.contains(ActionType.CALL_ACTION_TYPE) && action == 2){
                if(this.numBets[0] <= 3){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            if(legalActions.contains(ActionType.CALL_ACTION_TYPE) && action == 3){
                if(this.numBets[0] <= 2){
                    this.numBets[street] += 1;
                    System.out.println("    Raise");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.9*minCost+0.1*maxCost));
                }
                if(this.numBets[0] == 3){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE) && action == 1){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            if(legalActions.contains(ActionType.RAISE_ACTION_TYPE) && action == 4){
                if(this.numBets[0] <= 2){
                    this.numBets[street] += 1;
                    System.out.println("    Raise");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.9*minCost + 0.1*maxCost));
                }
                if(this.numBets[0] == 3){
                    this.numBets[street] += 1;
                    System.out.println("    Raise");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.7*minCost + 0.3*maxCost));
                }
                System.out.println("    Call");
                return new Action(ActionType.CALL_ACTION_TYPE);
            }
        }

        List<String> flushDraws = this.flushDraws(myCards, boardCards);
        List<Character> straightDraws = this.straightDraws(myCards, boardCards);
        if(street < 5){
            if(flushDraws.size() > 0){
                System.out.println("    Flush Draws: " + flushDraws.toString());
            }
            if(straightDraws.size() > 0){
                System.out.println("    Straight Draws: " + straightDraws.toString());
            }
        }
        
        if(legalActions.contains(ActionType.RAISE_ACTION_TYPE)){
            if(oppPip == 0){
                if(rank > 0.95){
                    this.numBets[street] += 1;
                    System.out.println("    Raise by a lot");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.5*minCost + 0.5*maxCost));
                }
                if(rank > 0.9){
                    double raiseAmt = pot;
                    raiseAmt = Math.min(Math.max(minCost, raiseAmt), maxCost);
                    this.numBets[street] += 1;
                    System.out.println("    Raise to pot");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)raiseAmt);
                }
                if(rank > 0.84){
                    double raiseAmt = 0.5*pot;
                    raiseAmt = Math.min(Math.max(minCost, raiseAmt), maxCost);
                    this.numBets[street] += 1;
                    System.out.println("    Raise to 1/2 pot");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)raiseAmt);
                }
                if(rank > 0.80){
                    double raiseAmt = (double)pot/3;
                    raiseAmt = Math.min(Math.max(minCost, raiseAmt), maxCost);
                    this.numBets[street] += 1;
                    System.out.println("    Raise to 1/3 pot");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)raiseAmt);
                }
            }
            else{
                if(rank > 0.97){
                    this.numBets[street] += 1;
                    System.out.println("    Raise by a lot");
                    return new Action(ActionType.RAISE_ACTION_TYPE, (int)(0.5*minCost + 0.5*maxCost));
                }
            }
        }

        double probImprove = 0;
        for(int i = 0; i<flushDraws.size(); i++){
            String draw = flushDraws.get(i);
            if(draw.charAt(1) == '3' && street == 3){
                probImprove += 0.04;
            }
            if(draw.charAt(1) == '4' && street == 3){
                probImprove += 0.35;
            }
            if(draw.charAt(1) == '4' && street == 4){
                probImprove += 0.195;
            }
        }
        if(straightDraws.size()==1 && street == 3){
            probImprove += 0.165;
        }
        if(straightDraws.size()>=2 && street == 3){
            probImprove += 0.315;
        }
        if(straightDraws.size() == 1 && street == 4){
            probImprove += 0.085;
        }
        if(straightDraws.size() >= 2 && street == 4){
            probImprove += 0.173;
        }

        if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
            if(probImprove > potOdds){
                System.out.println("    Call");
                return new Action(ActionType.CALL_ACTION_TYPE);
            }
        }

        if(rank <= 0.53){
            if(potOdds < 0.08){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
        }
        else if(rank > 0.98){
            if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                System.out.println("    Call");
                return new Action(ActionType.CALL_ACTION_TYPE);
            }
        }
        else if(rank > 0.9){
            if(potOdds < 0.36 && this.numBets[street] == 1){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            else if(potOdds < 0.3 && this.numBets[street] == 2){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
        }
        else if(rank > 0.77){
            if(potOdds < 0.31 && this.numBets[street] == 1){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            else if(potOdds < 0.19 && this.numBets[street] == 2){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
        }
        else if(rank > 0.69){
            if(potOdds < 0.24 && this.numBets[street] == 1){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            else if(potOdds < 0.13 && this.numBets[street] == 2){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
        }
        else if(rank > 0.53){
            if(potOdds < 0.15 && this.numBets[street] == 1){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            else if(potOdds < 0.10 && this.numBets[street] == 2){
                if(legalActions.contains(ActionType.CALL_ACTION_TYPE)){
                    System.out.println("    Call");
                    return new Action(ActionType.CALL_ACTION_TYPE);
                }
            }
            if(legalActions.contains(ActionType.FOLD_ACTION_TYPE)){
                System.out.println("    Fold");
                return new Action(ActionType.FOLD_ACTION_TYPE);
            }
            System.out.println("    Check");
                return new Action(ActionType.CHECK_ACTION_TYPE);
        }



        if(legalActions.contains(ActionType.CHECK_ACTION_TYPE)){
            System.out.println("    Check");
            return new Action(ActionType.CHECK_ACTION_TYPE);
        }
        System.out.println("    Call");
        return new Action(ActionType.CALL_ACTION_TYPE);

    }

    /**
     * Main program for running a Java pokerbot.
     */
    public static void main(String[] args) {
        Player player = new Player();
        Runner runner = new Runner();
        runner.parseArgs(args);
        runner.runBot(player);
    }
}