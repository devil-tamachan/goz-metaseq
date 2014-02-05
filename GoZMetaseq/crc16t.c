//C����ɂ��ŐV�A���S���Y�����T
//�������F�wC����ɂ��ŐV�A���S���Y�����T�x �Z�p�]�_�ЁC1991�N�CISBN4-87408-414-1
//http://oku.edu.mie-u.ac.jp/~okumura/algo/
//http://www.vector.co.jp/soft/data/prog/se002453.html
//license(�z�z��Vector������p): �����p�ɂ��Ă̐����͂���܂���B�������o�O�ɂ�鑹�Q�̔����Ȃǂɂ͉������˂܂��̂ł��������������B�o�O�𔭌����ꂽ�炨�m�点����������΍K���ł�.
//

/***********************************************************
	crc16t.c -- CRC
***********************************************************/
#include <limits.h>
#define CRCPOLY1  0x1021U  /* x^{16}+x^{12}+x^5+1 */
#define CRCPOLY2  0x8408U  /* ���E�t�] */
typedef unsigned char byte;
unsigned int crctable[UCHAR_MAX + 1];

void makecrctable1(void)
{
	unsigned int i, j, r;

	for (i = 0; i <= UCHAR_MAX; i++) {
		r = i << (16 - CHAR_BIT);
		for (j = 0; j < CHAR_BIT; j++)
			if (r & 0x8000U) r = (r << 1) ^ CRCPOLY1;
			else             r <<= 1;
		crctable[i] = r & 0xFFFFU;
	}
}

unsigned int crc1(int n, byte c[])
{
	unsigned int r;

	r = 0xFFFFU;
	while (--n >= 0)
		r = (r << CHAR_BIT) ^ crctable[(byte)(r >> (16 - CHAR_BIT)) ^ *c++];
	return ~r & 0xFFFFU;
}
/*
void makecrctable2(void)
{
	unsigned int i, j, r;

	for (i = 0; i <= UCHAR_MAX; i++) {
		r = i;
		for (j = 0; j < CHAR_BIT; j++)
			if (r & 1) r = (r >> 1) ^ CRCPOLY2;
			else       r >>= 1;
		crctable[i] = r;
	}
}

unsigned int crc2(int n, byte c[])
{
	unsigned int r;

	r = 0xFFFFU;
	while (--n >= 0)
		r = (r >> CHAR_BIT) ^ crctable[(byte)r ^ *c++];
	return r ^ 0xFFFFU;
}

#include <stdio.h>
#include <stdlib.h>
int main()
{
	static byte s[] = "Hello, world!";

	makecrctable1();
	printf("crc1(%s) = %04X\n", s, crc1(13, s));
	makecrctable2();
	printf("crc2(%s) = %04X\n", s, crc2(13, s));
	return EXIT_SUCCESS;
}
*/