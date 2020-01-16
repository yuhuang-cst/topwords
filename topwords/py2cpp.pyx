# cython: profile=False
# cython: boundscheck=False, wraparound=False, cdivision=True
# cython: language_level=3
# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp cimport bool
from cython.operator cimport dereference as deref, preincrement as inc

ctypedef unsigned int uint


cdef extern from "../topwords/topwords_lib.h":
	void topwords_em(const vector[string]& corpus, unordered_map[string, double]& vocab2freq,
			unordered_map[string, double]& vocab2psi, const string& loc, const uint n_iter,
			const double freq_threshold, const uint max_len, const double lamb, bool verbose, int n_jobs)
	void topwords_em(const string& input_txt, const string& vocab2freq_txt, const string& vocab2psi_txt, const string& sep, const string& loc,
                 const uint n_iter, const double freq_threshold, const uint max_len, const double lamb, bool verbose, int n_jobs)
	double sumlog(const vector[double]& log_values, const uint start, const uint n);


def em(corpus, n_iter=10, freq_threshold=1e-3, max_len=6, lamb=1e-5, verbose=True, n_jobs=-1, loc=""):
	"""
	Args:
		corpus (list): [T_1, T_2, ...]; list of unsegmented text T
		n_iter (int): Iteration number of EM algorithm
		freq_threshold (float): Candidate words whose frequencies < freq_threshold will be dropped in each iteration
		max_len (int): Maximum length of word
		lamb (float): Smooth factor when calculating word probability
		verbose (bool): Whether to print details when running EM algorithm
		n_jobs (int): Number of jobs for parallel processing; -1 means using all cores
		loc (str): Locale of transformation between wstring and string
	Returns:
		dict: {word: frequency, ...}
		dict: {word: psi}
	"""
	cdef uint corpus_size = len(corpus)
	cdef vector[string] ccorpus = [<string> T.encode('utf-8') for T in corpus]
	cdef unordered_map[string, double] cvocab2freq
	cdef unordered_map[string, double] cvocab2psi
	topwords_em(ccorpus, cvocab2freq, cvocab2psi, ''.encode('utf-8'), n_iter, freq_threshold, max_len, lamb, verbose, n_jobs)

	py_vocab2freq = {}
	cdef unordered_map[string, double].iterator it_f = cvocab2freq.begin()
	while it_f != cvocab2freq.end():
		py_vocab2freq[deref(it_f).first.decode('utf-8')] = deref(it_f).second
		inc(it_f)

	py_vocab2psi = {}
	cdef unordered_map[string, double].iterator it_p = cvocab2psi.begin()
	while it_p != cvocab2psi.end():
		py_vocab2psi[deref(it_p).first.decode('utf-8')] = deref(it_p).second
		inc(it_p)

	return py_vocab2freq, py_vocab2psi


def file_em(input_txt, vocab2freq_txt, vocab2psi_txt, sep='|',
		n_iter=10, freq_threshold=1e-3, max_len=6, lamb=1e-5, verbose=True, n_jobs=-1, loc=''):
	"""
	Args:
		input_txt (str): System path of input file with each line representing each T
		vocab2freq_txt (str): System path of output file to store vocabulary and frequency
		vocab2psi_txt (str): System path of output file to store vocabulary and psi
		sep (str): Separation string when writing output file
		loc (str): Locale of transformation between wstring and string
		n_iter (int): Iteration number of EM algorithm
		freq_threshold (float): Candidate words whose frequencies < freq_threshold will be dropped in each iteration
		max_len (int): Maximum length of word
		lamb (float): Smooth factor when calculating word probability
		verbose (bool): Whether to print details when running EM algorithm
		n_jobs (int): Number of jobs for parallel processing; -1 means using all cores
	"""
	topwords_em(input_txt.encode('utf-8'), vocab2freq_txt.encode('utf-8'), vocab2psi_txt.encode('utf-8'),
		sep.encode('utf-8'), loc.encode('utf-8'), n_iter, freq_threshold, max_len, lamb, verbose, n_jobs)

