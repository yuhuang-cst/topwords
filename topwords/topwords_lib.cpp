/**
* @Author: Yu Huang
* @Email: yuhuang-cst@foxmail.com
*/

#include "topwords_lib.h"

wstring s2ws(const string& src, const string& loc) {
    setlocale(LC_CTYPE, loc.c_str());
    uint ws_len = mbstowcs(NULL, src.c_str(), 0);
    wchar *p = new wchar[ws_len + 1];
    mbstowcs(p, src.c_str(), src.size());
    wstring dest(p, ws_len);
    delete[] p;
    return dest;
}

wstring s2ws(string_view src, const string& loc) {
    return s2ws(string(src.data(), src.size()));
}

string ws2s(const wstring& src, const string& loc) {
    setlocale(LC_CTYPE, loc.c_str());
    uint s_len = wcstombs(NULL, src.c_str(), 0);
    char *p = new char[s_len + 1];
    wcstombs(p, src.c_str(), s_len);
    string dest(p, s_len);
    delete []p;
    return dest;
}

string ws2s(wstring_view src, const string& loc) {
    return ws2s(wstring(src.data(), src.size()));
}

void read_wlines(const string& file_path, vector<wstring>& wlines, const string& loc) {
    ifstream fin(file_path);
    uint line_count = 0;
    string line;
    while (!fin.eof()) {
        line_count += 1;
        getline(fin, line);
        wlines.emplace_back(s2ws(line, loc));
    }
    fin.close();
}

void write_wlines(const string& file_path, const vector<wstring>& wlines, const string& loc) {
    ofstream fout(file_path);
    for (const auto & wline : wlines)
        fout << ws2s(wline, loc) << endl;
    fout.close();
}

bool compare_func(const pair<string, double>& p1, const pair<string, double>& p2) {
    return p1.second > p2.second;
}

void print_vocab_dict(ostream& out, const unordered_map<wstring_view, double>& vocab_dict, bool reverse, bool sort_by_value, string_view sep) {
    vector< pair<string, double> > pairs;
    wsvdict2spairs(vocab_dict, pairs);
    sort(pairs.begin(), pairs.end(), KeyValueComp<string, double>(reverse, sort_by_value));
    print_pairs(out, pairs, sep);
}

void print_vocab_dict(const string& path, const unordered_map<wstring_view, double>& vocab_dict, bool reverse, bool sort_by_value, string_view sep) {
    ofstream fout(path);
    print_vocab_dict(fout, vocab_dict, reverse, sort_by_value, sep);
    fout.close();
}

void get_all_ngram(wstring_view T, const uint max_len, unordered_set<wstring_view> &ngram_set) {
    uint T_len = T.size();
    uint max_len_s = min(T_len, max_len);
    for (uint l = 1; l <= max_len_s; ++l)
        for (uint i = 0; i < T_len - l + 1; ++i)
            ngram_set.emplace(T.substr(i, l));
}

void init_vocab2freq(const vector<wstring> &corpus, uint max_len, unordered_map<wstring_view, double> &vocab2freq) {
    //FIXME: parallel
    for (const auto & T: corpus) {
        unordered_set<wstring_view> ngram_set;
        get_all_ngram(T, max_len, ngram_set);
        for (const auto & term: ngram_set)
            if (vocab2freq.find(term) == vocab2freq.end()) // not found
                vocab2freq.emplace(make_pair(term, 1.0));
    }
}

/**
 * @param log_values [log(x_0), log(x_1), ...]
 * @return log(x_0 + x_1 + ...)
 * */
double sumlog(const vector<double>& log_values, const uint start, const uint n) {
    double max_log_value = *max_element(&log_values[start], &log_values[start+n]);
    if (is_neginf(max_log_value)) return DOUBLE_NEG_INF;
    double ret_log = 0.0;
    for(uint i = start; i < n; ++i)
        ret_log += exp(log_values[i] - max_log_value);
    ret_log = log(ret_log) + max_log_value;
    return ret_log;
}

/**
 * @param log_values [log(x_0), log(x_1), ...log(x_{n-1})]
 * @param n Number of values to include in the calculation
 * @return log(x_0 + x_1 + ... + x_{n-1})
 * */
double sumlog(const double* log_values, uint n) {
    double max_log_value = *max_element(log_values, log_values + n);
    if (is_neginf(max_log_value)) return DOUBLE_NEG_INF;
    double ret_log = 0.0;
    for (uint i = 0; i < n; ++i)
        ret_log += exp(log_values[i] - max_log_value);
    ret_log = log(ret_log) + max_log_value;
    return ret_log;
}

/**
 * @param[in] T text
 * @param[in] vocab2freq All vocabulary and frequency from last iteration
 * @param[in] freq_sum Total sum of vocabulary frequency
 * @param[out] vocab2freq_T Output vocabulary and frequency by processing the input text: {term: frequency}
 * @param[out] vocab2r_T Output vocabulary and score r: {term: r}
 * @param[in] max_len Maximum length of segmented term
 * @param[in] lamb Smooth factor when calculating word probability
 * @param[in] calculate_r Whether to calculate @param vocab2r_T
 * */
void em_iter_T(wstring_view T, const unordered_map<wstring_view, double>& vocab2freq, const double freq_sum,
        unordered_map<wstring_view, double>& vocab2freq_T, unordered_map<wstring_view, double>& vocab2r_T,
               const uint max_len, const double lamb, bool calculate_r) {
    uint vocab_size = vocab2freq.size();
    uint T_len = T.size();

    double *log_pt = new double[T_len + 1]; // pt = P(T_{|>=t|}); t = 0, 1, 2, ..., T_len
    unordered_map<wstring_view, double> *nt = new unordered_map<wstring_view, double>[T_len + 1]; // nt = {w_i: n_i(T_{|>=t|})}
    unordered_map<wstring_view, double> *rt = new unordered_map<wstring_view, double>[T_len + 1]; // rt = {w_i: r_i(T_{>=t})}
    double *log_probs = new double[T_len];

    log_pt[T_len] = 0.0; // dp init
    uint max_l;
    double freq, w_log_prob, log_pt_sum, rou_l;

    for (uint t = T_len - 1; t + 1 > 0; --t) { // consider T_[t, T_len); Note: use t + 1 > 0 instead of t > -1 to solve the problem caused by uint
        max_l = min(T_len - t, max_len);
        for (uint l = 1; l <= max_l; ++l) { // w_i = T_[t, t+l)
            wstring_view w = T.substr(t, l); // w = T_[t, t+l)
            freq = dict_get(vocab2freq, w, 0.0);
            w_log_prob = log((freq + lamb) / (freq_sum + vocab_size * lamb)); // smooth probability
            log_probs[l-1] = w_log_prob + log_pt[t+l]; // log_probs[l-1] = log \theta_[t: t+l) + log_pt[t+l]
        }
        log_pt[t] = log_pt_sum = sumlog(log_probs, max_l);

        unordered_map<wstring_view, double>& w2freq = nt[t];
        unordered_map<wstring_view, double>& w2r = rt[t];
        for (uint l = 1; l <= max_l; ++l) { // w_i = T_[t, t+l)
            rou_l = exp(log_probs[l-1] - log_pt_sum);
            wstring_view w = T.substr(t, l); // w = T_[t, t+l)
            dict_add(w2freq, w, rou_l);
            for (const auto & item: nt[t+l])
                dict_add(w2freq, item.first, item.second * rou_l);
            if (calculate_r) {
                dict_add(w2r, w, rou_l);
                for (const auto & item: rt[t+l]) {
                    if (item.first == w) continue;
                    dict_add(w2r, item.first, item.second * rou_l);
                }
            }
        }
    }
    vocab2freq_T = nt[0]; vocab2r_T = rt[0];
    delete []log_pt; delete []nt; delete []rt; delete []log_probs;
}

/**
 * @param[in] corpus list of unsegmented text T
 * @param[in, out] vocab2freq Vocabulary and frequency: {term: n_term}; will be initialized if empty
 * @param[out] vocab2phi Vocabulary and importance score @f$\phi@f$: {term: phi}
 * @param[in] n_iter Iteration number of EM algorithm
 * @param[in] freq_threshold Candidate words whose frequencies < freq_threshold will be dropped in each iteration
 * @param[in] max_len Maximum length of segmented term
 * @param[in] lamb Smooth factor when calculating word probability
 * @param[in] verbose Whether to print details when running EM algorithm
 * @param[in] n_jobs Number of jobs for parallel processing; -1 means using all cores
 * */
void topwords_em(const vector<wstring>& corpus, unordered_map<wstring_view, double>& vocab2freq, unordered_map<wstring_view, double>& vocab2phi,
               const uint n_iter, const double freq_threshold, const uint max_len, const double lamb, bool verbose, int n_jobs) {
    #ifdef _OPENMP
    omp_set_num_threads(n_jobs == -1? omp_get_num_procs(): n_jobs);
    #endif
    Logger logger(cout, verbose);
    if (vocab2freq.empty()) {
        logger.info("initializing vocabulary...");
        init_vocab2freq(corpus, max_len, vocab2freq);
        logger.info("vocabulary initilazation done.");
    }
    unordered_map<wstring_view, double> vocab2freq_next;

    int T_num = corpus.size();
    for (uint i = 1; i <= n_iter; ++i) {
        double freq_sum = 0.0;
        for (const auto & item: vocab2freq)
            freq_sum += item.second;
        logger.info("Iter =", i, "/", n_iter, ", vocab size =", vocab2freq.size());
        #pragma omp parallel for
        for (int j = 0; j < T_num; ++j) {
            wstring_view T = corpus[j];
            unordered_map<wstring_view, double> vocab2freq_T;
            unordered_map<wstring_view, double> vocab2r_T;
            em_iter_T(T, vocab2freq, freq_sum, vocab2freq_T, vocab2r_T, max_len, lamb, i==n_iter);

            #pragma omp critical
            {
                for (const auto & item: vocab2freq_T)
                    dict_add(vocab2freq_next, item.first, item.second);
                if (i == n_iter) {
                    for (const auto & item: vocab2r_T)
                        dict_add(vocab2phi, item.first, -log(1.0 - item.second));
                }
            }
        }
        vocab2freq = vocab2freq_next;
        for (auto it = vocab2freq.cbegin(); it != vocab2freq.cend();)
            it->second < freq_threshold? vocab2freq.erase(it++): ++it;
        vocab2freq_next.clear();
    }
    for (auto it = vocab2phi.cbegin(); it != vocab2phi.cend();)
        vocab2freq.find(it->first) == vocab2freq.end()? vocab2phi.erase(it++): ++it;
    logger.info("Final vocab size =", vocab2freq.size());
}

/**
* Interface for cython
*/
void topwords_em(const vector<string>& corpus, unordered_map<string, double>& vocab2freq, unordered_map<string, double>& vocab2phi, const string& loc,
                const uint n_iter, const double freq_threshold, const uint max_len, const double lamb, bool verbose, int n_jobs) {
    vector<wstring> wcorpus(corpus.size());
    for (const auto & s: corpus)
        wcorpus.emplace_back(s2ws(s, loc));
    unordered_map<wstring_view, double> wvocab2freq;
    unordered_map<wstring_view, double> wvocab2phi;
    topwords_em(wcorpus, wvocab2freq, wvocab2phi, n_iter, freq_threshold, max_len, lamb, verbose, n_jobs);
    for (const auto & item: wvocab2freq)
        vocab2freq.emplace(make_pair(ws2s(item.first), item.second));
    for (const auto & item: wvocab2phi)
        vocab2phi.emplace(make_pair(ws2s(item.first), item.second));
}

/**
 * @param[in] input_txt System path of input file with each line representing each T
 * @param[in] vocab2freq_txt System path of output file to store vocabulary and frequency
 * @param[in] vocab2phi_txt System path of output file to store vocabulary and phi
 * @param[in] sep Separation string when writing output file
 * @param[in] loc Locale of transformation between wstring and string
 * @param[in] n_iter Iteration number of EM algorithm
 * @param[in] freq_threshold Candidate words whose frequencies < freq_threshold will be dropped in each iteration
 * @param[in] max_len Maximum length of segmented term
 * @param[in] lamb Smooth factor when calculating word probability
 * @param[in] verbose Whether to print details when running EM algorithm
 * @param[in] n_jobs Number of jobs for parallel processing; -1 means using all cores
 * */
void topwords_em(const string& input_txt, const string& vocab2freq_txt, const string& vocab2phi_txt, const string& sep, const string& loc,
             const uint n_iter, const double freq_threshold, const uint max_len, const double lamb, bool verbose, int n_jobs) {
    Logger logger(cout, verbose);
    vector<wstring> corpus;
    read_wlines(input_txt, corpus, loc);
    unordered_map<wstring_view, double> vocab2freq, vocab2phi;
    topwords_em(corpus, vocab2freq, vocab2phi, n_iter, freq_threshold, max_len, lamb, verbose);
    logger.info("Writing vocabulary and frequency...");
    print_vocab_dict(vocab2freq_txt, vocab2freq);
    logger.info("Writing vocabulary and importance score...");
    print_vocab_dict(vocab2phi_txt, vocab2phi);
}
