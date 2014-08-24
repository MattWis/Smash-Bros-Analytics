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

def contiguous_frames(base_frame, data_slice):
  frames = map(frame, data_slice)

  low_run = 0
  while base_frame - low_run - 1 in frames:
    low_run += 1

  high_run = 0
  while base_frame + high_run + 1 in frames:
    high_run += 1

  return low_run + high_run + 1

def has_a_run(some_structure):
  base_frame = frame(some_structure[0])
  base_percent = percent(some_structure[0])
  data_slice = some_structure[1]

  same_percent = [x for x in data_slice if base_percent == percent(x)]
  return contiguous_frames(base_frame, same_percent) >= MIN_RUN_LEN



def get_events(filename):
  lives_left = 3
  game_number = 0

  game_data = map(parse_line, list(filename))
  game_data = [x for x in game_data if percent(x) != -1]

  data_slices = []
  for i, _ in enumerate(game_data):
    if i < MIN_RUN_LEN + 1:
      low_end = 0
    else:
      low_end = i - MIN_RUN_LEN - 1
    high_end = i + MIN_RUN_LEN
    data_slices.append(game_data[low_end:high_end])

  assert len(data_slices) == len(game_data)
  enumerated_slices = zip(game_data, data_slices)
  runs_of_data = [x[0] for x in enumerated_slices if has_a_run(x)]

  print(runs_of_data)


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

