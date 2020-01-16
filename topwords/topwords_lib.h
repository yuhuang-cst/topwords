/**
* @Author: Yu Huang
* @Email: yuhuang-cst@foxmail.com
*/

#ifndef TOPWORDS_LIB_H
#define TOPWORDS_LIB_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ctime>

#ifdef _OPENMP
#include "omp.h"
#endif

using namespace std;

typedef unsigned int uint;
typedef wchar_t wchar;

const double DOUBLE_INF = numeric_limits<double>::infinity();
const double DOUBLE_NEG_INF = -numeric_limits<double>::infinity();

class Logger {
public:
    bool verbose;
    ostream& out;
    Logger(ostream &out_, bool verbose_=true): out(out_) {verbose = verbose_;}

    template <typename T>
    void info(const T& v){
        if (verbose) out << v << endl;
    }
    template <typename T, typename... Args>
    void info(const T& v, Args... args) {
        if (verbose) out << v << " ";
        info(args...);
    }
};

template <typename K_TYPE, typename V_TYPE>
class KeyValueComp {
public:
    bool reverse = true;
    bool sort_by_value = true;
    KeyValueComp() {};
    KeyValueComp(bool reverse, bool sort_by_value) {
        this->reverse = reverse; this->sort_by_value = sort_by_value;
    }
    bool operator () (const pair<K_TYPE, V_TYPE>& p1, const pair<K_TYPE, V_TYPE>& p2) {
        if (reverse)
            return sort_by_value? p1.second > p2.second: p1.first > p2.first;
        else
            return sort_by_value? p1.second < p2.second: p1.first < p2.first;
    }
};

inline bool is_neginf(const double& x) {return x == DOUBLE_NEG_INF;}
inline bool is_posinf(const double& x) {return x == DOUBLE_INF;}
inline double get_seconds(const clock_t& t_span) {return (double)t_span / CLOCKS_PER_SEC;}
inline double get_seconds(const clock_t& b, const clock_t& e) {return (double)(e - b) / CLOCKS_PER_SEC;}

wstring s2ws(const string& src, const string& loc="");
wstring s2ws(string_view src, const string& loc="");
string ws2s(const wstring& src, const string& loc="");
string ws2s(wstring_view, const string& loc="");

void read_wlines(const string& file_path, vector<wstring>& lines, const string& loc="");
void write_wlines(const string& file_path, const vector<wstring>& wlines, const string& loc="");
void get_all_ngram(wstring_view T, const uint max_len, unordered_set<wstring_view> &ngram_set);
void init_vocab2freq(const vector<wstring> &corpus, uint max_len, unordered_map<wstring_view, double> &vocab2freq);
double sumlog(const vector<double>& log_values, const uint start, const uint n);
double sumlog(const double* log_values, uint n);
void em_iter_T(wstring_view T, const unordered_map<wstring_view, double>& vocab2freq, const double freq_sum,
               unordered_map<wstring_view, double>& vocab2freq_T, unordered_map<wstring_view, double>& vocab2r_T,
               const uint max_len, const double lamb, bool calculate_r=true);
void topwords_em(const vector<wstring>& corpus, unordered_map<wstring_view, double>& vocab2freq, unordered_map<wstring_view, double>& vocab2psi,
                        const uint n_iter=10, const double freq_threshold=1e-3, const uint max_len=6, const double lamb=1e-5, bool verbose=true, int n_jobs=-1);
void topwords_em(const vector<string>& corpus, unordered_map<string, double>& vocab2freq, unordered_map<string, double>& vocab2psi, const string& loc="",
                        const uint n_iter=10, const double freq_threshold=1e-3, const uint max_len=6, const double lamb=1e-5, bool verbose=true, int n_jobs=-1);
void topwords_em(const string& input_txt, const string& vocab2freq_txt, const string& vocab2psi_txt, const string& sep="|", const string& loc="",
                 const uint n_iter=10, const double freq_threshold=1e-3, const uint max_len=6, const double lamb=1e-5, bool verbose=true, int n_jobs=-1);
void print_vocab_dict(ostream& out, const unordered_map<wstring_view, double>& vocab_dict, bool reverse=true, bool sort_by_value=true, string_view sep=" ");
void print_vocab_dict(const string& path, const unordered_map<wstring_view, double>& vocab_dict, bool reverse=true, bool sort_by_value=true, string_view sep="|");


template <typename T1, typename T2>
void print_pairs(ostream& out, const vector< pair<T1, T2> >& pairs, string_view sep=" ") {
    for (const auto & p : pairs)
        out << p.first << sep << p.second << endl;
}

template <typename T1, typename T2>
void fout_pairs(string_view path, const vector< pair<T1, T2> >& pairs, string_view sep="|") {
    ofstream fout(path);
    print_pairs(fout, pairs, sep);
    fout.close();
}

template <typename K_TYPE, typename V_TYPE>
void dict2pairs(const unordered_map<K_TYPE, V_TYPE>& d, vector< pair<K_TYPE, V_TYPE> >& pairs) {
    for (const auto & item: d)
        pairs.emplace_back(item);
}

template <typename V_TYPE>
void wsvdict2spairs(const unordered_map<wstring_view, V_TYPE>& d, vector< pair<string, V_TYPE> >& pairs, const string& loc="") {
    for (const auto & item: d)
        pairs.emplace_back(make_pair(ws2s(item.first, loc), item.second));
}

template <typename K_TYPE, typename V_TYPE>
inline V_TYPE dict_get(const unordered_map<K_TYPE, V_TYPE>& d, const K_TYPE& k, const V_TYPE& default_v) {
    auto it = d.find(k);
    if (it == d.end()) // not found
        return default_v;
    return it->second;
}

template <typename K_TYPE, typename V_TYPE>
inline void dict_add(unordered_map<K_TYPE, V_TYPE>& d, const K_TYPE& k, const V_TYPE& v_add) {
    auto it = d.find(k);
    if (it == d.end()) // not found
        d.emplace(make_pair(k, v_add));
    else
        it->second += v_add;
}

#endif

