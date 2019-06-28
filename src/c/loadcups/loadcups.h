#ifndef LOADCUPS_H
#define LOADCUPS_H

struct t_cups_err{
	char fd1[3+1];
	char fd2[11+1];
	char fd3[11+1];
	char fd4[6+1];
	char fd5[10+1]; 
	char fd6[19+1];
	char fd7[12+1];
	char fd8[4+1];
	char fd9[6+1];
	char fd10[4+1];
	char fd11[8+1];
	char fd12[12+1];
	char fd13[2+1];
	char fd14[6+1];
	char fd15[11+1];
	char fd16[11+1];
	char fd17[6+1];
	char fd18[2+1];
	char fd19[3+1];
	char fd20[12+1];
	char fd21[12+1];
	char fd22[12+1];
	char fd23[12+1];
	char fd24[12+1];
	char fd25[12+1];
	char fd26[4+1];
	char fd27[11+1];
	char fd28[19+1];
	char fd29[11+1];
	char fd30[19+1];
	char fd31[10+1];
	char fd32[3+1];
	char fd33[1+1];
	char fd34[1+1];
	char fd35[4+1];
	char fd36[12+1];
	char fd37[1+1];
	char fd38[2+1];
};
typedef struct t_cups_err T_CUPS_ERR;

struct t_cups_tfl{
	char f1 [11+1];
	char f2 [6+1];
	char f3 [10+1];
	char f4 [19+1];
	char f5 [12+1];
	char f6 [4+1];
	char f7 [6+1];
	char f8 [4+1];
	char f9 [8+1];
	char f10 [12+1];
	char f11 [2+1];
	char f12 [6+1];
	char f13 [11+1];
	char f14 [6+1];
	char f15 [2+1];
	char f16 [3+1];
	char f17 [12+1];
	char f18 [12+1];
	char f19 [12+1];
	char f20 [12+1];
	char f21 [11+1];
	char f22 [19+1];
	char f23 [11+1];
	char f24 [19+1];
	char f25 [3+1];
	char f26 [1+1];
	char f27 [1+1];
	char f28 [1+1];
	char f29 [2+1];
	char f30 [10+1];
	char f31 [11+1];
	char f32 [1+1];
	char f33 [1+1];
	char f34 [19+1];
	char f35 [2+1];
	char t36 [1+1];
	char f37 [15+1];
	char file [129];
};

typedef struct t_cups_tfl T_CUPS_TFL;

struct t_cups_com {
	char f1 [11+1];
	char f2 [11+1];
	char f3 [6+1];
	char f4 [10+1];
	char f5 [19+1];
	char f6 [12+1];
	char f7 [12+1];
	char f8 [12+1];
	char f9 [4+1];
	char f10 [6+1];
	char f11 [4+1];
	char f12 [8+1];
	char f13 [15+1];
	char f14 [12+1];
	char f15 [2+1];
	char f16 [6+1];
	char f17 [11+1];
	char f18 [6+1];
	char f19 [2+1];
	char f20 [3+1];
	char f21 [12+1];
	char f22 [12+1];
	char f23 [12+1];
	char f24 [1+1];
	char f25 [3+1];
	char f26 [1+1];
	char f27 [1+1];
	char f28 [10+1];
	char f29 [11+1];
	char f30 [1+1];
	char f31 [2+1];
	char f32 [2+1];
	char f33 [12+1];
	char f34 [14+1];
	char file [129];
};
typedef struct t_cups_com T_CUPS_COM;


#define MAX_LINE_LEN 2048
#define ARRAY_SIZE( ARRAY ) (sizeof (ARRAY) / sizeof (ARRAY[0]))

typedef int (*FP)(char*);    /*结构体表示函数指针 */

char *rtrim(char *str);
void HexDump(char *buf,int len,int addr);

#endif
