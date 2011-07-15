#!/usr/bin/python

from getopt import getopt
from math import sqrt
from sys import argv, exit, stdin

def err(f):
    return sqrt(f * (1 - f) / games)

def print_percent(name, n, games):
    f = float(n) / games
    print "%s: %.1f%%+-%.1f%%" % (name, 100 * f, 100 * err(f))

def print_count(name, n):
    f = float(n) / games
    print "%s:\t%i (%.1f%%+-%.1f%%)" % (name, n, 100 * f, 100 * err(f))

print_result = False
opts, args = getopt(argv[1:], "r", [
        "result",
        ])
for opt, val in opts:
    if opt in ("-r", "--result"):
        print_result = True

games = 0
win = 0
loss = 0
draw = 0
result = 0.0
games_color_black = 0
games_color_white = 0
result_color_black = 0.0
result_color_white = 0.0
alternate_used = False
cpu_black = 0.0
cpu_white = 0.0
for line in stdin.readlines():
    if line.strip().startswith("#"):
        continue
    games += 1
    columns = line.split("\t")
    game_number = int(columns[0])
    score_black = columns[1]
    score_white = columns[2]
    length = int(columns[3])
    exchange_color = (columns[4].strip() == "True")
    cpu_black += float(columns[5])
    cpu_white += float(columns[6])
    if exchange_color:
        alternate_used = True
    if score_black.startswith("B+"):
        score = int(score_black[2:])
    elif score_black.startswith("W+"):
        score = -int(score_black[2:])
    elif score_black == "0":
        score = 0
    else:
        exit("Invalid score: " + score_black)
    if score > 0:
        win += 1
        result_value = 1.0
    elif score < 0:
        loss += 1
        result_value = 0.0
    elif score == 0:
        draw += 1
        result_value = 0.5
    result += result_value
    if exchange_color:
        result_color_white += result_value
        games_color_white += 1
    else:
        result_color_black += result_value
        games_color_black += 1

if print_result:
    f = float(result) / games
    print "%.2f\t%.2f\t%.2f\t%.2f\t%i" \
        % (100 * f, 100 * err(f), (cpu_black / games), (cpu_white / games),
           games)
else:
    print "Games:\t%i" % games
    if games > 0:
        # TODO: compute real standard error for result, using the formula
        # for binary results overestimates the error, because there are also
        # draws (=0.5)
        print_percent("Result", result, games)
        if alternate_used:
            print_percent("ResultB", result_color_black, games_color_black)
            print_percent("ResultW", result_color_white, games_color_white)
        print_count("Win", win)
        print_count("Loss", loss)
        print_count("Draw", draw)
        print "CpuB:\t%.1f" % (cpu_black / games)
        print "CpuW:\t%.1f" % (cpu_white / games)

