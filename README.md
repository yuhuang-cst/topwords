# TopWORDS
This is an implementation of **TopWORDS**. TopWORDS is 
an unigram language model, which assumes words are independent and 
the probability of a sentence is the cumprod of probablitie of words.
We use EM algorithm to optimize parameters.

Reference: 

[1] Deng, K. (2016). On the unsupervised analysis of domain-specific Chinese texts.

# Installation
We implemented TopWORDS in **C++** and also provided a **Python** wrapper using Cython. `gcc-7` compiler (for `-std=c++17`) and `OpenMP` 
parallel library (for `-fopenmp`) are needed to install this package. 

## C++
Write a `.cpp` and includes `topwords/topwords_lib.h` header. For example, the demo program `examples/main.cpp` looks like:

```cpp
#include "../topwords/topwords_lib.h"

using namespace std;

void output_result(const unordered_map<wstring_view, double>& vocab2freq, const unordered_map<wstring_view, double>& vocab2phi, int topk=200, int step=10) {
    // ...
}

void process1() {
    cout << "process1: =========================" << endl;
    string input_txt = "./story_of_stone/corpus.txt";
    vector<wstring> corpus;
    read_wlines(input_txt, corpus);
    unordered_map<wstring_view, double> vocab2freq, vocab2phi;
    topwords_em(corpus, vocab2freq, vocab2phi, 10, 1.0, 4, 1e-5, true, -1);
    output_result(vocab2freq, vocab2phi);
}

void process2() {
    cout << "process2: =========================" << endl;
    string input_txt = "./story_of_stone/corpus.txt";
    string vocab2freq_txt = "./cpp_vocab2freq.txt";
    string vocab2phi_txt = "./cpp_vocab2phi.txt";
    topwords_em(input_txt, vocab2freq_txt, vocab2phi_txt, "|", "", 10, 1.0, 4, 1e-5, true, -1);
}

int main() {
    process1();
    process2();
    return 0;
}
```

Then run the `Makefile` to generate Executable program:

```
cd examples
make
./main
```

## Python
If you are not familiar with C++, you can use the python wrapper directly. Simply run the setup script to install:

```
python3 setup.py install
```

or install from git repo:

```
pip3 install git+https://github.com/yuhuang-cst/topwords.git
```

The demo program `examples/main.py` looks like:

```python
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
		if (i % 10 == 0): print('')
	print('')

	print('Top 200 words (sorted by phi):')
	vocab_phi = sorted(vocab2phi.items(), key=lambda item:item[1], reverse=True)[:topk]
	for i, (word, freq) in enumerate(vocab_phi):
		print('{} {:.2f}; '.format(word, freq), end='')
		if (i % 10 == 0): print('')
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
```

# Demo
The output of the demo looks like: (Full results are located in `examples/`)

```
process1: =========================
initializing vocabulary...
vocabulary initilazation done.
Iter = 1 / 10 , vocab size = 799822
Iter = 2 / 10 , vocab size = 22955
Iter = 3 / 10 , vocab size = 21846
Iter = 4 / 10 , vocab size = 20684
Iter = 5 / 10 , vocab size = 20295
Iter = 6 / 10 , vocab size = 20091
Iter = 7 / 10 , vocab size = 19980
Iter = 8 / 10 , vocab size = 19909
Iter = 9 / 10 , vocab size = 19871
Iter = 10 / 10 , vocab size = 19854
Final vocab size = 19839
Top 200 words (sorted by frequency):
的 4230.70; 了 3815.96; 我 2030.22; 你 1941.54; 来 1910.58; 他 1714.86; 宝玉 1443.93; 说 1355.07; 又 1312.92; 也 1229.71; 
去 1190.71; 便 1121.88; 人 986.92; 这 973.97; 那 932.89; 呢 894.50; 是 878.41; 不 767.89; 就 750.02; 都 746.56; 
笑道 694.93; 道 667.20; 说着 648.27; 有 644.85; 王夫人 641.05; 要 610.88; 老太太 604.00; 我们 600.53; 一 590.54; 如今 589.79; 
好 579.68; 因 574.01; 在 559.62; 上 529.44; 凤姐 524.75; 贾母 518.53; 你们 508.19; 只 506.10; 着 505.92; 还 487.39; 
等 470.11; 说道 466.90; 宝钗 463.93; 叫 461.42; 黛玉 453.30; 和 450.92; 去了 446.22; 儿 444.65; 宝玉道 441.40; 袭人 438.81; 
一个 427.11; 姑娘 421.23; 自己 413.54; 出来 409.02; 再 405.93; 见 401.10; 他们 396.15; 将 395.06; 看 386.03; 贾琏 380.17; 
罢 378.53; 来了 377.60; 只见 373.79; 到 371.53; 才 369.74; 贾政 368.79; 倒 367.39; 子 356.25; 起来 351.82; 把 341.09; 
先 333.71; 凤姐儿 332.88; ` 330.14; 什么 329.43; 只是 324.14; 太太 317.73; 出 317.53; 得 317.29; 一面 314.00; 进来 313.81; 
回来 313.26; 平儿 312.83; 问 311.70; 不知 311.10; 怎么 308.06; 薛姨妈 306.61; 大 304.71; 过 304.65; 奶奶 300.50; 所以 300.36; 
大家 299.56; 老爷 299.52; 一时 296.73; 时 293.47; 众人 289.76; 咱们 287.66; 与 282.37; 下 281.72; 不是 281.13; 没有 280.40; 
忙 280.18; 方 275.88; 若 274.12; 那里 272.78; 些 271.14; 这个 270.12; 打 268.36; 只得 266.35; 家 266.17; 事 265.69; 
这里 260.65; 请 257.67; 过来 256.03; 二爷 254.63; 吃 253.51; 听见 253.04; 作 252.45; 别 251.18; 的人 250.35; 正 249.78; 
今日 249.20; 连 248.78; 为 247.09; 的话 244.22; 出去 242.53; 这样 239.47; 同 237.77; 已 237.70; 宝玉笑道 233.60; 可 231.84; 
听了 230.38; 紫鹃 229.53; 自 229.50; 回 228.76; 个 221.49; 往 221.27; 却 216.75; 就是 212.68; 心 211.44; 谁 211.43; 
林黛玉 209.79; 的事 208.84; 邢夫人 207.73; 没 206.58; 给 205.50; 多 205.22; 竟 204.91; 探春 204.20; 也不 203.27; 送 202.88; 
不过 202.46; 向 201.20; 也是 199.17; 且 199.09; 命 198.46; 听 198.03; 鸳鸯 196.40; 拿 195.91; 知道 194.03; 姐姐 193.98; 
刘姥姥 191.65; 从 190.66; 都是 190.38; 晴雯 189.49; 找 188.96; 贾珍 188.46; 贾母道 188.11; 起 188.05; 想 187.14; 头 181.02; 
见了 178.18; 两个 177.95; 么 176.90; 早 176.11; 气 175.82; 如何 175.52; 叫他 174.25; 今儿 172.41; 用 171.25; 小 170.25; 
话 169.34; 在这里 168.31; 袭人道 168.17; 们 168.07; 东西 168.06; 周瑞家的 167.90; 比 167.43; 贾政道 166.17; 并 165.99; 薛蟠 165.91; 
香菱 164.69; 心里 164.09; 宝玉听了 162.54; 也有 161.88; 不好 161.42; 做 160.77; 李纨 159.46; 凤姐道 158.99; 进去 158.62; 只管 158.09; 

Top 200 words (sorted by phi):
桌围 inf; 残菊 inf; 港台 inf; 扇坠 inf; 菩萨 inf; 疯疯癫癫 inf; 科幻 inf; 慧绣 inf; 噗哧 inf; 轰去魂魄 inf; 
豆腐 inf; 咭咭呱呱 inf; 忆菊 inf; 蛇影杯弓 inf; 妆狐媚 inf; 孽障 inf; 琥珀 inf; 呜呜咽咽 inf; 杏帘 inf; 咕咚 inf; 
慧纹 inf; 唐伯虎 inf; 稻香老农 inf; 皱一回眉 inf; 浣葛山庄 inf; 拾头打滚 inf; 桌椅 inf; 潇湘馆 inf; 绛云轩 inf; 狗彘奴欺 inf; 
容俊俏 inf; 颓丧 inf; 冰片 inf; 惹人厌 inf; 鹦鹉 inf; 骢马使弹 inf; 抖衣而颤 inf; 诟谇谣诼 inf; 影影绰绰 inf; 魂不附体 inf; 
咳嗽 inf; 蘅芷清芬 inf; 簪菊 inf; 乞丐 inf; 葫芦 inf; 摇了一摇 inf; 蓼风轩 inf; 射覆 inf; 残忍乖僻 inf; 鸳鸯 inf; 
欺负 inf; 嗳哟 inf; 菊影 inf; 稻香村 inf; 吩咐 inf; 痰盒 inf; 呜呼 inf; 荡悠悠 inf; 蓼汀花溆 inf; —— inf; 
鸦雀无闻 inf; 吱喽 inf; 访菊 inf; 咏菊 inf; 婆婆 inf; 绮霰 inf; 藕香榭 inf; 陈设 inf; 爽利 inf; 钥匙 inf; 
飘飘摇摇 inf; 咯吱 inf; 蜡烛 inf; 曹雪芹 inf; 伴鹤 inf; 耗子精 inf; 荣二宅 inf; 卯正二刻 inf; 硬作保山 inf; 碧痕 inf; 
婶婶 inf; 绣桔 inf; 汉宫春晓 inf; …… inf; 牡丹亭 inf; 逢凶化吉 inf; 锞十锭 inf; 苇叶 inf; 偕鸳 inf; 浩浩荡荡 inf; 
鲸卿 inf; 武侠 inf; 渺茫 inf; 漱盂 inf; 枉费 inf; 杀律 inf; 陈皮 inf; 祭祀 inf; 供菊 inf; 巾帕 inf; 
威赫 inf; 绣鸾 inf; 粗鄙 inf; 冲一冲 inf; 温飞卿 inf; 玻璃 inf; 鸡蛋 inf; 荇叶渚 inf; 守庚申 inf; и着鞋 inf; 
嬷嬷 inf; 椅搭 inf; 蘼芜 inf; 尤氏 inf; 的 18290.63; 了 14816.04; 宝玉 8510.77; 你 7341.34; 来 7077.09; 我 7014.89; 
他 6271.53; 说 5868.91; 又 5565.78; 便 5542.95; 也 4734.43; 王夫人 4610.45; 去 4571.93; 笑道 4163.00; 呢 4113.70; 那 4029.09; 
这 3875.85; 凤姐 3666.55; 如今 3544.68; 都 3443.05; 老太太 3345.54; 说着 3315.36; 道 3310.15; 宝钗 3287.71; 黛玉 3195.22; 就 3162.51; 
人 3109.81; 贾母 3069.25; 是 3007.89; 因 2904.07; 贾琏 2801.28; 姑娘 2725.29; 袭人 2608.52; 薛姨妈 2510.03; 贾政 2451.42; 我们 2443.03; 
自己 2419.29; 好 2291.90; 要 2270.59; 只 2145.63; 不 2124.75; 有 2116.86; 和 2106.30; 宝玉道 2078.45; 你们 2046.33; 等 2031.82; 
叫 1986.99; 在 1972.22; 紫鹃 1963.04; 奶奶 1955.59; 所以 1919.55; 说道 1914.12; ` 1807.24; 再 1803.39; 平儿 1802.89; 还 1800.45; 
刘姥姥 1789.22; 才 1787.87; 将 1771.02; 什么 1753.53; 周瑞家的 1741.98; 罢 1736.18; 上 1730.33; 太太 1714.37; 怎么 1704.66; 咱们 1704.44; 
邢夫人 1701.42; 只见 1665.50; 一 1640.03; 老爷 1629.90; 一面 1589.77; 先 1578.09; 着 1566.62; 见 1540.10; 今日 1532.92; 二爷 1530.59; 
儿 1528.16; 看 1525.06; 探春 1510.26; 晴雯 1501.63; 大家 1500.79; 一个 1491.98; 倒 1487.99; 他们 1477.78; 出来 1473.27; 把 1472.82; 

process2: =========================
initializing vocabulary...
vocabulary initilazation done.
Iter = 1 / 10 , vocab size = 799822
Iter = 2 / 10 , vocab size = 22955
Iter = 3 / 10 , vocab size = 21846
Iter = 4 / 10 , vocab size = 20684
Iter = 5 / 10 , vocab size = 20295
Iter = 6 / 10 , vocab size = 20091
Iter = 7 / 10 , vocab size = 19980
Iter = 8 / 10 , vocab size = 19909
Iter = 9 / 10 , vocab size = 19871
Iter = 10 / 10 , vocab size = 19854
Final vocab size = 19839
Writing vocabulary and frequency...
Writing vocabulary and importance score...
```

# Theory

## Notation
- Characters: $A = \{a_1, a_2, ..., a_p\}$
- Vocabulary: $D = \{w_1, w_2, ..., w_N\}$, $w = a_{i_1}a_{i_2}...a_{i_l}$
- Word probability: $\mathbf{\theta} = (\theta_1, ..., \theta_N)$
- K-word (segmented) sentence: $S = w_{i_1}, w_{i_2}, ..., w_{i_K}$









