import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.io.*;
import java.util.*;


/** Utility methods for evaluating or creating a hand of cards. */
public abstract class Hand {
    /**
     * Private constructor to disable instantiation of an abstract class.
     */
    private Hand() {

    }

    public static int minEval1 = 999999;
    public static int minEval2 = 999999;

    /**
     * Evaluates the given hand and returns its value as an integer.
     * Based on Kevin Suffecool's 5-card hand evaluator and with Paul Senzee's pre-computed hash.
     * @param cards a hand of cards to evaluate
     * @return the value of the hand as an integer between 1 and 7462
     */
    public static int evaluate(Card[] cards) {
        // Only 5-card hands are supported
        if (cards == null || cards.length != 5) {
            throw new IllegalArgumentException("Exactly 5 cards are required.");
        }

        // Binary representations of each card
        final int c1 = cards[0].getValue();
        final int c2 = cards[1].getValue();
        final int c3 = cards[2].getValue();
        final int c4 = cards[3].getValue();
        final int c5 = cards[4].getValue();

        // No duplicate cards allowed
        if (hasDuplicates(new int[]{c1, c2, c3, c4, c5})) {
            throw new IllegalArgumentException("Illegal hand.");
        }

        // Calculate index in the flushes/unique table
        final int index = (c1 | c2 | c3 | c4 | c5) >> 16;

        // Flushes, including straight flushes
        if ((c1 & c2 & c3 & c4 & c5 & 0xF000) != 0) {
            return Tables.Flushes.TABLE[index];
        }

        // Straight and high card hands
        final int value = Tables.Unique.TABLE[index];
        if (value != 0) {
            return value;
        }

        // Remaining cards
        final int product = (c1 & 0xFF) * (c2 & 0xFF) * (c3 & 0xFF) * (c4 & 0xFF) * (c5 & 0xFF);
        return Tables.Hash.Values.TABLE[hash(product)];
    }



    //combinations start
    static int combinationUtil(Card arr[], Card data[], int start,
    int end, int index, int r, int indicator) {
    // Current combination is ready to be printed, print it
    Card[] cardArr = new Card[5];


    if (index == r) {
        for (int j=0; j<r; j++)
            cardArr[j]=data[j];
            System.out.println("here");
        
        int val = evaluate(cardArr);
        if (indicator == 1){
            if (val<minEval1){
                minEval1 = val;
            }
        } else{
            if (val<minEval2){
                minEval2 = val;
            } 
        }
        System.out.println("evaluation for hand "+indicator+" is "+val);
        System.out.println("min Eval 1 is now "+minEval1);
        System.out.println("min Eval 2 is now "+minEval2);
        System.out.println("hand was");
        for (int i = 0; i < cardArr.length; i++){
            System.out.print(cardArr[i] + " ");
        }
        System.out.println();

        return val;
    }

    // replace index with all possible elements. The condition
    // "end-i+1 >= r-index" makes sure that including one element
    // at index will make a combination with remaining elements
    // at remaining positions
    for (int i=start; i<=end && end-i+1 >= r-index; i++) {
        data[index] = arr[i];
        combinationUtil(arr, data, i+1, end, index+1, r, indicator);
        }
    return r;
    }

    // The main function that prints all combinations of size r
    // in arr[] of size n. This function mainly uses combinationUtil()
    static void printCombination(Card[] arr, int n, int r, int indicator)
    {
        // A temporary array to store all combination one by one
        Card data[]=new Card[r];

        // Print all combination using temporary array 'data[]'
        combinationUtil(arr, data, 0, n-1, 0, r, indicator);
    }

    /*Driver function to check for above function*/
    public static int battleHands(Card[] cards1, Card[] cards2) {

        int r = 5;
        int n1 = cards1.length;
        int n2 = cards2.length;
        printCombination(cards1, n1, r, 1);
        printCombination(cards2, n2, r, 2);
        

        if (minEval1<minEval2){
            System.out.println("1 wins, minEval1: "+minEval1+" minEval2: "+minEval2);
            return 1;
        } else if (minEval2<minEval1){
            System.out.println("2 wins, minEval1: "+minEval1+" minEval2: "+minEval2);
            return 2;
        } else { //tie
            System.out.println("tie, minEval1: "+minEval1+" minEval2: "+minEval2);
            return 0;
        }
    }



    /**
     * Creates a new 5-card hand from the given string.
     * @param string the string to create the hand from, such as "Kd 5s Jc Ah Qc"
     * @return a new hand as an array of cards
     * @see Card
     */
    public static Card[] fromString(String string) {
        final String[] parts = string.split(" ");
        final Card[] cards = new Card[parts.length];

        if (parts.length != 5)
            throw new IllegalArgumentException("Exactly 5 cards are required.");

        int index = 0;
        for (String part : parts)
            cards[index++] = Card.fromString(part);

        return cards;
    }

    /**
     * Converts the given hand into concatenation of their string representations
     * @param cards a hand of cards
     * @return a concatenation of the string representations of the given cards
     */
    public static String toString(Card[] cards) {
        final StringBuilder builder = new StringBuilder();

        for (int i = 0; i < cards.length; i++) {
            builder.append(cards[i]);
            if (i < cards.length - 1)
                builder.append(" ");
        }

        return builder.toString();
    }

    /**
     * Checks if the given array of values has any duplicates.
     * @param values the values to check
     * @return true if the values contain duplicates, false otherwise
     */
    private static boolean hasDuplicates(int[] values) {
        Arrays.sort(values);
        for (int i = 1; i < values.length; i++) {
            if (values[i] == values[i - 1])
                return true;
        }
        return false;
    }

    private static int hash(int key) {
        key += 0xE91AAA35;
        key ^= key >>> 16;
        key += key << 8;
        key ^= key >>> 4;
        return ((key + (key << 2)) >>> 19) ^ Tables.Hash.Adjust.TABLE[(key >>> 8) & 0x1FF];
    }
    public static void main (String[] args) {
        Card[] hand1 = {
             
            new Card(Card.KING, Card.CLUBS),
            new Card(Card.QUEEN, Card.HEARTS),
            
            new Card(Card.JACK, Card.CLUBS),
            new Card(Card.TEN, Card.CLUBS),
            new Card(Card.NINE, Card.CLUBS),
            new Card(Card.TREY, Card.CLUBS),
            new Card(Card.FOUR, Card.CLUBS),
        };
        Card[] hand2 = {
            
            new Card(Card.KING, Card.CLUBS),
            new Card(Card.KING, Card.HEARTS),
            
            new Card(Card.JACK, Card.DIAMONDS),
            new Card(Card.JACK, Card.SPADES),
            new Card(Card.JACK, Card.CLUBS),
            new Card(Card.TREY, Card.SPADES),
            new Card(Card.FOUR, Card.CLUBS),
        };


        System.out.println(battleHands(hand1,hand2));
        System.out.println("min eval 1: " + minEval1);
        System.out.println("min eval 2: " + minEval2);

    }


}

