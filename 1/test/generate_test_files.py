#!/usr/bin/python3

import sys
from random import random, choice
from string import ascii_letters


new_line_chance = 0.1
space_chance = 0.3

try:
    files_count = int(sys.argv[1])
    file_length = int(sys.argv[2])
    file_name_base = sys.argv[3]

    for i in range(files_count):
        file_name = f'{file_name_base}{i}.txt'
        with open(file_name, 'w+') as f:
            line = ''
            for ch in range(file_length):
                line += choice(ascii_letters)
                if random() < new_line_chance:
                    f.write(line + '\n')
                    line = ''
                if random() < space_chance:
                    line += ' '
except IndexError:
    print('./generate_test_files.py <files_count> <file_length> <file_name_base>')
