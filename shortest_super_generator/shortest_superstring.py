import os
import random
import shutil
import itertools
rand = random.randrange

def process_all():
    read_dir = "outputs/"
    write_dir = "shortest_superstrings/"
    with open(read_dir+filename) as string_file:
        orig_strings = string_file.read().split("\n")
        for i in range(1,5+1):
            write_filename = "{}{}{}.out".format(write_dir,filename,i)
            with open(write_filename,'w') as write_file:
                write_file.write(shortest_superstring(orig_strings))

def calc_overlap(str1,str2):
    min_len = min(len(str1),len(str2))
    for i in range(min_len):
        if str1[min_len-1-i] != str2[i]:
            print (str2[:i])
            return str2[:i]
    return str2[:min_len]

def calc_overlaps(in_strings):
    overlaps = []
    for i in range(len(in_strings)):
        overlaps.append([])
        for j in range(len(in_strings)):
            print (in_strings[i],in_strings[j])
            print(calc_overlap(in_strings[i],in_strings[j]))
            overlaps[i].append(len(calc_overlap(in_strings[i],in_strings[j])))
    return overlaps

def convert_adj_mat_to_list(mat):
    return  [[(v1,mat[v2][v1]) for v1 in range(len(mat))]
                           for v2 in range(len(mat))]


def calc_cost(graph,order):
    return len(order) - sum(graph[order[i]][order[i+1]] for i in range(len(order)-1))

def TSP(graph):
    size = len(graph)
    #print(graph)
    #print(list((calc_cost(graph,order),order) for order in itertools.permutations(range(size),size)))
    return min((calc_cost(graph,order),order) for order in itertools.permutations(range(size),size))

def shortest_superstring(in_strings):
    return TSP(calc_overlaps(in_strings))

def make_tsp(letters):
    size = len(letters)
    rand_weights = [[rand(1,20) for i in range(size)] for j in range(size)]
    #print(rand_weights)
    tsp = TSP(rand_weights)
    letterstr = "".join(letters[i] for i in tsp[1])
    #print (letterstr)
    return letterstr,rand_weights

def make_good_tsp(word,letters):
    assert set(word) & set(letters) == set(word)
    for i in range(1000):
        tsp_instance = make_tsp(letters)
        print("tspfinished")
        if word in tsp_instance[0]:
            return tsp_instance
    return None

#TSP(None)

print(make_good_tsp("abcd","abcde"))
