"""
@Author: Yu Huang
@Email: yuhuang-cst@foxmail.com
"""

import re

def process(input_path, output_path):
	text = open(input_path).read()
	lines = re.split(r'[、，：。！？；”“（）《》．,!:?;"()\'\s]', text)
	lines = [line.strip() for line in lines]
	open(output_path, 'w').writelines([line+'\n' for line in lines if len(line) > 1])


if __name__ == '__main__':
	input_path = 'story_of_stone/story_of_stone.txt'
	output_path = 'story_of_stone/corpus.txt'
	process(input_path, output_path)