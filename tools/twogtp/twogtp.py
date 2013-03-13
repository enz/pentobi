#!/usr/bin/python
# Python script to play a number of games between two different GTP engines.

from fcntl import flock, LOCK_EX, LOCK_NB
from getopt import getopt
from os import remove
from os.path import exists
from shlex import split
from string import lower, strip
from subprocess import PIPE, Popen
from sys import argv, exit, stderr
from threading import Lock, Thread

class GtpClient:
    def __init__(self, cmd, color):
        self.color = color
        self.process = Popen(split(cmd), stdin = PIPE, stdout = PIPE,
                             close_fds = True)
        
    def send(self, cmd):
        stderr.write(self.color + ">> " + cmd + "\n")
        self.process.stdin.write(cmd + "\n")
        self.process.stdin.flush()
        response = ""
        line = self._readline()
        while line != "\n":
            response = response + line
            line = self._readline()
        if response[-1] == "\n":
            response = response[:-1]
        stderr.write(self.color + "<< " + response + "\n")
        if (response[0] == "?"):
            raise Exception(response[2:])
        if (response[0] == "="):
            response = response[2:]
            return response
        exit(self.color + ": invalid response: " + response)

    def send_no_err(self, cmd):
        try:
            self.send(cmd)
        except:
            pass

    def _readline(self):
        line = self.process.stdout.readline()
        if self.process.poll() != None:
            exit(self.color + " terminated")
        return line

class OutputFile:
    """
    Result file. Keeps track of the game results and provides synchronized
    access for the threads to the number of the next game to play.
    """
    def __init__(self, prefix):
        self.prefix = prefix
        self.lock = Lock()
        self.games = dict()
        self.game_numbers = set()
        self.next_game_number = 0
        if not exists(self.prefix + ".dat"):
            return
        with open(self.prefix + ".dat", "r") as f:
            for line in f.readlines():
                if line.strip().startswith("#"):
                    continue
                columns = line.split("\t")
                game_number = int(columns[0])
                self.game_numbers.add(game_number)
                self.games[game_number] = line
        while self.next_game_number in self.game_numbers:
            self.next_game_number += 1

    def get_next_game_number(self):
        with self.lock:
            result = self.next_game_number
            self.next_game_number += 1
            while self.next_game_number in self.game_numbers:
                self.next_game_number += 1
            return result

    def add_result(self, game_number, result_black, result_white, move_number,
                   exchange_color, cpu_black, cpu_white, sgf):
        with self.lock:
            self.game_numbers.add(game_number)
            self.games[game_number] =  \
                "%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
                % (game_number, result_black, result_white, move_number,
                   exchange_color, cpu_black, cpu_white)
            with open(self.prefix + ".dat", "w") as f:
                f.write("# game\tres_b\tres_w\tlen\texchg\tcpu_b\tcpu_w\n")
                for i in self.game_numbers:
                    f.write(self.games[i])
            with open(self.prefix + ".blksgf", "a") as f:
                f.write(sgf)

def invert_result(result):
    if result.find("B+") != -1:
        return result.replace("B+", "W+")
    elif result.find("W+") != -1:
        return result.replace("W+", "B+")
    else:
        return result

def convert_four_player_result(result):
    """
    Make a two-player score out of the score of the four players in game
    variant classic. Assumes that Black plays Blue and Red, and White plays
    Yellow and Green (but as independent players without adding the score
    of both players like in game variant classic two-player). The score is
    B+1 if the winning color was played by Black, W+1 if it was played by White
    and 0 if it ties with one of the colors played by the other player.
    """
    result_array = result.split()
    if int(result_array[0]) > int(result_array[2]):
        max_black = int(result_array[0])
    else:
        max_black = int(result_array[2])
    if int(result_array[1]) > int(result_array[3]):
        max_white = int(result_array[1])
    else:
        max_white = int(result_array[3])
    if max_black > max_white:
        return "B+1"
    elif max_white > max_black:
        return "W+1"
    else:
        return "0"
        
def play_game(game_number, black, white, variant, output_file):
    stderr.write("=========================================================\n")
    stderr.write("Game %i\n" % game_number)
    stderr.write("=========================================================\n")
    exchange_color = (alternate and game_number % 2 != 0)
    black.send("clear_board")
    white.send("clear_board")
    cpu_black = float(black.send("cputime"))
    cpu_white = float(white.send("cputime"))
    if exchange_color:
        black, white = white, black
    to_play = black
    other = white
    if variant == "duo" or variant == "junior":
        colors = [ "b", "w" ]
    else:
        colors = [ "blue", "yellow", "red", "green" ]
    color_to_play = 0
    move_number = 0
    nu_passes = 0
    resign = False
    sgf = "(;GM[%s]GN[%s]\n" % (game_name, game_number)
    if exchange_color:
        sgf += "C[Player 1: %s\nPlayer 2: %s]\n" % (white_cmd, black_cmd)
    else:
        sgf += "C[Player 1: %s\nPlayer 2: %s]\n" % (black_cmd, white_cmd)
    while True:        
        try:
            move = strip(to_play.send("genmove " + colors[color_to_play]))
        except:
            sgf += ")\n"
            with open(prefix + ".fail.blksgf", "w") as f:
                f.write(sgf)
            raise
        move = lower(move)
        if move == "resign":
            resign = True
            break
        other.send("play " + colors[color_to_play] + " " + move)
        if move != "pass":
            nu_passes = 0            
            if variant == "duo":
                if color_to_play == 0:
                    prop_id = "B"
                else:
                    prop_id = "W"
            else:
                prop_id = str(color_to_play + 1)
            sgf += ";%s" % (prop_id)
            for p in split(move):
                sgf += "[%s]" % (p)
            if to_play.evaluate:
                try:
                    sgf += "V[%s]" % (to_play.send("get_value"))
                except:
                    pass
            sgf += "\n"
        else:
            nu_passes += 1
            if nu_passes == len(colors):
                break
        move_number += 1
        to_play, other = other, to_play
        color_to_play = color_to_play + 1
        if color_to_play == len(colors):
            color_to_play = 0
    sgf += ")\n"
    if exchange_color:
        black, white = white, black
    cpu_black = float(black.send("cputime")) - cpu_black
    cpu_white = float(white.send("cputime")) - cpu_white
    if resign:
        if not (variant == "duo" or variant == "junior"
                or variant == "classic_2" or variant == "trigon_2"):
            exit("resign only allowed in two-player game variants")
        if color_to_play == 0 or color_to_play == 2:
            result_black = "W+R"
            result_white = "W+R"
        else:
            result_black = "B+R"
            result_white = "B+R"
    else:
        result_black = black.send("final_score")
        result_white = white.send("final_score")
        if variant == "classic" or variant == "trigon":
            result_black = convert_four_player_result(result_black)
            result_white = convert_four_player_result(result_white)
    if exchange_color:
        result_black = invert_result(result_black)
        result_white = invert_result(result_white)
    output_file.add_result(game_number, result_black, result_white, move_number,
                           exchange_color, cpu_black, cpu_white, sgf)

def thread_main():
    black = GtpClient(black_cmd, "B")
    white = GtpClient(white_cmd, "W")
    black.evaluate = (evaluate == "black" or evaluate == "both")
    white.evaluate = (evaluate == "white" or evaluate == "both")
    black.send_no_err("set_game " + game_name)
    white.send_no_err("set_game " + game_name)
    while True:
        game_number = output_file.get_next_game_number()
        if game_number >= nu_games:
            break
        play_game(game_number, black, white, variant, output_file)

white_cmd = ""
black_cmd = ""
alternate = False
nu_games = 1
nu_threads = 1
variant = "duo"
prefix = "output"
evaluate = ""
opts, args = getopt(argv[1:], "ab:f:g:w:n:", [
        "alternate",
        "black=",
        "eval=",
        "file=",
        "game=",
        "nugames=",
        "threads=",
        "white=",
        ])
for opt, val in opts:
    if opt in ("-a", "--alternate"):
        alternate = True
    elif opt in ("-b", "--black"):
        black_cmd = val
    elif opt in ("--eval"):
        evaluate = val
    elif opt in ("-f", "--file"):
        prefix = val
    elif opt in ("-g", "--game"):
        variant = val
    elif opt in ("-n", "--nugames"):
        nu_games = int(val)
    elif opt in ("--threads"):
        nu_threads = int(val)
    elif opt in ("-w", "--white"):
        white_cmd = val

if black_cmd == "":
    exit("Missing black player")
if white_cmd == "":
    exit("Missing white player")
if nu_threads <= 0:
    exit("Invalid number of threads")

if variant == "c":
    variant = "classic"
elif variant == "c2":
    variant = "classic_2"
elif variant == "d":
    variant = "duo"
elif variant == "t":
    variant = "trigon"
elif variant == "t2":
    variant = "trigon_2"
elif variant == "j":
    variant = "junior"

if variant == "classic":
    game_name = "Blokus"
elif variant == "classic_2":
    game_name = "Blokus Two-Player"
elif variant == "trigon":
    game_name = "Blokus Trigon"
elif variant == "trigon_2":
    game_name = "Blokus Trigon Two-Player"
elif variant == "duo":
    game_name = "Blokus Duo"
elif variant == "junior":
    game_name = "Blokus Junior"
else:
    exit("invalid game variant: " + variant)

output_file = OutputFile(prefix)
lock_filename = prefix + ".lock"
with open(lock_filename, "w") as lock_file:
    flock(lock_file, LOCK_EX | LOCK_NB)
    threads = []
    for i in range(0, nu_threads):
        t = Thread(None, thread_main)
        t.start()
        threads.append(t)
    for t in threads:
        t.join()
remove(lock_filename)
