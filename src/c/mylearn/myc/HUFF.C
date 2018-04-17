/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*                                                                  * 
*HUFF.C Huffman encode for multimedia application 8*8 pixel Ver 3  * 
*                                                                  * 
*Ver 1:  Complied in Borland Turbo C++ 3.0                         * 
*Ver 2:  Complied in Microsoft Visual C++ 6.0                      * 
*Ver 3:  Complied in Microsoft Visual C++ 6.0                      * 
*          add code to print code table of the compression         * 
*          print output message in Chinese                         * 
*                                                                  * 
*by Lee Meitz, Solid Mechanics, Huazhong Univ of Sci and Tech      * 
*2001.11.15 - 2001.12.27                                           * 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#define  DNUM 64    //define data number 8*8 
#define  LOOP 10000 //times of compression

typedef struct 
{ 
    unsigned short weight, data; 
    unsigned short parent, lchild, rchild; 
} HuffNode;

typedef struct 
{ 
    unsigned char code; 
    unsigned short codelength; 
} HuffCode;

unsigned int fCount[256] = {0}; 
unsigned int data_num; 
unsigned int code_size; 
unsigned int last_bit; 
void FrequencyCount(unsigned char*);         //频率统计 
void HuffSelect(HuffNode*, int, int*, int*); //从结点中选出权最小的两个节点 
void HuffmanCodeTable(HuffNode*, HuffCode*); //构造huffman树，生成huffman编码表 
void HuffmanCompress(unsigned char*, unsigned char *, HuffCode*); //压缩数据 
void BitPrint(unsigned char*);               //按位打印结果，用于调试

void main() 
{ 
    int i, j, loop;                               //variable for loop
    HuffNode hfdata[2*DNUM] = {{0, 0, 0, 0, 0}};  //Huffman node 
    HuffCode code_table[256] = {{0, 0}};          //code table will be searched by subscript 
    unsigned char hfcode[2*DNUM];                 //output code 
    time_t time1, time2; 
/*  unsigned char pixel[DNUM] = { 
    1,2,3,4, 1,2,3,4, 1,2,3,4, 1,1,1,1}; 
*/ 
/*  unsigned char pixel[DNUM] = { 
        139,144,149,153,155,155,155,155, 
        144,151,153,156,159,156,156,156, 
        150,155,160,163,158,156,156,156, 
        159,161,162,160,160,159,159,159, 
        159,160,161,162,162,155,155,155, 
        161,161,161,161,160,157,157,157, 
        162,162,161,163,162,157,157,157, 
        162,162,161,161,163,158,158,158};
*/ 
    unsigned char pixel[DNUM] = { //random data 
        141, 101, 126, 111, 163, 112, 133, 156, 
        103, 144, 111, 176, 117, 120, 188, 187, 
        175, 164, 190, 156, 112, 179, 142, 119, 
        140, 111, 127, 186, 196, 190, 189, 127, 
        185, 103, 185, 110, 192, 139, 159, 104, 
        151, 193, 178, 198, 114, 170, 179, 149, 
        124, 149, 165, 108, 141, 176, 113, 164, 
        101, 140, 120, 126, 173, 189, 158, 184}; 
/*  unsigned char pixel[DNUM] = { 
        202, 221, 159, 183, 41, 136, 247, 66, 
        146, 29, 101, 108, 45, 61, 210, 236, 
        90, 130, 54, 66, 132, 206, 119, 232, 
        184, 135, 96, 78, 120, 41, 231, 203, 
        150, 94, 172, 142, 122, 180, 150, 204, 
        232, 121, 180, 221, 3, 207, 115, 147, 
        72, 149, 169, 121, 76, 208, 235, 43, 
        107, 58, 0, 237, 197, 7, 210, 89}; 
*/ 
    FrequencyCount(pixel); 
    time1 = time(NULL); 
    for (loop=0; loop<LOOP; loop++) { 
        //set huffman nodes data and weight, i=0:255, j=1:64 
        for (i=0, j=1, data_num=0; i<256; i++) { 
            if (fCount[i]) { 
                hfdata[j].weight = fCount[i]; 
                hfdata[j++].data = i; 
                data_num ++; 
            } 
        } 
        //build huffman tree and generate huffman code table 
        HuffmanCodeTable(hfdata, code_table); 
        //compress source data to huffman code using code table 
        HuffmanCompress(pixel, hfcode, code_table); 
        //initial hfdata and code_table 
        for (j=0; j<2*DNUM; j++) { 
            hfdata[j].data=0; 
            hfdata[j].lchild=0; 
            hfdata[j].parent=0; 
            hfdata[j].rchild=0; 
            hfdata[j].weight=0; 
        } 
    } 
    time2 = time(NULL); 
    //conclude 
    printf(" 哈夫曼编码压缩图块，压缩报告华中科技大学力学系：李美之 "); 
    printf(" ◎源数据（%d字节）: ", DNUM); 
    for (i=0; i<DNUM; i++) { 
        printf(i%8==7 ? "%02X " : "%02X ", pixel[i]); 
    } 
    printf(" ◎压缩数据（%d字节）: ", code_size); 
    for (i=0; i<code_size; i++) { 
        printf(i%8==7 ? "%02X " : "%02X ", hfcode[i]); 
    } 
    //打印码表 
    printf(" ◎码表－编码字典（%d项） ", data_num); 
    for (i=0; i<256; i++) { 
        if (code_table[i].codelength) { 
            printf("%3d|%02X: ", i, i); 
            for (j=0; j<code_table[i].codelength; j++) { 
                printf("%d", ((code_table[i].code << j)&0x80)>>7); 
            } 
            printf("\t"); 
        } 
    } 
    printf(" ◎压缩率：%2.0f%% \t压缩时间：%.3f毫秒 ",(float)code_size/DNUM * 100, 1E3*(time2-time1)/LOOP); 
}

void BitPrint(unsigned char *hfcode) 
{ 
    int i, j; 
    int endbit = last_bit; 
    unsigned char thebyte; 
    for (i=0; i < code_size-1; i++) { 
        thebyte = hfcode[i]; 
        for (j=0; j<8; j++) { 
            printf("%d", ((thebyte<<j)&0x80)>>7); 
        } 
    } 
    if (last_bit == 7) { 
        endbit = -1; 
    } 
    thebyte = hfcode[i]; 
    for (j=7; j>endbit; j--) { 
        printf("%d", ((thebyte<<(7-j))&0x80)>>7); 
    } 
}

void HuffmanCompress(unsigned char *pixel, unsigned char *hfcode, HuffCode * code_table) 
{ 
    int i, j; 
    int curbit=7; //current bit in _thebyte_ 
    unsigned int bytenum=0; //number of destination code can also be position of byte processed in destination 
    unsigned int ptbyte=0; //position of byte processed in destination 
    unsigned int curlength; //code's length of _curcode_ 
    unsigned char curcode; //current byte's huffman code 
    unsigned char thebyte=0; //destination byte write 
    unsigned char value; //current byte's value (pixel[]) 
    //process every byte 
    for (i=0; i<DNUM; i++) { 
        value = pixel[i]; 
        curcode = (code_table[value]).code; 
        curlength = (code_table[value]).codelength; 
        //move out every bit from curcode to destination 
        for (j=0;j<=curlength;j++) { 
            if ((curcode<<j)&0x80) { 
                thebyte |= (unsigned char)(0x01<<curbit); 
            } 
            curbit --; 
            if (curbit < 0) { 
                hfcode[ptbyte++] = thebyte; 
                thebyte = 0; 
                curbit  = 7; 
                bytenum ++; 
            } 
        } 
    } 
    //think about which bit is the end 
    if (curbit != 7) { 
        hfcode[ptbyte] = thebyte; 
        bytenum ++; 
    } 
    code_size = bytenum; 
    last_bit  = curbit; 
}

void HuffmanCodeTable(HuffNode *hfdata, HuffCode *code_table) 
{ 
    int i, j; //variable for loop 
    int tree_num = 2*data_num - 1; //node of huffman tree 
    int min1, min2; //two minimum weight 
    int p; //the id of parent node 
    unsigned char curcode; //current code being processing 
    int curlength; //current code's length 
    //build huffman tree 
    for (i=data_num; i<tree_num; i++) { 
        HuffSelect(hfdata, i, &min1, &min2); 
        hfdata[min1].parent = i+1; 
        hfdata[min2].parent = i+1; 
        hfdata[i+1].lchild = min1; 
        hfdata[i+1].rchild = min2; 
        hfdata[i+1].weight = hfdata[min1].weight + hfdata[min2].weight; 
    } 
    //generate huffman code 
    //i present the i th code, j present from leaf to root in huffman tree 
    //hfdata[i].data (0:255) is a byte number 
    //编码从叶读到根，按位从高往低压入一个字节，读编码从左向右 
    for (i=1; i<=data_num; i++) { 
        curcode = 0; 
        curlength = 0; 
        for (j=i, p=hfdata[j].parent; p!=0; j=p, p=hfdata[j].parent) { 
            curlength ++; 
            if (j==hfdata[p].lchild) curcode >>= 1; 
            else curcode = (curcode >> 1) | 0x80; //0x80 = 128 = B1000 0000 
        } 
        code_table[hfdata[i].data].code = curcode; 
        code_table[hfdata[i].data].codelength = curlength; 
    } 
}

void HuffSelect(HuffNode *hfdata, int end, int *min1, int *min2) 
{ 
    int i; //variable for loop 
    int s1, s2; 
    HuffNode wath[30]; 
    for (i=0; i<30; i++) { 
        wath[i] = hfdata[i]; 
    } 
    s1 = s2 = 1; 
    while (hfdata[s1].parent) { 
        s1++; 
    } 
    for (i=2; i<=end; i++) { 
        if (hfdata[i].parent == 0 && hfdata[i].weight < hfdata[s1].weight) { 
            s1 = i; 
        } 
    } 
    while (hfdata[s2].parent || s1 == s2) { 
        s2++; 
    } 
    for (i=1; i<=end; i++) { 
        if (hfdata[i].parent ==0 && hfdata[i].weight < hfdata[s2].weight && (i - s1)) { 
            s2 = i; 
        } 
    } 
    *min1 = s1; 
    *min2 = s2; 
}

void FrequencyCount(unsigned char *chs) 
{ 
    int i; 
    for (i=0; i<DNUM; i++) { 
        fCount[*(chs+i)] ++; 
    } 
} 

