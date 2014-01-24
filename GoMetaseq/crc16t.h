//C言語による最新アルゴリズム事典
//奥村晴彦『C言語による最新アルゴリズム事典』 技術評論社，1991年，ISBN4-87408-414-1
//http://oku.edu.mie-u.ac.jp/~okumura/algo/
//http://www.vector.co.jp/soft/data/prog/se002453.html
//license(配布元Vectorから引用): ご利用についての制限はありません。ただしバグによる損害の賠償などには応じかねますのでご了承ください。バグを発見されたらお知らせいただければ幸いです.
//

#ifdef __cplusplus
extern "C" {
#endif

void makecrctable1(void);
unsigned int crc1(int n, byte c[]);

#ifdef __cplusplus
}
#endif
