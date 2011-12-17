#!/usr/bin/python
# Python script to play a number of games between two different GTP engines.

from getopt import getopt
from os.path import exists
from shlex import split
from string import lower, strip
from subprocess import PIPE, Popen
from sys import argv, exit, stderr

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

def invert_score(score):
    if score.find("B+") != -1:
        return score.replace("B+", "W+")
    elif score.find("W+") != -1:
        return score.replace("W+", "B+")
    else:
        return score

def convert_four_player_score(score):
    """
    Make a two-player score out of the score of the four players in game
    variant classic. Assumes that Black plays Blue and Red, and White plays
    Yellow and Green (but as independent players without adding the score
    of both players like in game variant classic two-player). The score is
    B+1 if the winning color was played by Black, W+1 if it was played by White
    and 0 if it ties with one of the colors played by the other player.
    """
    score_array = score.split()
    if int(score_array[0]) > int(score_array[2]):
        max_black = int(score_array[0])
    else:
        max_black = int(score_array[2])
    if int(score_array[1]) > int(score_array[3]):
        max_white = int(score_array[1])
    else:
        max_white = int(score_array[3])
    if max_black > max_white:
        return "B+1"
    elif max_white > max_black:
        return "W+1"
    else:
        return "0"
        
def play_game(game_number, black, white, game_variant):
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
    if game_variant == "duo":
        colors = [ "b", "w" ]
    else:
        colors = [ "blue", "yellow", "red", "green" ]
    color_to_play = 0
    move_number = 0
    nu_passes = 0
    sgf = "(;GM[%s]GN[%s]\n" % (game_name, game_number)
    if exchange_color:
        sgf += "C[Player 1: %s\nPlayer 2: %s]\n" % (black_cmd, white_cmd)
    else:
        sgf += "C[Player 1: %s\nPlayer 2: %s]\n" % (white_cmd, black_cmd)
    while True:        
        move = strip(to_play.send("genmove " + colors[color_to_play]))
        other.send("play " + colors[color_to_play] + " " + move)
        if lower(move) != "pass":
            nu_passes = 0            
            if game_variant == "duo":
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
    score_black = black.send("final_score")
    score_white = white.send("final_score")
    if game_variant == "classic" or game_variant == "trigon":
        score_black = convert_four_player_score(score_black)
        score_white = convert_four_player_score(score_white)
    if exchange_color:
        score_black = invert_score(score_black)
        score_white = invert_score(score_white)
    filename = prefix + ".blksgf"
    with open(filename, "a") as f:
        f.write(sgf)
    filename = prefix + ".dat"
    if not exists(filename):
        with open(filename, "w") as f:
            f.write("# game\tscore_b\tscore_w\tlen\texchange\tcpu_b\tcpu_w\n")
    with open(filename, "a") as f:
        f.write("%s\t%s\t%s\t%s\t%s\t%s\t%s\n"
                % (game_number, score_black, score_white, move_number,
                   exchange_color, cpu_black, cpu_white))

def find_start_index():
    filename = prefix + ".dat"
    start_index = 0
    if not exists(filename):
        return start_index
    with open(filename, "r") as f:
        for line in f.readlines():
            if line.strip().startswith("#"):
                continue
            columns = line.split("\t")
            start_index = int(columns[0]) + 1
    return start_index

white_cmd = ""
black_cmd = ""
alternate = False
number_games = 1
game_variant = "duo"
prefix = "output"
evaluate = ""

opts, args = getopt(argv[1:], "ab:f:g:w:n:", [
        "alternate",
        "black=",
        "eval=",
        "file=",
        "game=",
        "white=",
        "nugames=",
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
        game_variant = val
    elif opt in ("-n", "--nugames"):
        number_games = int(val)
    elif opt in ("-w", "--white"):
        white_cmd = val
if black_cmd == "":
    exit("Missing black player")
if white_cmd == "":
    exit("Missing white player")
if game_variant == "classic":
    game_name = "Blokus"
elif game_variant == "classic_2":
    game_name = "Blokus Two-Player"
elif game_variant == "trigon":
    game_name = "Blokus Trigon"
elif game_variant == "trigon_2":
    game_name = "Blokus Trigon Two-Player"
elif game_variant == "duo":
    game_name = "Blokus Duo"
else:
    exit("invalid game variant: " + game_variant)
black = GtpClient(black_cmd, "B")
white = GtpClient(white_cmd, "W")
black.evaluate = (evaluate == "black" or evaluate == "both")
white.evaluate = (evaluate == "white" or evaluate == "both")
black.send_no_err("set_game " + game_name)
white.send_no_err("set_game " + game_name)
start = find_start_index()
for game_number in range(start, number_games):
    play_game(game_number, black, white, game_variant)
