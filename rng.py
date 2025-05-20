#! /usr/bin/env python3
from random import shuffle
from sys import argv

length = int(argv[1])
numbers = list(range(-((length + 1) // 2), length // 2))
shuffle(numbers)
print(" ".join(map(str, numbers)))
