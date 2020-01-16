/**
* @Author: Yu Huang
* @Email: yuhuang-cst@foxmail.com
*/

#include <iomanip>
#include "../topwords/topwords_lib.h"

using namespace std;

void output_result(const unordered_map<wstring_view, double>& vocab2freq, const unordered_map<wstring_view, double>& vocab2psi, int topk=200, int step=10) {
    vector< pair<string, double> > pairs;
    for (const auto & item: vocab2freq)
        pairs.emplace_back(make_pair(ws2s(item.first), item.second));
    sort(pairs.begin(), pairs.end(), KeyValueComp<string, double>());

    cout.setf(ios::fixed);
    cout << "Top " << topk << " words (sorted by frequency):" << endl;
    for (int i = 0; i < topk; ++i) {
        cout << pairs[i].first << " " << setprecision(2) << pairs[i].second << "; ";
        if ((i+1) % step == 0) cout << endl;
    }
    cout << endl;

    pairs.clear();
    for (const auto & item: vocab2psi)
        pairs.emplace_back(make_pair(ws2s(item.first), item.second));
    sort(pairs.begin(), pairs.end(), KeyValueComp<string, double>());
    cout << "Top " << topk << " words (sorted by psi):" << endl;
    for (int i = 0; i < topk; ++i) {
        cout << pairs[i].first << " " << setprecision(2) << pairs[i].second << "; ";
        if ((i+1) % step == 0) cout << endl;
    }
    cout << endl;
}

void process1() {
    cout << "process1: =========================" << endl;
    string input_txt = "./story_of_stone/corpus.txt";
    vector<wstring> corpus;
    read_wlines(input_txt, corpus);
    unordered_map<wstring_view, double> vocab2freq, vocab2psi;
    topwords_em(corpus, vocab2freq, vocab2psi, 10, 1.0, 4, 1e-5, true, -1);
    output_result(vocab2freq, vocab2psi);
}

void process2() {
    cout << "process2: =========================" << endl;
    string input_txt = "./story_of_stone/corpus.txt";
    string vocab2freq_txt = "./cpp_vocab2freq.txt";
    string vocab2psi_txt = "./cpp_vocab2psi.txt";
    topwords_em(input_txt, vocab2freq_txt, vocab2psi_txt, "|", "", 10, 1.0, 4, 1e-5, true, -1);
}

int main() {
    process1();
    process2();
    return 0;
}