"""
@Author: Yu Huang
@Email: yuhuang-cst@foxmail.com
"""

import os
import topwords

def process1():
	print('process1: =========================')
	corpus_path = os.path.join('story_of_stone', 'corpus.txt')
	corpus = open(corpus_path).read().splitlines()
	vocab2freq, vocab2phi = topwords.em(
		corpus, n_iter=10, freq_threshold=1.0, max_len=4, lamb=1e-5, verbose=True, n_jobs=-1, loc='')

	topk, step = 200, 10
	vocab_freq = sorted(vocab2freq.items(), key=lambda item: item[1], reverse=True)[:topk]
	print('Top 200 words (sorted by frequency):')
	for i, (word, freq) in enumerate(vocab_freq):
		print('{} {:.2f}; '.format(word, freq), end='')
		if ((i+1) % step == 0): print('')
	print('')

	print('Top 200 words (sorted by phi):')
	vocab_phi = sorted(vocab2phi.items(), key=lambda item:item[1], reverse=True)[:topk]
	for i, (word, freq) in enumerate(vocab_phi):
		print('{} {:.2f}; '.format(word, freq), end='')
		if ((i+1) % step == 0): print('')
	print('')


def process2():
	print('process2: =========================')
	corpus_path = os.path.join('story_of_stone', 'corpus.txt')
	vocab2freq_path = os.path.join('py_vocab2freq.txt')
	vocab2phi_path = os.path.join('py_vocab2phi.txt')
	topwords.file_em(corpus_path, vocab2freq_path, vocab2phi_path, sep='|',
		n_iter=10, freq_threshold=1.0, max_len=4, lamb=1e-5, verbose=True, n_jobs=-1, loc='')


if __name__ == '__main__':
	process1()
	process2()
