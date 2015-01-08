#!/usr/bin/python2
import cv2
import fileinput

ONES_DIGIT_LOCATION = 42
TENS_DIGIT_LOCATION = 22
HUNDREDS_DIGIT_LOCATION = 2
THRESHOLD = 5

NUM_LIKELY = 5

digits = range(10)
digits[0] = cv2.imread("/home/mwismer/Repos/ssbb/templates/0.tiff")
digits[1] = cv2.imread("/home/mwismer/Repos/ssbb/templates/1.tiff")
digits[2] = cv2.imread("/home/mwismer/Repos/ssbb/templates/2.tiff")
digits[3] = cv2.imread("/home/mwismer/Repos/ssbb/templates/3.tiff")
digits[4] = cv2.imread("/home/mwismer/Repos/ssbb/templates/4.tiff")
digits[5] = cv2.imread("/home/mwismer/Repos/ssbb/templates/5.tiff")
digits[6] = cv2.imread("/home/mwismer/Repos/ssbb/templates/6.tiff")
digits[7] = cv2.imread("/home/mwismer/Repos/ssbb/templates/7.tiff")
digits[8] = cv2.imread("/home/mwismer/Repos/ssbb/templates/8.tiff")
digits[9] = cv2.imread("/home/mwismer/Repos/ssbb/templates/9.tiff")

def get_percent_from_correlations(correlations):
  digits = [None, None, None] # Last digit is hundreds digit

  for (_, digit, location) in correlations:
    if abs(location - ONES_DIGIT_LOCATION) <= THRESHOLD:
      if digits[0] == None:
        digits[0] = digit
    elif abs(location - TENS_DIGIT_LOCATION) <= THRESHOLD:
      if digits[1] == None:
        digits[1] = digit
    elif abs(location - HUNDREDS_DIGIT_LOCATION) <= THRESHOLD:
      if digits[2] == None:
        digits[2] = digit

  if digits[0] == None:
    return -1
  elif digits[1] == None:
    # Arguably should be an error if there is a hundreds digit but no tens digit
    return digits[0]
  elif digits[2] == None:
    return digits[1] * 10 + digits[0]
  else:
    return digits[2] * 100 + digits[1] * 10 + digits[0]

def get_percent_from_file(filename):
  correlations = {}
  image = cv2.imread(filename)

  for i in range(10):
    correlation = cv2.matchTemplate(image, digits[i], cv2.TM_CCORR)
    correlation = cv2.normalize(correlation)
    for location, value in enumerate(correlation[0]):
      correlations[(value, i, location)] = ""

  likely = correlations.keys()
  likely.sort()
  likely.reverse()
  likely = likely[0:NUM_LIKELY]

  return get_percent_from_correlations(likely)


for line in fileinput.input():
  line = line.strip()
  percent = get_percent_from_file(line)
  print(line + ", " + str(percent))

