package javabot;

import javabot.skeleton.Action;
import javabot.skeleton.ActionType;
import javabot.skeleton.GameState;
import javabot.skeleton.State;
import javabot.skeleton.TerminalState;
import javabot.skeleton.RoundState;
import javabot.skeleton.Bot;
import javabot.skeleton.Runner;

import java.util.List;
import java.util.Random;
import java.util.Set;
import java.lang.Integer;
import java.lang.String;
import java.util.*;
import java.io.*;

/**
 * A pokerbot.
 */
public class Player implements Bot {
    // Your instance variables go here.
    public boolean guaranteedWin = false;

    /**
     * Called when a new game starts. Called exactly once.
     */
    public Player() {
    }

    /**
     * Called when a new round starts. Called State.NUM_ROUNDS times.
     *
     * @param gameState The GameState object.
     * @param roundState The RoundState object.
     * @param active Your player's index.
     */
    public void handleNewRound(GameState gameState, RoundState roundState, int active) {
        int myBankroll = gameState.bankroll;  // the total number of chips you've gained or lost from the beginning of the game to the start of this round
        float gameClock = gameState.gameClock;  // the total number of seconds your bot has left to play this game
        int roundNum = gameState.roundNum;  // the round number from 1 to State.NUM_ROUNDS
        List<String> myCards = roundState.hands.get(active);  // your cards
        boolean bigBlind = (active == 1);  // true if you are the big blind
    }

    /**
     * Called when a round ends. Called State.NUM_ROUNDS times.
     *
     * @param gameState The GameState object.
     * @param terminalState The TerminalState object.
     * @param active Your player's index.
     */
    public void handleRoundOver(GameState gameState, TerminalState terminalState, int active) {
        int myDelta = terminalState.deltas.get(active);  // your bankroll change from this round
        RoundState previousState = (RoundState)(terminalState.previousState);  // RoundState before payoffs
        int street = previousState.street;  // 0, 3, 4, or 5 representing when this round ended
        List<String> myCards = previousState.hands.get(active);  // your cards
        List<String> oppCards = previousState.hands.get(1-active);  // opponent's cards or "" if not revealed
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
        String ranks = "23456789TJQKA";
        //boolean[] myRanks = new boolean[17]; // 2, 3, ... K, A, 2, 3, 4, 5
        //boolean[] boardRanks = new boolean[17];
        boolean[] totalRanks = new boolean[17];

        for(int i = 0; i < myCards.size(); i++){
            char rank = myCards.get(i).charAt(0);
            if(rank == 'T'){
                totalRanks[8] = true;
            }
            else if(rank == 'J'){
                totalRanks[9] = true;
            }
            else if(rank == 'Q'){
                totalRanks[10] = true;
            }
            else if(rank == 'K'){
                totalRanks[11] = true;
            }
            else if(rank == 'A'){
                totalRanks[12] = true;
            }
            else if(62<=rank && rank<=65){
                totalRanks[rank-62] = true;
                totalRanks[rank-49] = true;
            }
            else{
                totalRanks[rank-62] = true;
            }
        }
        for(int i = 0; i < boardCards.size(); i++){
            char rank = boardCards.get(i).charAt(0);
            if(rank == 'T'){
                totalRanks[8] = true;
            }
            else if(rank == 'J'){
                totalRanks[9] = true;
            }
            else if(rank == 'Q'){
                totalRanks[10] = true;
            }
            else if(rank == 'K'){
                totalRanks[11] = true;
            }
            else if(rank == 'A'){
                totalRanks[12] = true;
            }
            else if(62<=rank && rank<=65){
                totalRanks[rank-62] = true;
                totalRanks[rank-49] = true;
            }
            else{
                totalRanks[rank-62] = true;
            }
        }

        for(int i = 0; i <= 12; i++){
            if(!totalRanks[i]){
                boolean[] potentialRanks = totalRanks.clone();
                potentialRanks[i] = true;
                if(i <= 3){
                    potentialRanks[i + 13] = true;
                }

                for(int j = 0; j <= 12; j++){
                    boolean straight = true;
                    for(int k = 0; k < 5; k++){
                        if(!potentialRanks[k]){
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
        List<String> boardCards = roundState.deck;  // the board cards
        int myPip = roundState.pips.get(active);  // the number of chips you have contributed to the pot this round of betting
        int oppPip = roundState.pips.get(1-active);  // the number of chips your opponent has contributed to the pot this round of betting
        int myStack = roundState.stacks.get(active);  // the number of chips you have remaining
        int oppStack = roundState.stacks.get(1-active);  // the number of chips your opponent has remaining
        int myBids = roundState.bids.get(active);        // How much you bid previously (available only after auction)
        int oppBids = roundState.bids.get(1-active);     // How much opponent bid previously (available only after auction)
        int continueCost = oppPip - myPip;  // the number of chips needed to stay in the pot
        int myContribution = State.STARTING_STACK - myStack;  // the number of chips you have contributed to the pot
        int oppContribution = State.STARTING_STACK - oppStack;  // the number of chips your opponent has contributed to the pot
        int pot = myContribution + oppContribution;
        double potOdds = (double)continueCost/(continueCost + pot);
        int minCost = 0;
        int maxCost = 0;

        if(!this.guaranteedWin){
            if(1.5*(1000-roundNum+1) + 1 < myBankroll){
                this.guaranteedWin = true;
                System.out.println("guaranteed win");
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

        if (legalActions.contains(ActionType.RAISE_ACTION_TYPE)) {
            List<Integer> raiseBounds = roundState.raiseBounds(); // the smallest and largest numbers of chips for a legal bet/raise
            minCost = raiseBounds.get(0) - myPip; // the cost of a minimum bet/raise
            maxCost = raiseBounds.get(1) - myPip; // the cost of a maximum bet/raise
        }

        // Basic bot that bids and raises randomly, or just checks and calls.
        Random rand = new Random();
        if (legalActions.contains(ActionType.BID_ACTION_TYPE)) { 
            return new Action(ActionType.BID_ACTION_TYPE, rand.nextInt(myPip + maxCost)); // Random bid between 0 and our stack size
        }
        if (legalActions.contains(ActionType.CHECK_ACTION_TYPE)) {  // Check
            return new Action(ActionType.CHECK_ACTION_TYPE);
        } else {
            return new Action(ActionType.FOLD_ACTION_TYPE);
        }



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