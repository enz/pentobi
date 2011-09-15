#!/usr/bin/python

from getopt import getopt
from math import sqrt
from sys import argv, exit, stdin

class Statistics:
    def __init__(self, name):
        self._name = name
        self._sum = 0.0
        self._sum_sq = 0.0
        self._count = 0

    def add(self, value):
        self._sum += value
        self._sum_sq += (value * value)
        self._count += 1

    def get_name(self):
        return self._name

    def get_count(self):
        return self._count

    def get_deviation(self):
        return sqrt(self.get_variance())

    def get_error_mean(self):
        return self.get_deviation() / sqrt(self._count)

    def get_mean(self):
        return self._sum / self._count

    def get_variance(self):
        mean = self.get_mean()
        return self._sum_sq / self._count - mean * mean


def print_stat(statistics):
    if statistics.get_count() > 0:
        print "%s:\t%.1f+-%.1f" \
            % (statistics.get_name(),
               statistics.get_mean(),
               statistics.get_error_mean())

def print_stat_percent(statistics):
    if statistics.get_count() > 0:
        print "%s:\t%.1f%%+-%.1f%%" \
            % (statistics.get_name(),
               100 * statistics.get_mean(),
               100 * statistics.get_error_mean())

def print_stat_percent_count(statistics):
    if statistics.get_count() > 0:
        print "%s:\t%i (%.1f%%+-%.1f%%)" \
            % (statistics.get_name(),
               statistics.get_count(),
               100 * statistics.get_mean(),
               100 * statistics.get_error_mean())

print_result = False
opts, args = getopt(argv[1:], "r", [
        "result",
        ])
for opt, val in opts:
    if opt in ("-r", "--result"):
        print_result = True

games = 0
win = Statistics("Win")
loss = Statistics("Loss")
draw = Statistics("Draw")
result = Statistics("Result")
score_stat = Statistics("Score")
result_color_black = Statistics("ResB")
result_color_white = Statistics("ResW")
alternate_used = False
cpu_black = Statistics("CpuB")
cpu_white = Statistics("CpuW")
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
    cpu_black.add(float(columns[5]))
    cpu_white.add(float(columns[6]))
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
    score_stat.add(score)
    if score > 0:
        win.add(1)
        loss.add(0)
        draw.add(0)
        result_value = 1.0
    elif score < 0:
        win.add(0)
        loss.add(1)
        draw.add(0)
        result_value = 0.0
    elif score == 0:
        win.add(0)
        loss.add(0)
        draw.add(1)
        result_value = 0.5
    result.add(result_value)
    if exchange_color:
        result_color_white.add(result_value)
    else:
        result_color_black.add(result_value)

if print_result:
    print "%.2f\t%.2f\t%.2f\t%.2f\t%i" \
        % (100 * result.get_mean(),
           100 * result.get_error_mean(),
           cpu_black.get_mean(),
           cpu_whiteget_mean(),
           games)
else:
    print "Games:\t%i" % games
    print_stat_percent(result)
    if games > 0:
        if alternate_used:
            print_stat_percent(result_color_black)
            print_stat_percent(result_color_white)
        print_stat_percent_count(win)
        print_stat_percent_count(loss)
        print_stat_percent_count(draw)
        print_stat(cpu_black)
        print_stat(cpu_white)
        print_stat(score_stat)

