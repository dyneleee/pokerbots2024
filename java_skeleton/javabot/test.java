package javabot;

import java.util.*;
import java.io.*;

public class test {

    public static List<String> flushDraws(List<String> myCards, List<String> boardCards){
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

    public static List<Character> straightDraws(List<String> myCards, List<String> boardCards){
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

    public static Card toCard(String card){
        int rank = rankNumeric(card.charAt(0));
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

    public static int rankNumeric(char rank){
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

    public static double handRank(List<String> myCards, List<String> boardCards){
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
    public static void main(String[] args) {
        List<String> myCards = new ArrayList<String>();
        myCards.add("Ks");
        myCards.add("Th");
        myCards.add("5s");
        List<String> boardCards = new ArrayList<String>();
        boardCards.add("Ah");
        boardCards.add("Jh");
        boardCards.add("2d");
        //System.out.println(flushDraws(myCards, boardCards).toString());
        //System.out.println(straightDraws(myCards, boardCards).toString());
        System.out.println(handRank(myCards, boardCards));
    }
    
}