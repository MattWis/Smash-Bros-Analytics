from os import walk
from json import dumps

MIN_RUN_LEN = 3

tournament = {}
games = []

tournament_directory = "/home/mwismer/Repos/ssbb/videos/Apex 2013/"

GAME_START = "GAME_START"
GAME_END = "GAME_END"
P1_HIT = "P1_HIT"
P2_HIT = "P2_HIT"
P1_DEAD = "P1_DEAD"
P2_DEAD = "P2_DEAD"
P1_LOSE_GAME = "P1_LOSE_GAME"
P2_LOSE_GAME = "P2_LOSE_GAME"
P1_LOSE_SET = "P1_LOSE_SET"
P2_LOSE_SET = "P2_LOSE_SET"

def parse_line(line):
  frame = int(line.split(".")[0])
  percent = int(line.split(", ")[1])
  return (frame, percent)

def frame(datapoint):
  return datapoint[0]

def percent(datapoint):
  return datapoint[1]

def group_by_percent(data):
  output = [(0, -1)]
  for snapshot in data:
    if percent(snapshot) != percent(output[-1]):
      output.append(snapshot)
  return output

def filter_long_runs(run_len, changes):
  output = []
  for i, data_point in enumerate(changes[:-2]):
    next_data_point = changes[i+1]
    if frame(next_data_point) - frame(data_point) > run_len:
      output.append(data_point)
  return output


def get_events(filename):
  lives_left = 3
  game_number = 0

  game_data = map(parse_line, list(filename))

  game_data = group_by_percent(game_data)
  game_data = filter_long_runs(3, game_data)
  game_data = group_by_percent(game_data)
  game_data = filter_long_runs(9, game_data)
  game_data = group_by_percent(game_data)

  print(filename)
  print(game_data)


def merge_events(event_hashes):
  sorted(event_hashes, key = lambda x: x['frame'])


(_, dirs, _) = walk(tournament_directory).next()

for game_dir in dirs:
  p1_file = open(tournament_directory + game_dir + "/p1.txt")
  p2_file = open(tournament_directory + game_dir + "/p2.txt")

  p1_events = get_events(p1_file)
  #p2_events = get_events(p2_file)

  #games.append({ game_dir, merge_events([p1_events, p2_events]) })



tournament["Apex 2013"] = dirs
dumps(tournament)

