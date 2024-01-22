import numpy as np
import random as rd
from sklearn.cluster import kmeans_plusplus
from sklearn.cluster import KMeans

import eval7
import pickle

# card is represented as tuple (rank, suit), where ranks are all numerical
# each hand, either our hand or the board, is represented as a tuple sorted first by rank and then suit
# finishing state is represented by tuple (our hand, board)

def turn_hists(finish_groups): # turn to river
    rd4_hists = {} # maps 4th round state to histogram
    deck = []
    for i in range(14, 1, -1):
        for j in range(3, -1,-1):
            deck.append((i, j))
    #deck.sort(reverse = True)
    #print(deck)

    # 2-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            hole_deck = deck.copy()
            hole = (deck[i], deck[j])
            hole_deck.remove(hole[0])
            hole_deck.remove(hole[1])
            #print('hole deck: ' + str(hole_deck))

            for a in range(len(hole_deck)):
                for b in range(a+1, len(hole_deck)):
                    for c in range(b+1, len(hole_deck)):
                        for d in range(c+1, len(hole_deck)):
                            board = (hole_deck[a], hole_deck[b], hole_deck[c], hole_deck[d])
                            turn_deck = hole_deck.copy()
                            turn_deck.remove(board[0])
                            turn_deck.remove(board[1])
                            turn_deck.remove(board[2])
                            turn_deck.remove(board[3])
                            #print('turn deck: ' + str(turn_deck))
                            histogram = [0, 0, 0, 0, 0]

                            #print('hole: ' + str(hole) + ' board: ' + str(board))

                            for e in range(len(turn_deck)):
                                river_card = turn_deck[e]
                                river_board = [board[0], board[1], board[2], board[3], river_card]
                                river_board.sort(reverse = True)
                                finish_state = tuple(river_board)
                                group = finish_groups[(hole, finish_state)]
                                histogram[group] += 1

                            rd4_hists[(hole, board)] = histogram

    # 3-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            for k in range(j+1, len(deck)):
                hole_deck = deck.copy()
                hole = (deck[i], deck[j], deck[k])
                hole_deck.remove(hole[0])
                hole_deck.remove(hole[1])
                hole_deck.remove(hole[2])
                #print('hole deck: ' + str(hole_deck))

                for a in range(len(hole_deck)):
                    for b in range(a+1, len(hole_deck)):
                        for c in range(b+1, len(hole_deck)):
                            for d in range(c+1, len(hole_deck)):
                                board = (hole_deck[a], hole_deck[b], hole_deck[c], hole_deck[d])
                                turn_deck = hole_deck.copy()
                                turn_deck.remove(board[0])
                                turn_deck.remove(board[1])
                                turn_deck.remove(board[2])
                                turn_deck.remove(board[3])
                                #print('turn deck: ' + str(turn_deck))
                                histogram = [0, 0, 0, 0, 0]

                                #print('hole: ' + str(hole) + ' board: ' + str(board))

                                for e in range(len(turn_deck)):
                                    river_card = turn_deck[e]
                                    river_board = [board[0], board[1], board[2], board[3], river_card]
                                    river_board.sort(reverse = True)
                                    finish_state = tuple(river_board)
                                    group = finish_groups[(hole, finish_state)]
                                    histogram[group] += 1

                                rd4_hists[(hole, board)] = histogram
    return rd4_hists

def auction_hists(rd4_groups): # post-auction to turn
    rd3_hists = {}
    deck = []
    for i in range(14, 1, -1):
        for j in range(3, -1,-1):
            deck.append((i, j))
    #deck.sort(reverse = True)

    # 2-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            hole_deck = deck.copy()
            hole = (deck[i], deck[j])
            hole_deck.remove(hole[0])
            hole_deck.remove(hole[1])
            #print('hole deck: ' + str(hole_deck))

            for a in range(len(hole_deck)):
                for b in range(a+1, len(hole_deck)):
                    for c in range(b+1, len(hole_deck)):
                        board = (hole_deck[a], hole_deck[b], hole_deck[c])
                        flop_deck = hole_deck.copy()
                        flop_deck.remove(board[0])
                        flop_deck.remove(board[1])
                        flop_deck.remove(board[2])
                        #print('turn deck: ' + str(turn_deck))
                        histogram = [0, 0, 0, 0, 0]

                        #print('hole: ' + str(hole) + ' board: ' + str(board))

                        for d in range(len(flop_deck)):
                            turn_card = flop_deck[d]
                            turn_board = [board[0], board[1], board[2], turn_card]
                            turn_board.sort(reverse = True)
                            turn_state = tuple(turn_board)
                            group = rd4_groups[(hole, turn_state)]
                            histogram[group] += 1

                        rd3_hists[(hole, board)] = histogram

    # 3-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            for k in range(j+1, len(deck)):
                hole_deck = deck.copy()
                hole = (deck[i], deck[j], deck[k])
                hole_deck.remove(hole[0])
                hole_deck.remove(hole[1])
                hole_deck.remove(hole[2])
                #print('hole deck: ' + str(hole_deck))

                for a in range(len(hole_deck)):
                    for b in range(a+1, len(hole_deck)):
                        for c in range(b+1, len(hole_deck)):
                            board = (hole_deck[a], hole_deck[b], hole_deck[c])
                            flop_deck = hole_deck.copy()
                            flop_deck.remove(board[0])
                            flop_deck.remove(board[1])
                            flop_deck.remove(board[2])
                            #print('turn deck: ' + str(turn_deck))
                            histogram = [0, 0, 0, 0, 0]

                            #print('hole: ' + str(hole) + ' board: ' + str(board))

                            for d in range(len(flop_deck)):
                                turn_card = flop_deck[d]
                                turn_board = [board[0], board[1], board[2], turn_card]
                                turn_board.sort(reverse = True)
                                turn_state = tuple(turn_board)
                                group = rd4_groups[(hole, turn_state)]
                                histogram[group] += 1

                            rd3_hists[(hole, board)] = histogram
    return rd3_hists

def flop_hists(rd3_groups): # flop to auction
    rd2_hists = {}
    deck = []
    for i in range(14, 1, -1):
        for j in range(3, -1,-1):
            deck.append((i, j))

    # 2-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            hole_deck = deck.copy()
            hole = (deck[i], deck[j])
            hole_deck.remove(hole[0])
            hole_deck.remove(hole[1])
            #print('hole deck: ' + str(hole_deck))

            for a in range(len(hole_deck)):
                for b in range(a+1, len(hole_deck)):
                    for c in range(b+1, len(hole_deck)):
                        board = (hole_deck[a], hole_deck[b], hole_deck[c])
                        flop_deck = hole_deck.copy()
                        flop_deck.remove(board[0])
                        flop_deck.remove(board[1])
                        flop_deck.remove(board[2])
                        #print('turn deck: ' + str(turn_deck))
                        histogram = [0, 0, 0, 0, 0]

                        #print('hole: ' + str(hole) + ' board: ' + str(board))

                        # win auction
                        for d in range(len(flop_deck)):
                            auction_card = flop_deck[d]
                            new_hole = [hole[0], hole[1], auction_card]
                            new_hole.sort(reverse = True)
                            group = rd3_groups[(tuple(new_hole), board)]
                            histogram[group] += 1

                        # lose auction
                        group = rd3_groups[(hole, board)]
                        histogram[group] += 1

                        rd2_hists[(hole, board)] = histogram

    return rd2_hists

def preflop_hists(rd2_groups): # preflop to flop
    rd1_hists = {}
    deck = []
    for i in range(14, 1, -1):
        for j in range(3, -1,-1):
            deck.append((i, j))

    # 2-card holes
    for i in range(len(deck)):
        for j in range(i+1, len(deck)):
            hole_deck = deck.copy()
            hole = (deck[i], deck[j])
            hole_deck.remove(hole[0])
            hole_deck.remove(hole[1])
            histogram = [0, 0, 0, 0, 0]
            #print('hole deck: ' + str(hole_deck))

            for a in range(len(hole_deck)):
                for b in range(a+1, len(hole_deck)):
                    for c in range(b+1, len(hole_deck)):
                        flop = (hole_deck[a], hole_deck[b], hole_deck[c])
                        group = rd2_groups[(hole, flop)]
                        histogram[group] += 1

            rd1_hists[hole] = histogram

    return rd1_hists

def kmeanspp(data, k, iterations):
    '''Run kmeans'''
    darr = np.array(data)
    kmeans = KMeans(n_clusters=k, init= 'k-means++', n_init = 5, max_iter=iterations)
    kmeans.fit(darr)
    cl = kmeans.labels_ # Contains labels of clusters
    return cl 
    
    
def samplingkmeanspp(samsize, data, k, iterations):
    #data is a dict with that we're inputting 
    '''sampling k-mean algorithm that outputs k clusters'''
    # Convert to a group
    a = data.items()
    # Convert object to a list 
    b = list(a)
    #Take out hands from tuple (leave just histogram)
    histlist = []
    for i in range(len(b)):
        histlist.append(b[i][1])
    #Create random sample to do kmeans on (use rand if we're doing samplingkmean later)
    #rand = rd.sample(histlist, samsize)
    #Do kmeans
    clusterlst = kmeanspp(histlist, k, iterations)
    clmap = {}
    for i in range(len(b)):
        clmap[b[i[0]], clusterlst[i]]
    return clmap

finish_groups = {} # given, maps finishing state to groups 0-4

rd4_hists = turn_hists(finish_groups) # maps 4th round state to histogram

rd4_groups = samplingkmeanspp(10, rd4_hists, 5, 5) # maps 4th round state to groups

rd3_hists = auction_hists(rd4_groups)
# rd3_groups = cluster(rd3_hists)
rd3_groups = samplingkmeanspp(10, rd3_hists, 5, 5)

rd2_hists = flop_hists(rd3_groups)
# rd2_groups = cluster(rd2_hists)
rd2_groups = samplingkmeanspp(10, rd2_hists, 5, 5)

rd1_hists = preflop_hists(rd2_groups)
# rd1_groups = cluster(rd1_hists)
rd1_groups = samplingkmeanspp(10, rd1_hists, 5, 5) # should end up with 20 preflop clusters



#def kmeanspp(data, k):
#    '''Select starting nodes for kmeans'''
#    d = np.array(data)
#    centers, indices = kmeans_plusplus(d, n_clusters=k, random_state=None)
#    return centers, indices

