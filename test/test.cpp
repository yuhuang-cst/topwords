/**
* @Author: Yu Huang
* @Email: yuhuang-cst@foxmail.com
*/

#include <cassert>
#include <cstring>
#include "../topwords/topwords_lib.h"

using namespace std;

void test_wstring() {
    string s = "我爱c"; // utf-8
    assert(string("我爱").size() == 3 + 3);
    assert(string("我爱c").size() == 3 + 3 + 1);
    assert(wstring(L"我爱").size() == 2); assert(wcslen(wstring(L"我爱").c_str()) == 2);
    assert(wstring(L"我爱c").size() == 3); assert(wcslen(wstring(L"我爱c").c_str()) == 3);

    wstring ws = L"我爱中国"; // unicode
    assert(ws[4] == L'\0');
    assert(wcslen(ws.c_str()) == 4);
    wchar tmp = ws[2]; ws[2] = L'\0';
    assert(wcslen(ws.c_str()) == 2);
    ws[2] = tmp;

    assert(wcslen(ws.c_str()) == 4);
    tmp = ws[4]; ws[4] = L'\0';
    assert(wcslen(ws.c_str()) == 4);
    ws[4] = tmp;
    assert(wcslen(ws.c_str()) == 4);
}

void test_dict_add() {
    unordered_map<string, double> d;
    dict_add(d, string("hello"), 1.0);
    dict_add(d, string("hello"), 2.0);
    dict_add(d, string("world"), 1.0);
    assert(d["hello"] == 3.0);
    assert(d["world"] == 1.0);
}

void test_max_ele() {
    double a[5] = {6.0, 8.1, 2.2, 3.3, 4.4};
    double max_v = *max_element(a+2, a+5);
    assert(max_v == 4.4);

    a[0] = DOUBLE_INF; a[1] = -DOUBLE_INF;
    max_v = *max_element(a, a+5);
    assert(max_v == DOUBLE_INF);
}

void test_sumlog() {
    double log_probs[5] = {log(5), log(2), log(1), log(0), log(0)};
    assert(sumlog(log_probs, 5) == log(8));
    assert(sumlog(log_probs+3, 2) == log(0));
}

void test_sortinf() {
    double a[5] = {DOUBLE_INF, 8.1, 2.2, DOUBLE_NEG_INF, 4.4};
    sort(a, a + 5);
    cout << "test sortinf: ";
    for (int i = 0; i < 5; ++i)
        cout << a[i] << " ";
    cout << endl;
}

void test_isinf() {
    assert(!is_posinf(log(0.0)));
    assert(is_neginf(log(0.0)));
    assert(log(0.0) == DOUBLE_NEG_INF);
}

void test_corpus_em() {
    cout << "test_corpus_em begin =======================" << endl;
    vector<wstring> corpus;
    corpus.emplace_back(L"我爱北京天安门");
    corpus.emplace_back(L"天安门真雄伟");
    corpus.emplace_back(L"天安门在北京");
    corpus.emplace_back(L"北京是个大城市");
    corpus.emplace_back(L"北京这个城市适合我");
    corpus.emplace_back(L"天安门挂着毛主席画像");
    corpus.emplace_back(L"毛主席真伟大");
    corpus.emplace_back(L"毛主席厉害");
    corpus.emplace_back(L"毛主席牛");
    corpus.emplace_back(L"硬币上印着毛主席");
    unordered_map<wstring_view, double> vocab2freq, vocab2psi;
    topwords_em(corpus, vocab2freq, vocab2psi, 10, 1e-3,4, 1e-5);
    cout << "Frequency: --------------------" << endl;
    print_vocab_dict(cout, vocab2freq);
    cout << "Importance score: -------------------" << endl;
    print_vocab_dict(cout, vocab2psi);
    cout << "test_corpus_em end =======================" << endl;
}

void test_file_em() {
    cout << "test_file_em begin =======================" << endl;
    string input_txt = "test_corpus.txt";
    string vocab2freq_txt = "vocab2freq.txt";
    string vocab2psi_txt = "vocab2psi.txt";
    topwords_em(input_txt, vocab2freq_txt, vocab2psi_txt, "|", "", 10, 1e-3, 4, 1e-5);
    cout << "test_file_em end =======================" << endl;
}

int main() {
    test_wstring();
    test_dict_add();
    test_max_ele();
    test_sumlog();
    test_sortinf();
    test_isinf();
    test_corpus_em();
    test_file_em();
    cout << "All tests pass!" << endl;
    return 0;
}

