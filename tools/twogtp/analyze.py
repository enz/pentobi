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
        print "%s: %.2f+-%.2f" \
            % (statistics.get_name(),
               statistics.get_mean(),
               statistics.get_error_mean()),


def print_stat_percent(statistics):
    if statistics.get_count() > 0:
        print "%s: %.1f%%+-%.1f%%" \
            % (statistics.get_name(),
               100 * statistics.get_mean(),
               100 * statistics.get_error_mean()),


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
result = Statistics("Res")
result_color_black = Statistics("ResB")
result_color_white = Statistics("ResW")
alternate_used = False
cpu_black = Statistics("CpuB")
cpu_white = Statistics("CpuW")
len = Statistics("Len")
for line in stdin.readlines():
    if line.strip().startswith("#"):
        continue
    games += 1
    columns = line.split("\t")
    game_number = int(columns[0])
    result_black = columns[1]
    result_white = columns[2]
    exchange_color = (columns[4].strip() == "True")
    cpu_black.add(float(columns[5]))
    cpu_white.add(float(columns[6]))
    len.add(int(columns[3]))
    if exchange_color:
        alternate_used = True
    if result_black.startswith("B+"):
        win.add(1)
        loss.add(0)
        draw.add(0)
        result_value = 1.0
    elif result_black.startswith("W+"):
        win.add(0)
        loss.add(1)
        draw.add(0)
        result_value = 0.0
    elif result_black == "0":
        win.add(0)
        loss.add(0)
        draw.add(1)
        result_value = 0.5
    else:
        exit("Invalid result: " + result_black)
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
           cpu_white.get_mean(),
           games)
else:
    print "Gam: %i" % games,
    print ",",
    print_stat_percent(result)
    if games > 0:
        if alternate_used:
            print ",",
            print_stat_percent(result_color_black)
            print ",",
            print_stat_percent(result_color_white)
        print
        print_stat_percent(win)
        print ",",
        print_stat_percent(loss)
        print ",",
        print_stat_percent(draw)
        print
        print_stat(cpu_black)
        print ",",
        print_stat(cpu_white)
        print ",",
        print_stat(len)
    print
